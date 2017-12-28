/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2017 by CodeSkin LLC, www.codeskin.com.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/system/error_code.hpp>

#include "CanAsyncWrapper.h"

#pragma once

template <typename CanAsyncImplementation = CanAsyncWrapper>
class CanAsyncService
		: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;

	explicit CanAsyncService(boost::asio::io_service &io_service)
	: boost::asio::io_service::service(io_service),
	  async_work_1_(new boost::asio::io_service::work(async_io_service_1_)),
	  async_thread_1_(boost::bind(&boost::asio::io_service::run, &async_io_service_1_)),
	  async_work_2_(new boost::asio::io_service::work(async_io_service_2_)),
	  async_thread_2_(boost::bind(&boost::asio::io_service::run, &async_io_service_2_))
	{
	}

	typedef boost::shared_ptr<CanAsyncImplementation> implementation_type;

	void construct(implementation_type &impl){
		impl.reset(new CanAsyncImplementation());
	}

	void destroy(implementation_type &impl){
		//impl->destroy();
		//impl->close();
		impl.reset();
	}

	bool open(implementation_type &impl, SharedCanAdapter aCanAdapter){
		return impl->open(aCanAdapter);
	}

	void close(implementation_type &impl){
		impl->close();
	}

	bool sendMessage(implementation_type &impl, const SharedCanMessage &aMsg){
		uint16_t transactionId;
		return impl->sendMessage(aMsg, &transactionId);
	}

	bool getReceivedMessage(implementation_type &impl, SharedCanMessage &aMsg, uint32_t aTimeoutMs){
		return impl->getReceivedMessage(aMsg, aTimeoutMs);
	}

	bool getSendAcknMessage(implementation_type &impl, SharedCanMessage &aMsg, uint32_t aTimeoutMs){
		return impl->getSendAcknMessage(aMsg, 0, aTimeoutMs);
	}

	template <typename Handler>
	class GetReceivedMessageOperation
	{
	public:
		GetReceivedMessageOperation(implementation_type &impl,
				boost::asio::io_service &io_service,
				Handler handler)
		: impl_(impl),
		  io_service_(io_service),
		  work_(io_service),
		  handler_(handler)
		{
		}

		void operator() () const
		{
			bool keepTrying = true;
			do {
				implementation_type impl = impl_.lock();
				if(impl && impl->isOpen())
				{
					boost::system::error_code ec;
					SharedCanMessage ndu;
					if(impl->getReceivedMessage(ndu, 100)){
						this->io_service_.post(boost::asio::detail::bind_handler(
								handler_, ec, ndu));
						keepTrying = false;
					}
				}
				else
				{
					SharedCanMessage ndu;
					this->io_service_.post(boost::asio::detail::bind_handler(
							handler_, boost::asio::error::operation_aborted, ndu));
					keepTrying = false;
				}
			} while (keepTrying);
		}

	private:
		boost::weak_ptr<CanAsyncImplementation> impl_;
		boost::asio::io_service &io_service_;
		boost::asio::io_service::work work_;
		Handler handler_;
	};

	template <typename Handler>
	void asyncGetReceivedMessage(implementation_type &impl, Handler handler)
	{
		this->async_io_service_1_.post(GetReceivedMessageOperation<Handler>(impl,
				this->get_io_service(), handler));
	}

	template <typename Handler>
	class GetSendAcknMessageOperation
	{
	public:
		GetSendAcknMessageOperation(implementation_type &impl,
				boost::asio::io_service &io_service,
				Handler handler)
		: impl_(impl),
		  io_service_(io_service),
		  work_(io_service),
		  handler_(handler)
		{
		}

		void operator() () const
		{
			bool keepTrying = true;
			do {
				implementation_type impl = impl_.lock();
				if(impl && impl->isOpen())
				{
					boost::system::error_code ec;
					SharedCanMessage ndu;
					if(impl->getSendAcknMessage(ndu, 0, 100)){
						this->io_service_.post(boost::asio::detail::bind_handler(
								handler_, ec, ndu));
						keepTrying = false;
					}
				}
				else
				{
					SharedCanMessage ndu;
					this->io_service_.post(boost::asio::detail::bind_handler(
							handler_, boost::asio::error::operation_aborted, ndu));
					keepTrying = false;
				}
			} while (keepTrying);
		}

	private:
		boost::weak_ptr<CanAsyncImplementation> impl_;
		boost::asio::io_service &io_service_;
		boost::asio::io_service::work work_;
		Handler handler_;
	};

	template <typename Handler>
	void asyncGetSendAcknMessage(implementation_type &impl, Handler handler)
	{
		this->async_io_service_2_.post(GetSendAcknMessageOperation<Handler>(impl,
				this->get_io_service(), handler));
	}

private:
	void shutdown_service()
	{
		async_work_1_.reset();
		async_io_service_1_.stop();
		async_thread_1_.join();

		async_work_2_.reset();
		async_io_service_2_.stop();
		async_thread_2_.join();
	}

	boost::asio::io_service async_io_service_1_;
	boost::scoped_ptr<boost::asio::io_service::work> async_work_1_;
	boost::thread async_thread_1_;

	boost::asio::io_service async_io_service_2_;
	boost::scoped_ptr<boost::asio::io_service::work> async_work_2_;
	boost::thread async_thread_2_;
};

template <typename CanAsyncImplementation>
boost::asio::io_service::id CanAsyncService<CanAsyncImplementation>::id;
