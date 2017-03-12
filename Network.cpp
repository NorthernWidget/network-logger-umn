
/*
 * =====================================================================================
 *
 *       Filename:  Network.cpp
 *
 *    Description:  Protocol used for radio communication using the ALogger
 *
 *        Version:  1.0
 *        Created:  02/01/2017 11:26:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luke Cesarz, Jeff Worm, David Nickel, Robert Schulz, Ying
 *                  Yang
 *     University:  University of Minnesota TC
 *
 * =====================================================================================
 *
## LICENSE: GNU GPL v3

Network.h is part of Logger, an Arduino library written by Andrew D. Wickert
and Chad T. Sandell.

Copyright (C) 2011-2015, Andrew D. Wickert<br>
Copyright (C) 2016, Andrew D. Wickert and Chad T. Sandell

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Network.h>

//Default Constructor
void Network::Network() {}

retVal Network::initNetwork(uint8_t networkID)
{
    //TODO: set myID from EEPROM
    this.networkID = networkID;
    digitalWrite(radioChipSelectPin, LOW);
    //TODO: set other chip selects high
    radio.initialize(RF69_915MHZ, this.myID, this.networkID);
    radio.writeReg(0x03, 0x0A);
    radio.writeReg(0x04, 0x00);
    radio.setHighPower();
    //TODO: check usb connection and (un)set amCoord then broadcast i am coord
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Fills a packet class object with data from a packet that
 * that has been received. Returns NOMESSAGE if there was
 * nothing to be received, ow returns SUCCESS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
retVal Network::readPacket(Packet* p)
{
    digitalWrite(radioChipSelectPin, LOW);
    //TODO: set other chip selects high
    if (this.radio.receiveDone()) {
        if (this.radio.ACKRequested())
            this.radio.sendACK();
        p->setdSize(this.radio.PAYLOADLEN - 1);
        p->setsAddr(this.radio.SENDERID);
        p->setdAddr(this.radio.TARGETID);
        p->setopCode(this.radio.DATA[0]);
        p->setRSSI(this.radio.readRSSI());
        for (int i = 0; i < p->getdSize(); i++) {
            p->setdata(this.radio.DATA[i + 1], i);
        }
        for (int i = p->getdSize(); i < MAXDATASIZE; i++) {
            p->setdata(0, i);
        }
        return SUCCESS;
    }
    return NOMESSAGE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends the packet to the node identified by p->getdAddr()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
retVal Network::sendPacket(Packet* p)
{
    digitalWrite(radioChipSelectPin, LOW);
    //TODO: set other chip selects high
    uint8_t data[p->getdSize() + 1];
    data[0] = p->getopCode();
    for (int i = 0; i < p->getdSize(); i++) {
        data[i + 1] = p->getdata(i);
    }
    if (this.useAck && p->getdAddr() != BROADCASTADDRESS) {
        if (this.radio.sendWithRetry(p->getdAddr(), data, p->getdSize() + 1)) {
            return SUCCESS;
        }
        else {
            return NOACK;
        }
    }
    else {
        this.radio.send(p->getdAddr(), data, p->getdSize() + 1);
        return SUCCESS;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Fills the packet pointed to by p with the info passed in
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::createPacket(uint8_t opCode, uint8_t sAddr, uint8_t dAddr, uint8_t dSize, uint8_t data[], Packet* p)
{
    p->setopCode(opCode);
    p->setsAddr(sAddr);
    p->setdAddr(dAddr);
    p->setdSize(dSize);
    for (int i = 0; i < dSize; i++)
        p->setdata(data[i], i);
    for (int i = dSize; i < MAXDATASIZE; i++)
        p->setdata(0, i);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network looking for 
 * the coordinator node
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::lookForCoord()
{
    Packet* p = new Packet();
    createPacket(ASKFORCOORD, this.myID, BROADCASTADDRESS, 0, null, p);
    sendPacket(p);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network saying that 
 * we have a connection to coord
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::foundCoord()
{
    Packet* p = new Packet();
    createPacket(IHAVECOORD, this.myID, BROADCASTADDRESS, 0, null, p);
    sendPacket(p);
}
