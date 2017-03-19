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
    //TODO: check usb connection and (un)set amCoord then (do nothing)broadcast i am coord
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
    createPacket(ASKFORCOORD, this.myID, BROADCASTADDRESS, 0, NULL, p);
    sendPacket(p);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network saying that 
 * we have a connection to coord
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::foundCoord()
{
    Packet* p = new Packet();
    createPacket(IHAVECOORD, this.myID, BROADCASTADDRESS, 0, NULL, p);
    sendPacket(p);
}

retVal runNetwork()
{
    while (tbd) {
        if (!(this.haveCoord || this.amCoord)) {
            lookForCoord();
            delay(DEFAULTWAITTIME); //Times to be determined. DEFAULTWAITTIME is just a placeholder for time being
        }
        Packet* p = new Packet();
        uint8_t misscount = 0;
        while (missCount < MAXMISSCOUNT) {
            if (readPacket(p) == SUCCESS)
                receivedPacket(p);
            else
                misscount++;
            delay(DEFAULTWAITTIME);
        }
        if (this.haveCoord) {
            //TODO: forward data and send your own data
        }
        else if (this.amCoord) {
            //TODO: upload to server
        }
        else {
            //TODO: choose path and found coord and set have coord
        }
    }
}

retVal receivedPacket(Packet* p)
{
    uint8_t opCode = p->getopCode();
    if ((this.haveCoord || this.amCoord) && opCode == DTRANSMISSION) {
        //add the data to a queue of packets to be retransmited
        p->setdAddr(this.nextHop);
        bool ok = inQueue.enqueue(p);
        if (ok)
            return INQUEUED;
        return DROPPED;
    }
    else if (!(this.haveCoord || this.amCoord) && (opCode == IHAVECOORD || opCode == IAMCOORD)) {
        //add the offer to a queue of packets to choose the one with the best RSSI later
        bool ok = inQueue.enqueue(p);
        if (ok)
            return INQUEUED;
        return DROPPED;
    }
  else if(!(this.haveCoord || this.amCoord) && (opCode == DTRANSMISSION || opCode == UAREPATH){
        //This is the recipt of a packet we should only get if we have access to the coordinator,
        //so tell the sender that we don't have coordinator access.
        Packet* s;
        unit8_t numAttempts = 0;
        createPacket(DROPPEDCOORD, this.myId, p->getsAddr(), 0, NULL, s);
        while (numAttempts < MAXATTEMPTS && sendPacket(s) == NOACK)
            numAttempts++;
        if (numAttempts == MAXATTEMPTS) {
            outQueue.enqueue(s);
            return OUTQUEUED;
        }
        return SUCCESS;
  }
  else if((this.haveCoord || this.amCoord) && opCode == DROPPEDCOORD && p->getsAddr() == this.nextHop){
        //Our next hop no longer has coordinator access so we can brodcast it to our leaves
        Packet* s;
        this.leafindex = 0;
        createPacket(DROPPEDCOORD, this.myId, BROADCASTADDRESS, 0, NULL, s);
        sendPacket(s);
        return SUCCESS;
  }
 	else if(this.amCoord && opCode == ASKFORCOORD){
        //This is the coordinator and someone requested access, let them know I can hear them
        Packet* s;
        uint8_t numAttempts = 0;
        this.leafindex = 0;
        createPacket(IAMCOORD, this.myId, p->getsAddr(), 0, NULL, s);
        while (numAttempts < MAXATTEMPTS && sendPacket(s) == NOACK)
            numAttempts++;
        if (numAttempts == MAXATTEMPTS) {
            outQueue.enqueue(s);
            return OUTQUEUED;
        }
        return SUCCESS;
  }
  else if(this.haveCoord && opCode == ASKFORCOORD){
        //This is has coordinator access and someone requested access, let them know I can hear them
        Packet* s;
        uint8_t numAttempts = 0;
        this.leafindex = 0;
        createPacket(IHAVECOORD, this.myId, p->getsAddr(), 0, NULL, s);
        while (numAttempts < MAXATTEMPTS && sendPacket(s) == NOACK)
            numAttempts++;
        if (numAttempts == MAXATTEMPTS) {
            outQueue.enqueue(s);
            return OUTQUEUED;
        }
        return SUCCESS;
  }
  else if((this.haveCoord || this.amCoord) && opCode == UAREPATH){
        //Someone is says they are using this address as next hop it should be stored
        for (int i = 0; i < leafindex; i++) {
            if (this.leaves[leafindex] == p->getAddr())
                return IGNORED;
        }
        this.leaves[leafindex] = p->getsAddr();
        this.leafindex++;
        return SUCCESS;
  }
  return IGNORED;
}
