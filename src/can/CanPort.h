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

#ifndef CAN_PORT_H_
#define CAN_PORT_H_

#ifdef SCONS_TARGET_WIN
#include <stdint.h>
#endif

#include <boost/shared_ptr.hpp>

#include "CanMessage.h"

class CanPort;
typedef boost::shared_ptr<CanPort> SharedCanPort;

class CanPort {

public:

	/**
	 * Send message.
	 * Queues CAN message into transmit buffer. A transaction id is associated with
	 * the pending transmission, and can be used in conjunction with getSendAcknMessage()
	 * to subsequently confirm a successful transmission.
	 *
	 * @param aMsg CAN message
	 * @param aTransactionId id associated with transmission (set to nullptr if not used)
	 * @return true if successful
	 */
	virtual bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId) = 0;

	/**
	 * Get number of messages in receive buffer.
	 * Received messages are retrieved by means of getReceivedMessage().
	 * @param number of messages in receive buffer
	 */
	virtual int numReceivedMessagesAvailable() = 0;

	/**
	 * Retrieve message from receive buffer.
	 *
	 * @param aMsg object to store received message
	 * @param aTimeoutMs time to wait for received message
	 * @return true when valid message is returned by function
	 */
	virtual bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs) = 0;

	/**
	 * Get number of successfully sent messages stored in transmit acknowledge buffer.
	 * Messages transmitted are stored in the transmit acknowledge buffer and can
	 * be retrieved my means of getSendAcknMessage().
	 * @param number of transmit acknowledge buffer
	 */
	virtual int numSendAcknMessagesAvailable() = 0;

	/**
	 * Retrieve message from transmit acknowledge buffer.
	 * If a non-zero transaction id is specified, the function will block until
	 * the message corresponding to the transaction id as been transmitted, and
	 * stored in the transmit acknowledge buffer, or until the specified timeout
	 * expires.
	 *
	 * @param aMsg object to store transmitted message
	 * @param aTransactionId desired transaction id
	 * @param aTimeoutMs time to wait for received message
	 * @return true when valid message is returned by function
	 */
	virtual bool getSendAcknMessage(SharedCanMessage& aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs) = 0;

	/**
	 *  Getter for error code corresponding to last error that occurred.
	 *  Use getErrorDescription() to obtain textual description of error code.
	 *  @return error code
	 */
	virtual int getErrorCode() = 0;

	/**
	 * Convert numerical error code to error description.
	 *
	 * @param aErrorCode error code obtained from getErrorCoder()
	 * @param aErrorString textual error description
	 * @param aSecondaryCode secondary error code (if applicable)
	 */
	virtual void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode) = 0;

	/**
	 *  Return CAN error counters.
	 *  @param aTxErrorCounter number of transmit errors
	 *  @param aRxErrorCounter number of receive errors
	 */
	virtual void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter) = 0;
};

#endif /* CAN_PORT_H_ */
