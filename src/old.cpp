#include <SmartThingsESP8266WiFi.h>

/* 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
*/

#define PIN_LED 0 // build in LED on Huzzah
#define PIN_CONTACT 16

// Forward declaration
void messageCallout(String message);

String str_ssid = "Provost-Mesh";
String str_password = "cra6ga12o4doph09";
IPAddress ip(192, 168, 135, 202);
IPAddress gateway(192, 168, 135, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dnsserver(192, 168, 135, 1);
const unsigned int serverPort = 8090;

// Hub Information
IPAddress hubIp(192, 168, 135, 2);
const unsigned int hubPort = 39501;

//Create a SmartThings Ethernet ESP8266WiFi object
st::SmartThingsESP8266WiFi smartthing(str_ssid, str_password, ip, gateway, subnet, dnsserver, serverPort, hubIp, hubPort, messageCallout);

bool isDebugEnabled; // enable or disable debug in this example

int contactState = false;

void updateContactState(int currentContactState)
{
    contactState = currentContactState;
    String state = (contactState ? String("{ \"contact\":\"closed\" }") : String("{ \"contact\":\"open\" }"));
    Serial.printf("State changed: %s\r\n", state.c_str());

    if (contactState) // true means closed
        digitalWrite(PIN_LED, LOW);
    else
        digitalWrite(PIN_LED, HIGH);

    smartthing.send(state);
}

void on()
{
    digitalWrite(PIN_LED, LOW);   // turn LED on
    smartthing.send("{ \"switch\": \"on\" }"); // send message to cloud
}

void off()
{
    digitalWrite(PIN_LED, HIGH);   // turn LED off
    smartthing.send("{ \"switch\":\"off\" }"); // send message to cloud
}

void setup()
{
    Serial.begin(115200);
    Serial.println("");
    Serial.println("setup..");

    // setup hardware pins
    pinMode(16, INPUT_PULLDOWN_16); // pulldown only available on Pin 16
    pinMode(PIN_LED, OUTPUT);       // define PIN_LED as an output
    digitalWrite(PIN_LED, HIGH);    // set value to HIGH (off)

    //Run the SmartThings init() routine to make sure the ThingShield is connected to the ST Hub
    smartthing.init();

    //synch up the ST cloud
    smartthing.send("switch:off"); // send message to cloud

    int currentContactState = digitalRead(PIN_CONTACT);
    updateContactState(currentContactState);
}

unsigned long lastMillis;
void loop()
{
    if (millis() > lastMillis + 10000)
    {
        Serial.printf("Heap %ld, millis %ld\r\n", ESP.getFreeHeap(), millis());
        lastMillis = millis();
    }

    int currentContactState = digitalRead(PIN_CONTACT);
    if (currentContactState != contactState)
    {
        updateContactState(currentContactState);
    }

    // run smartthing logic
    smartthing.run();
}

void messageCallout(String message)
{
    // if debug is enabled print out the received message
    if (isDebugEnabled)
    {
        Serial.print("Received message: '");
        Serial.print(message);
        Serial.println("' ");
    }

    // if message contents equals to 'on' then call on() function
    // else if message contents equals to 'off' then call off() function
    if (message.equals("switch/on"))
    {
        on();
    }
    else if (message.equals("switch/off"))
    {
        off();
    }
}
