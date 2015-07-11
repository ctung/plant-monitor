#include "MQTT/MQTT.h"

// light dependenet resistor on analog pin 0
int ldrPin = A0;
// moisture sensore on analog pin 1
int moisturePin = A1;
// specify plant name
// path="/plant/<plant_name>"
const char* path="/plant/roger";

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

char sensorVal[5]="";
char topic[100]="";

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
    pinMode(moisturePin,INPUT);
}

void loop() {
    if (millis() > timestamp) { // publish every interval milliseconds
        // publish lightlevel
        itoa(analogRead(ldrPin),sensorVal,10); // convert int number as base 10 to char string
        strcpy(topic,path);
        strcat(topic,"/light"); // topic = "/plant/<plant_name>/light
        client.publish(topic, sensorVal);
        // publish moisture level
        itoa(analogRead(moisturePin),sensorVal,10); // convert int number as base 10 to char string
        strcpy(topic,path);
        strcat(topic,"/moisture");  // topic = "/plant/<plant_name>/moisture
        client.publish(topic, sensorVal);
        // set new interval timer
        timestamp = millis() + interval;
    }
    if (client.isConnected())
        client.loop();
}
