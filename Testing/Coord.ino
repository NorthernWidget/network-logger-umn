#include <Packet.h>
#include <Network.h>
//#include <Logger.h>
#include <Queue.h>

#define LED           9 // LED positive pin
#define CS1           8
#define CS2           10

Network net;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.println(" ready");

  pinMode(CS2, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(CS2, LOW);
  digitalWrite(LED, LOW);
  delay(4);

  net.initNetwork(1);
  net.setmyID(1);
  net.setCoord();
}

void loop() {
  static long opcode;
  // put your main code here, to run repeatedly:
  net.runNetwork();
  Serial.println(opcode);
  for(int i = 0; i<100; i++);
  
}