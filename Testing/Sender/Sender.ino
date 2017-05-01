#include <Packet.h>
#include <Network.h>
//#include <Logger.h>
#include <Queue.h>


Network net;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.println(" ready");

  delay(4);

  net.setmyID(68);
  net.initNetwork(1);
  //net.setNextHop(1);
  //net.setCoord();
}

void loop() {
  static long opcode = 7;
  // put your main code here, to run repeatedly:
  net.sendOverNetwork(opcode);
  delay(10);
  net.runNetwork();
}
