
/*
  ControlFurby

  Poll information from Jenkins server, update Furby reaction accordingly
  See README.MD
*/
#include <SPI.h>
#include <Ethernet.h>
#include <aJSON.h>
#include <MemoryFree.h>
#include <Jenkins.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xED };
// we are avoiding using DNS for the moment
//byte ip[] = {192, 168, 0, 166};

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
EthernetServer server = EthernetServer(80);

const unsigned int runFurbyPin = 5;

bool furbyRunning = false;
unsigned long furbyEndTime; 

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
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

  server.begin();   // start listening for notifications

  // give the ethernet time to initialize
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {

  checkFurbyState();
  //runResult = queryJenkins();

  runResult = calledByJenkins();

  if (runResult != unknown) {
    Serial.print("Result from Jenkins: ");
    Serial.println(runResult);
    handleResult(runResult);
  }   
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

  char json[200];
  readResult(client, json);
  
  String resultString = parseJson(json, "result");
  
  Serial.println(getFreeMemory());
  while (client.connected()) {
    delay(5);
  }
  
  client.stop();

  return convertResultValue(resultString); 
}

jenkins_result_enum calledByJenkins() {
    // if an incoming client connects, there will be bytes available to read:
  EthernetClient client = server.available();
  if (client == true) {
    char json[200];
    readResult(client, json);
    String resultString = parseJson(json, "status");
    return convertResultValue(resultString); 
  }
  return unknown;
}

String parseJson(char json[], char fieldToCheck[]) {

  aJsonObject* jsonObject = aJson.parse(json);
  Serial.println("Parsed jsonObject");
  aJsonObject*  jenkinsResult= aJson.getObjectItem(jsonObject, fieldToCheck);
  String resultString = jenkinsResult->valuestring;
  Serial.print("Result:" );
  Serial.println(resultString);

  aJson.deleteItem(jsonObject); // clean up memory
  return resultString;
}

jenkins_result_enum convertResultValue(String resultString) {
  // convert Result to return value
  if (resultString.equals(SUCCESS)) return success;
  if (resultString.equals(FAILURE)) return failure;
  if (resultString.equals(ABORT)) return aborted;
  if (resultString.equals(UNSTABLE)) return unstable;

  return unknown;  
}

void readResult (EthernetClient client, char json[]) {
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

void furbyRaspberry(int failCount) { 
  Serial.print("Boo"); 
  Serial.println(failCount);

  runFurby(failCount * 10);
}

// Start running, if not already, and set time interval out for turning it off
void runFurby (int seconds) {
  if (furbyRunning) return;
  furbyRunning = true;
  furbyEndTime = millis() + (1000*seconds);
  digitalWrite(runFurbyPin, HIGH);
}

// is it time to turn it off?
void checkFurbyState() {
  if (millis() > furbyEndTime) {
    furbyEndTime = 0;
    digitalWrite(runFurbyPin, LOW);
    furbyRunning=false;
  }
}
