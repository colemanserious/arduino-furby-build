
/*
  ControlFurby

  Poll information from Jenkins server, update Furby reaction accordingly
    
  Inspired by:
  http://arduino.cc/en/Tutroial/WebClient
 */
#include <SPI.h>
#include <Ethernet.h>
#include <aJSON.h>
#include <MemoryFree.h>
#include <Jenkins.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xED };
// we are avoiding using DNS for the moment
//IPAddress ip(192,168,0,166);  // set up a static IP for this device

// where are we running Jenkins from?
//IPAddress jenkinsServer(127,0,0,1);
//IPAddress jenkinsServer(192,168,2,183);
int jenkinsPort = 8080;
char jenkinsServer[] = "whowhatware.com";
//byte jenkinsServer[] = { 107, 170, 166, 174 };
//byte jenkinsServer[] = { 192, 168, 2, 183 };
const String jobName = "MakeFurbyDoItsThing";
jenkins_result_enum runResult = unknown;
jenkins_result_enum previousRun = unknown;
String runId;
String prevRunId;
int badResultCount = 0;

EthernetClient client;

const unsigned int ledPin = 11;   // pin 13 typically has an LED, but we can use others
const unsigned int tummyPin = 9;
const unsigned int unknownPin = 3;
const unsigned int runFurbyPin = 5;
unsigned int tummyResult;
unsigned int loopCounter = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  // pinMode(ledPin, OUTPUT);     
  // pinMode(tummyPin, INPUT);
  pinMode(runFurbyPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println(F("Beginning startup"));
  while (!Serial) {
    Serial.println(F("Hmmm - what's going on here?"));
      ; // wait for serial port to connect.  Needed for Leonardo only
  }
 
  // start the Ethernet connection
  Ethernet.begin(mac);

  Serial.println(Ethernet.localIP());
  
  // give the ethernet time to initialize
  delay(1000);
  
  //digitalWrite(ledPin, HIGH);    
}

// the loop routine runs over and over again forever:
void loop() {

  // tummyResult = digitalRead(tummyPin);
  // if (loopCounter < 255) {
  //   loopCounter++;
  // } else { loopCounter = 0; }
  // analogWrite(unknownPin, loopCounter);
  // digitalWrite(unknownBrotherPin, HIGH);
  // digitalWrite(ledPin, tummyResult);
  // Serial.println("Tummy value: ");
  // Serial.println(tummyResult);

  runResult = queryJenkins();
  Serial.print("Result from Jenkins: ");
  Serial.println(runResult);
  handleResult(runResult);
   
}

/**
 * Can only return one value, no struct.  So using build result.
 * Will document any other side-effect variables here...
 *   - runId: execution id (specific instance of execution)
 */
jenkins_result_enum queryJenkins() {

  if (!client.connect(jenkinsServer, jenkinsPort)) {
    Serial.println(F("exception: unable to connect"));
    return unknown;
  }

  // call out to get results
  client.println("GET /job/" + jobName + "/lastBuild/api/json?tree=result,id HTTP/1.1");
  client.println(F("Host: localhost"));
  client.println();
  
  while (!client.available()) {
    delay(1);
  }

  char json[60];
  char c;
  int letterCount = 0;
  boolean noJSONYet = true;
  while (client.available()) {
    c= client.read();
    if (c!= '{' && noJSONYet) {
      continue;
    } else if (noJSONYet) {
      noJSONYet = false;   // we must have trip
    }
    if (c != '#') {
      json[letterCount++] = c;
    }
  }
  Serial.print("Received json: ");
  Serial.println(json);

  aJsonObject* jsonObject = aJson.parse(json);
  Serial.println("Parsed jsonObject");
  aJsonObject*  jenkinsResult= aJson.getObjectItem(jsonObject, "result");
  String resultString = jenkinsResult->valuestring;
  Serial.print("Result:" );
  Serial.println(resultString);

  aJson.deleteItem(jsonObject); // clean up memory

  Serial.println(getFreeMemory());
  while (client.connected()) {
    delay(5);
  }
  
  client.stop();

  // convert Result to return value
  if (resultString.equals(SUCCESS)) return success;
  if (resultString.equals(FAILURE)) return failure;
  if (resultString.equals(ABORT)) return aborted;
  if (resultString.equals(UNSTABLE)) return unstable;

  return unknown;
}

void handleResult( jenkins_result_enum results) {
  switch (results) {
    case success:
        if ((previousRun == unstable) or (previousRun == failure)) {
          badResultCount = 0;  
          furbyCheer();        
        }
      break;
    case failure:
      badResultCount++;
      furbyRaspberry(badResultCount);
      break;
    case unstable:
      badResultCount++;
      furbyRaspberry(badResultCount);
      break;
    // unknown, aborted - both treated as no-ops
  }
  previousRun = results;
}

void furbyCheer() { 
  Serial.println("Hurray!");
  runFurby(5);
}

void runFurby (int seconds) {
  digitalWrite(runFurbyPin, HIGH);
  delay(seconds * 10000);
  digitalWrite(runFurbyPin, LOW);  
}

void furbyRaspberry(int failCount) { 
  Serial.print("Boo"); 
  Serial.println(failCount);

  runFurby(failCount * 10);
}