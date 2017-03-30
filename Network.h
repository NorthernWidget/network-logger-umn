/*
 * =====================================================================================
 *
 *       Filename:  Network.h
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
#include <Logger.h>
#include <RFM69.h>
#include <SPI.h>
#ifndef Network_h
#define Network_h
//radio globals
//#define FREQUENCY     RF69_915MHZ
const uint8_t radioChipSelectPin = 10 //TODO:this is just a random number, change it when the board layout is done
    //RetVal
    enum retVal { SUCCESS, 		//Generic Success
        		  FAIL, 		//Generic Failure
        		  NOMESSAGE, 	//No message received to read in
        		  NOACK, 		//An ACK was requested and not received
        		  IGNORED, 		//A message was received that we don't need to do anything for
        		  INQUEUED, 	//Info added to a queue of received info to be handled later
        		  OUTQUEUED, 	//Info added to a queue to be retransmited later
        		  DROPPED }; 	//Queue was full and the info was lost
//Opcodes
#define DTRANSMISSION 0x01 //dataOwner, data0, data1, ... , dataN use dSize to determin N
#define DROPPEDCOORD 0x02 //no data
#define ASKFORCOORD 0x03 //no data
#define IHAVECOORD 0x04 //no data
#define IAMCOORD 0x05 //no data
#define UAREPATH 0x06 //no data

/*Defining the packet*/
#define MAXDATASIZE 60
#define BROADCASTADDRESS 255
#define DEFAULTWAITTIME 10000 //10 seconds TBD
#define MAXMISSCOUNT 5 //TBD
#define MAXATTEMPTS 5 //TBD

//These functions are just placeholders currently, as this code is currently
//in development.
//
class Network {
public:
    //default construct
    Network();

    //Network Functions:
    //inits the network (main constructor) and sets the network ID
    void initNetwork(uint8_t networkID);

    //handles the every wake network events
    void runNetwork();

    //The data to the network class for forwarding to the coordinator
    bool sendOverNetwork(float data){ return dataQueue.enqueue(data) };

private:
    //fill a packet class object with data that was received
    retVal readPacket(Packet* p);

    //send a packet
    retVal sendPacket(Packet* p);

    //fill a packet class object with data to send
    void createPacket(uint8_t opCode, uint8_t sAddr, uint8_t dAddr, uint8_t dSize, uint8_t data[], Packet* p);

    //returns true if the chip running this fnc is coord
    bool checkIfCoord(){ return this.amCoord };

    //finds a path to coord
    void lookForCoord();

    //inform network that we have coord connection
    void foundCoord();

    //decide what to do with a received packet
    retVal receivedPacket(Packet* p);

    //void setPath(uint8_t p[])

    RFM69 radio; //the radio object
    uint8_t myID; //TODO:set this from EEPROM in initialization
    bool useAck = true; //do we want acks
    //bool 		encrypt = false;   //TODO:this will be implemented last
    //char 		*encryptKey;       //TODO:this will be implemented last
    uint8_t networkID = 0;
    uint8_t leaves[255];
    uint8_t leafindex = 0;
    uint8_t nextHop;
    uint16_t currentRSSI = 0;
    bool haveCoord;
    bool amCoord;
    bool reconnected = false;
    Queue<float> dataQueue;
}

#endif
