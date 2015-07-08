#include "MQTT/MQTT.h"


void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/
//MQTT client("server_name", 1883, callback);
byte server[] = { 192,168,1,116 };
MQTT client(server, 1883, callback);
 
// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    if (message.equals("RED"))    
        RGB.color(255, 0, 0);
    else if (message.equals("GREEN"))    
        RGB.color(0, 255, 0);
    else if (message.equals("BLUE"))    
        RGB.color(0, 0, 255);
    else    
        RGB.color(255, 255, 255);
    delay(1000);
}

int ldrPin = A0;
char s[5]="";
// allow us to use itoa() in this scope
extern char* itoa(int a, char* buffer, unsigned char radix);
unsigned long timestamp;
unsigned long interval = 600000; // 10 minute intervals

void setup() {
    RGB.control(true);
    
    // connect to the server
    client.connect("sparkclient");

    // publish/subscribe
    if (client.isConnected()) {
        client.publish("/outTopic","hello world");
        client.subscribe("/inTopic");
    }
    
    timestamp = millis() + interval;
    pinMode(ldrPin,INPUT);
}

void loop() {
    if (millis() > timestamp) { // publish lightLevel every interval milliseconds
        itoa(analogRead(ldrPin),s,10); // convert int number as base 10 to char string
        client.publish("/plant/brightness", s);
        timestamp = millis() + interval;
    }
    if (client.isConnected())
        client.loop();
}
