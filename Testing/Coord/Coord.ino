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
  pinMode(3, INPUT);

  digitalWrite(CS2, LOW);
  digitalWrite(LED, LOW);
  delay(4);

  net.setmyID(1);
  net.setCoord();
  //Serial.println("1");
  //delay(5);
  net.initNetwork(1);
  //Serial.println("2");
}

void loop() {
  long opcode = 100;
  //Serial.println(digitalRead(2));
  // put your main code here, to run repeatedly:
  Serial.println("enter run");
  net.runNetwork();
  Serial.println("exit run");
//  delay(10);
}
