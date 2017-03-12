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
#include <Network.h>

void Network::Network(){}

retVal Network::readPacket(Packet *p){
  if(this.radio.receiveDone())
  {
      p->setdSize(this.radio.PAYLOADLEN-1);
      p->setsAddr(this.radio.SENDERID);
      p->setdAddr(this.radio.TARGETID);
      p->setopCode(this.radio.DATA[0]);
      for(int i = 0; i < p->getdSize(); i++){
        p->setdata(this.radio.DATA[i+1],i);
      }
      for(int i = p->getdSize(); i < 60; i++){
        p->setdata(0,i);
      }
      return SUCCESS;
  }
  return NOMESSAGE;
}

retVal Network::sendPacket(Packet *p){
  uint8_t data[p->getdSize()+1];
  data[0] = p->getopCode();
  for(int i = 0; i < p->getdSize(); i++){
    data[i+1] = p->getdata(i);
  }
  if(this.useAck){
    if(this.radio.sendWithRetry(p->getdAddr(),data, p->getdSize()+1)){
      return SUCCESS;
    }
    else{
      return NOACK;
    }
  }
  else{
    this.radio.send(p->getdAddr(),data, p->getdSize()+1);
    return SUCCESS;
  }
}
