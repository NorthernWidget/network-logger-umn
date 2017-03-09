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
#ifndef Network_h
#define Network_h
//RetVal
#define retVal uint8_t
//Opcodes
#define DTRANSMISSION 0x01
#define DROPPEDCOORD 0x02
#define ASKFORCOORD 0x03
#define IHAVECOORD 0x04
#define IAMCOORD 0x05
#define UAREPATH 0x06

//Function to make an opcode a broadcast Opcode
#define BroadCast(x) 0x80 | (x)


/*Defining the packet*/
#define MAXPACKETSIZE 255

class Packet
{
    public:
        Packet();

        //getters
        uint8_t getopCode(){ return opcode; }
        uint8_t getsAddr(){ return sAddr; }
        uint8_t getdAddr(){ return dAddr; }
        uint8_t getpSize(){ return pSize; }
        uint8_t getData(){ return data; }

        //setters
        void setopCode(uint8_t code){ self.opCode=code; }
        void setsAddr(uint8_t sAddr){ self.sAddr=sAddr; }
        void setdAddr(uint8_t dAddr){ self.dAddr=dAddr; }
        void setpSize(uint8_t pSize){ self.pSize=pSize; }
        void setdata(uint8_t data){ self.data=data; }

    private:
        //parts of the packet structure
        uint8_t opCode;         //Operation we are performing (data transmission, I have coord, etc.)
        uint8_t sAddr;
        uint8_t dAddr;          //Destination address
        uint8_t pSize;          //Packet size
        uint8_t data[255];      //The data (optional)
}

//These functions are just placeholders currently, as this code is currently
//in development. 
//
class Network
{
    public:
        //default construct
        Network();
        //Network Functions:

        //inits the network (main constructor)
        retVal      initNetwork();
        //fill a packet class object with data that was received
        retVal      readPacket();
        //send a packet 
        retVal      sendPacket(Packet *p);
        //fill a packet class object with data to send
        retVal      createPacket(uint8_t opCode, uint8_t sAddr, uint8_t dAddr, uint8_t pSize, uint8_t data[], Packet* p);
        //returns 1 if the chip running this fnc is coord
        retVal      checkIfCoord();
        //finds a path to coord
        retVal      lookForCoord();
        //inform network that we have coord connection
        retVal      foundCoord();
        //decide what to do with a received packet
        retVal      receivedPacket();

        void setPath(uint8_t p[])
    private:
        uint8_t Path[255];
        uint8_t nextHop;
        bool    haveCoord;
        bool    amCoord;
}   

#endif

