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

#ifndef Network_h
#define Network_h

/*Defining the packet*/
#define MAXPACKETSIZE 255

class Packet
{
    public:
        Packet();
        
        unsigned char* getopCode(){ return opcode; }
        unsigned char* getsAddr(){ return sAddr; }
        unsigned char* getdAddr(){ return dAddr; }
        unsigned char* getpSize(){ return pSize; }
        unsigned char* getData(){ return data; }
    private:
        unsigned char *opCode;
        unsigned char *sAddr;
        unsigned char *dAddr;
        unsigned char *pSize;
        unsigned char data[251];
}

//These functions are just placeholders currently, as this code is currently
//in development. 
//
class Network
{
    public:
        Network();

        void initNetwork();
}

#endif

