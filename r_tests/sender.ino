#include <Ethernet.h>
#include <Dhcp.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <RFM69.h>
#include <SPI.h>

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
#define CS1           8
#define CS2           10

RFM69 radio;

void setup()
{
    pinMode(CS2, OUTPUT);
    digitalWrite(CS2, LOW);
    radio.initialize(FREQUENCY, TONODEID, NETWORKID);
    radio.setHighPower();
}

void loop()
{
    BYTE data[255];
    for(int i = 0; i < 255; i++)
    {
        data[i] = 0xaa;
    }
    radio.send(TONODEID,data, 255);
    while(1);
}


