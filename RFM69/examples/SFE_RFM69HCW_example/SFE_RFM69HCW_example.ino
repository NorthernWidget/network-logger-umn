#include <Ethernet.h>
#include <Dhcp.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>

// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio1 module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)

// RFM69 frequency, uncomment the frequency of your module:

//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        false // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED           9 // LED positive pin
#define CS1           2
#define CS2           3
//#define GND           8 // LED ground pin

// Create a library object for our RFM69HCW module:

RFM69 radio1;
RFM69 radio2;

void setup()
{
  // Open a serial port so we can send keystrokes to the module:
  
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  // Set up the indicator LED (optional):
  
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  //pinMode(GND,OUTPUT);
  //digitalWrite(GND,LOW);
    
  // Initialize the RFM69HCW:

  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, LOW);
  radio1.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio1.setHighPower(); // Always use this for RFM69HCW
  digitalWrite(CS1, LOW);
  digitalWrite(CS2, HIGH);
  radio2.initialize(FREQUENCY, TONODEID, NETWORKID);
  radio2.setHighPower();

  // Turn on encryption if desired:
  
  if (ENCRYPT)
    radio1.encrypt(ENCRYPTKEY);
    radio2.encrypt(ENCRYPTKEY);
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:
  
  static char sendbuffer[62];
  static int sendlength = 0;

  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).
  
  // If there is any serial input, add it to the buffer:

  if (Serial.available() > 0)
  {
    digitalWrite(CS1, HIGH);
    digitalWrite(CS2, LOW);
    char input = Serial.read();
    
    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    // If the input is a carriage return, or the buffer is full:
    
    if ((input == '\r') || (sendlength == 61)) // CR or buffer full
    {
      // Send the packet!


      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(": [");
      for (byte i = 0; i < sendlength; i++)
        Serial.print(sendbuffer[i]);
      Serial.println("]");
      
      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():
      
      if (USEACK)
      {
        if (radio1.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received :(");
      }

      // If you don't need acknowledgements, just use send():
      
      else // don't use ACK
      {
        radio1.send(TONODEID, sendbuffer, sendlength);
      }
      
      sendlength = 0; // reset the packet
//      Blink(LED,10);
    }
  }

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio2.receiveDone()) // Got one!
  {
    digitalWrite(CS1, LOW);
    digitalWrite(CS2, HIGH);
    // Print out the information:
    
    Serial.print("received from node ");
    Serial.print(radio2.SENDERID, DEC);
    Serial.print(": [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    char arry[radio2.DATALEN];
    for (byte i = 0; i < radio2.DATALEN; i++){
      Serial.print((char)radio2.DATA[i]);
      arry[i] = radio2.DATA[i];
    }

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.
    
    Serial.print("], RSSI ");
    Serial.println(radio2.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)
    
    if (radio2.ACKRequested())
    {
      radio1.sendACK();
      Serial.println("ACK sent");
    }

    if(arry[0] == 'o' && arry[1] == 'n'){
      digitalWrite(LED, HIGH);
    }
    else{
      digitalWrite(LED, LOW);
    }
  }
}
