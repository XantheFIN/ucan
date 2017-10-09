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
	 * Queues message for transmission.
	 * @param aMsg message to be queued
	 * @param aTimeoutMs how long to wait if transmit queue is full
	 * @return true if successfully queued
	 */
	virtual bool sendMessage(SharedCanMessage aMsg, uint32_t aTimeoutMs) = 0;

	/**
	 * Returns number of messages in receive buffer.
	 * @return number of messages in buffer
	 */
	virtual int numReceivedMessagesAvailable() = 0;

	/**
	 * Retrieves message from receive buffer.
	 * @param aMsg message received
	 * @param aTimeoutMs how long to wait if receive queue is empty
	 * @return true of message successfully dequeued
	 */
	virtual bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs) = 0;

	/**
	 * Returns number of messages in transmit acknowledge buffer
	 * @return number of messages in buffer
	 */
	virtual int numSentMessagesAvailable() = 0;

	/**
	 * Retrieves message from transmit acknowledge buffer.
	 * @param aMsg message transmitted
	 * @param aTimeoutMs how long to wait if transmit acknowledge queue is empty
	 * @return true of message successfully dequeued
	 */
	virtual bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs) = 0;

	/**
	 * Getter for error code
	 * @return error
	 */
	virtual int getErrorCode() = 0;

	/**
	 * Retrieves error description and secondary error code
	 * @param aErrorCode code for which description is requested
	 * @param aErrorString string for error description
	 * @param aSecondaryCode secondary error code
	 */
	virtual void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode) = 0;

	/**
	 * Retrieves CAN error counters (according to CAN standard)
	 * @param aTxErrorCounter transmit error counter
	 * @param aRxErrorCounter receiver error counter
	 */
	virtual void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter) = 0;
};

#endif /* CAN_PORT_H_ */
