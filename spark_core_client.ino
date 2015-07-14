
#include "elapsedMillis/elapsedMillis.h"
#include "spark-dallas-temperature/spark-dallas-temperature.h"
#include "MQTT/MQTT.h"

#define ldrPin A0        // light dependenet resistor on analog pin 0
#define moisturePin A1   // moisture sensore on analog pin 1
#define tempPin D3       // Temperature Sensor
#define waterPin D4      // output pin to drive water pump relay
const char* plantName="moneyTree";  // plantName and clientName

char sensorVal[5]="";   // variable to hold sensor readings
char topic[100]="";     // buffer to hold MQTT topic string

// timer to track elapsed time between waterings
elapsedMillis waterTimer;
unsigned int waterInterval = 1000*60*60*24; // water no more than once a day

// timer to track elapsed time between sensor readings
elapsedMillis timeElapsed;
unsigned int sensorInterval = 1000*60*10; // read sensors every 10 minutes

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

/* DS18S20 Temperature chip i/o */
DallasTemperature sensors(new OneWire(tempPin));

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    // convert payload to c_string
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    
    // convert c_string to integer
    char* pEnd;
    long int water = strtol(p,&pEnd,10);
    if (water > 0 && waterTimer > waterInterval) {
        // flash the LED blue 
        RGB.control(true);
        RGB.color(0, 0, 255);
        
        getTopic(topic,plantName,"pump");
        client.publish(topic,"100");
        
        // run the pump for the specified number of seconds
        digitalWrite(waterPin,HIGH);
        delay(water*1000);
        digitalWrite(waterPin,LOW);
        
        client.publish(topic,"0");
        
        // release control of LED
        RGB.control(false);
        
        // reset the elapsed time since last watering
        waterTimer = 0;
    }
}


// allow us to use itoa() in this scope
extern char* itoa(int a, char* buffer, unsigned char radix);

void setup() {
    // connect to the server using client name plant_<plantName>
    strcpy(topic,"plant_");
    strcat(topic,plantName);
    client.connect(topic);

    // publish/subscribe
    if (client.isConnected()) {
        getTopic(topic,plantName,"status");
        client.publish(topic,"connected");
        getTopic(topic,plantName,"addWater");
        client.subscribe(topic);
    }
    
    pinMode(ldrPin,INPUT);
    pinMode(moisturePin,INPUT);
    pinMode(waterPin,OUTPUT);
    digitalWrite(waterPin,LOW);
    sensors.begin();
}

void loop() {
    if (timeElapsed > sensorInterval) { // publish every interval milliseconds
        // flash the LED green
        RGB.control(true);
        RGB.color(0,255,0);
        
        // publish lightlevel
        itoa(analogRead(ldrPin),sensorVal,10); // convert int number as base 10 to char string
        getTopic(topic,plantName,"light");
        client.publish(topic, sensorVal);
        
        // publish moisture level
        itoa(analogRead(moisturePin),sensorVal,10); // convert int number as base 10 to char string
        getTopic(topic,plantName,"moisture");
        client.publish(topic, sensorVal);
        
        // publish the temperature
        sensors.requestTemperatures();
        float farenheit = sensors.getTempFByIndex(0);
        snprintf(sensorVal, 5, "%f", farenheit);
        getTopic(topic,plantName,"temp");
        client.publish(topic, sensorVal);
        
        // release control of LED
        delay(1000);
        RGB.control(false);
        
        // set new interval timer
        timeElapsed = 0;
    }
    if (client.isConnected())
        client.loop();
}

// create a topic string in the form of /plant/<plantName>/<attribute>
char* getTopic(char* buffer, const char* plantName, const char* attribute)
{
    strcpy(buffer,"/plant/");
    strcat(buffer,plantName);
    strcat(buffer,"/");
    strcat(buffer,attribute);
    return buffer;
}
