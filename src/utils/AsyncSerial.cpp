/*
 * Author: Terraneo Federico
 * Distributed under the Boost Software License, Version 1.0.
 * Created on September 7, 2009, 10:46 AM
 */

#include "AsyncSerial.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>

//
//Class AsyncSerial
//

class AsyncSerialImpl: private boost::noncopyable
{
public:
    AsyncSerialImpl(): io(), port(io), backgroundThread(), open(false){}

    boost::asio::io_service io; ///< Io service object
    boost::asio::serial_port port; ///< Serial port object
    boost::thread backgroundThread; ///< Thread that runs read/write operations
    boost::atomic_bool open; ///< True if port open

    /// Data are queued here before they go in writeBuffer
    std::vector<char> writeQueue;
    boost::shared_array<char> writeBuffer; ///< Data being written
    size_t writeBufferSize; ///< Size of writeBuffer
    boost::mutex writeQueueMutex; ///< Mutex for access to writeQueue
    char readBuffer[AsyncSerial::readBufferSize]; ///< data being read

    /// Read complete callback
    boost::function<void (const char*, size_t)> callback;
};

AsyncSerial::AsyncSerial(): pimpl(new AsyncSerialImpl)
{

}

AsyncSerial::AsyncSerial(const std::string& devname, unsigned int baud_rate,
        boost::asio::serial_port_base::parity opt_parity,
		boost::asio::serial_port_base::character_size opt_csize,
		boost::asio::serial_port_base::flow_control opt_flow,
		boost::asio::serial_port_base::stop_bits opt_stop)
        : pimpl(new AsyncSerialImpl)
{
    open(devname,baud_rate,opt_parity,opt_csize,opt_flow,opt_stop);
}

void AsyncSerial::open(const std::string& devname, unsigned int baud_rate,
		boost::asio::serial_port_base::parity opt_parity,
		boost::asio::serial_port_base::character_size opt_csize,
		boost::asio::serial_port_base::flow_control opt_flow,
		boost::asio::serial_port_base::stop_bits opt_stop)
{
    if(pimpl->open) close();

    pimpl->port.open(devname);
    pimpl->port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    pimpl->port.set_option(opt_parity);
    pimpl->port.set_option(opt_csize);
    pimpl->port.set_option(opt_flow);
    pimpl->port.set_option(opt_stop);

#ifdef SCONS_TARGET_WIN
//    boost::asio::serial_port::native_type handle = pimpl->port.native();
//    EscapeCommFunction( handle, SETDTR );
//    EscapeCommFunction( handle, CLRDTR );
//    EscapeCommFunction( handle, SETRTS );
//    EscapeCommFunction( handle, CLRRTS );
#endif

    //This gives some work to the io_service before it is started
    pimpl->io.post(boost::bind(&AsyncSerial::doRead, this));

    boost::thread t(boost::bind(&boost::asio::io_service::run, &pimpl->io));
    pimpl->backgroundThread.swap(t);
    pimpl->open = true; //Port is now open
}

bool AsyncSerial::setBaudrate(unsigned int baud_rate) const
{
	if(!pimpl->open)
	{
		return false;
	}
	pimpl->port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
	return true;
}

void AsyncSerial::close()
{
    pimpl->io.post(boost::bind(&AsyncSerial::doClose, this));
    pimpl->backgroundThread.join();
    pimpl->io.reset();
}

void AsyncSerial::write(const std::vector<char>& data)
{
	if(!pimpl->open) return;
    {
    	boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
        pimpl->writeQueue.insert(pimpl->writeQueue.end(),data.begin(),
                data.end());
    }
    pimpl->io.post(boost::bind(&AsyncSerial::doWrite, this));
}

AsyncSerial::~AsyncSerial()
{
	try {
		close();
	} catch(...)
	{
		//Don't throw from a destructor
	}
}

void AsyncSerial::doRead()
{
	if(!pimpl->open) return;

    pimpl->port.async_read_some(boost::asio::buffer(pimpl->readBuffer,readBufferSize),
            boost::bind(&AsyncSerial::readEnd,
            this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void AsyncSerial::readEnd(const boost::system::error_code& error,
        size_t bytes_transferred)
{
    if(error)
    {
        //error can be true even because the serial port was closed.
        //In this case it is not a real error, so ignore
        if(pimpl->open)
        {
            doClose();
        }
    } else {
        if(pimpl->callback) pimpl->callback(pimpl->readBuffer,
                bytes_transferred);
        doRead();
    }
}

void AsyncSerial::doWrite()
{
	if(!pimpl->open) return;

    //If a write operation is already in progress, do nothing
    if(pimpl->writeBuffer==0)
    {
    	boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
        pimpl->writeBufferSize=pimpl->writeQueue.size();
        pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
        copy(pimpl->writeQueue.begin(),pimpl->writeQueue.end(),
                pimpl->writeBuffer.get());
        pimpl->writeQueue.clear();
        async_write(pimpl->port, boost::asio::buffer(pimpl->writeBuffer.get(),
                pimpl->writeBufferSize),
                boost::bind(&AsyncSerial::writeEnd, this, boost::asio::placeholders::error));
    }
}

void AsyncSerial::writeEnd(const boost::system::error_code& error)
{
    if(!error)
    {
    	boost::lock_guard<boost::mutex> l(pimpl->writeQueueMutex);
        if(pimpl->writeQueue.empty())
        {
            pimpl->writeBuffer.reset();
            pimpl->writeBufferSize=0;
            
            return;
        }
        pimpl->writeBufferSize=pimpl->writeQueue.size();
        pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
        copy(pimpl->writeQueue.begin(),pimpl->writeQueue.end(),
                pimpl->writeBuffer.get());
        pimpl->writeQueue.clear();
        async_write(pimpl->port, boost::asio::buffer(pimpl->writeBuffer.get(),
                pimpl->writeBufferSize),
                boost::bind(&AsyncSerial::writeEnd, this, boost::asio::placeholders::error));
    } else {
        if(pimpl->open)
        {
            doClose();
        }
    }
}

void AsyncSerial::doClose()
{
    pimpl->open = false;
    pimpl->port.cancel();
    pimpl->port.close();
}

void AsyncSerial::setReadCallback(const boost::function<void (const char*, size_t)>& callback)
{
    pimpl->callback = callback;
}

void AsyncSerial::clearReadCallback()
{
    pimpl->callback.clear();
}

//
//Class CallbackAsyncSerial
//

CallbackAsyncSerial::CallbackAsyncSerial(): AsyncSerial()
{

}

CallbackAsyncSerial::CallbackAsyncSerial(const std::string& devname,
        unsigned int baud_rate,
        boost::asio::serial_port_base::parity opt_parity,
        boost::asio::serial_port_base::character_size opt_csize,
        boost::asio::serial_port_base::flow_control opt_flow,
        boost::asio::serial_port_base::stop_bits opt_stop)
        :AsyncSerial(devname,baud_rate,opt_parity,opt_csize,opt_flow,opt_stop)
{

}

void CallbackAsyncSerial::setCallback(const
        boost::function<void (const char*, size_t)>& callback)
{
    setReadCallback(callback);
}

void CallbackAsyncSerial::clearCallback()
{
    clearReadCallback();
}

CallbackAsyncSerial::~CallbackAsyncSerial()
{
    clearReadCallback();
}
