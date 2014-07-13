
/*
  ControlFurby

  Poll information from Jenkins server, update Furby reaction accordingly
    
  Inspired by:
  http://arduino.cc/en/Tutorial/WebClient
 */
#include <SPI.h>
#include <Ethernet.h>
#include <JsonParser.h>

using namespace ArduinoJson::Parser;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xED };

// we are avoiding using DNS for the moment
//IPAddress ip(192,168,0,166);  // set up a static IP for this device

// where are we running Jenkins from?
//IPAddress jenkinsServer(127,0,0,1);
//IPAddress jenkinsServer(192,168,2,183);
int jenkinsPort = 8080;
char jenkinsServer[] = "whowhatware.com";
//byte jenkinsServer[] = { 192, 168, 2, 183 };
String jobName = "MakeFurbyDoItsThing";

EthernetClient client;

int ledPin = 11;   // pin 13 typically has an LED, but we can use others
int shownMessage = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);     
  
  Serial.begin(9600);
  Serial.println("Beginning startup");
  while (!Serial) {
    Serial.println("Hmmm - what's going on here?");
      ; // wait for serial port to connect.  Needed for Leonardo only
  }
 
  // start the Ethernet connection
  Ethernet.begin(mac);

  Serial.println(Ethernet.localIP());
  
  // give the ethernet time to initialize
  delay(1000);
  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW

  if (!client.connect(jenkinsServer, jenkinsPort)) {
    Serial.println("exception: unable to connect");
    return;
  }

  Serial.println("Connected - getting data");
  client.println("GET /job/" + jobName + "/lastBuild/api/json HTTP/1.1");
  client.println("Host: localhost");
  client.println();
  
  while (!client.available()) {
    delay(1);
  }

  String json;
  json.reserve(300);     
  char c;
  boolean noJSONYet = true;
  while (client.available()) {
    c= client.read();
    if (c!= '{' && noJSONYet) {
      continue;
    } else if (noJSONYet) {
      noJSONYet = false;   // we must have trip
    }
    if (c == '\\') {
     json += "\\";  // insert  
    }
    json += c;
  }
  Serial.println(json);

  char jsonAsArray[json.length()];
  json.toCharArray(jsonAsArray, json.length());
  JsonParser<28> parser;
  JsonHashTable hashTable = parser.parseHashTable(jsonAsArray);
  if (!hashTable.success()) {
    Serial.println("Unable to parse successfully");
    Serial.println("Parsed string:");
    Serial.println(json);
    Serial.println("Index: ");
    Serial.println(json.length());
  } else {
    Serial.println("Result retrieved:");
    Serial.println(hashTable.getString("result"));
  }
  
  while (client.connected()) {
    delay(5);
  }
  
  client.stop();
  
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
