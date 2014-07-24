
/*
  ControlFurby

  Poll information from Jenkins server, update Furby reaction accordingly
  See README.MD
*/
#include <Ethernet.h>
#include <aJSON.h>
#include <Jenkins.h>

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xED };
// we are avoiding using DNS for the moment
//byte ip[] = {192, 168, 0, 166};
//IPAddress ip(2, 68, 135, 111); //2.68.135.111
IPAddress ip(10,9,64,122);  
IPAddress myDns(75,75,75,75);
IPAddress gateway(10,9,64,1);

// where are we running Jenkins from?
//IPAddress jenkinsServer(107,170,166,174);
int jenkinsPort = 8080;
char jenkinsServer[] = "whowhatware.com";
//byte jenkinsServer[] = { 107, 170, 166, 174 };
//byte jenkinsServer[] = { 192, 168, 2, 183 };
const String jobName = "MakeFurbyDoItsThing";
jenkins_result_enum runResult = unknown;
jenkins_result_enum previousRun = unknown;
String runResultString;
String runId;
String prevRunId;
int badResultCount = 0;

EthernetClient client;
//EthernetServer server = EthernetServer(80);

/** Pins **/
const unsigned int runFurbyPin = 5;
const unsigned int speakerPin = 9;  //11 on Mega, 9 on Uno, Nano, etc

bool furbyRunning = false;
unsigned long furbyEndTime; 
unsigned long nextJenkinsCheckTime = 0;
unsigned long furbyRetryVal = 0;

//#define RUNSOUND
/** Object for playing WAV files from SD card **/
#ifdef RUNSOUND 
TMRpcm tmrpcm;   
#endif
const uint8_t SdChipSelect = 4;

#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

// the setup routine runs once when you press reset:
void setup() {                
  /** get ready to control Furby power **/
  pinMode(runFurbyPin, OUTPUT);

  #ifdef RUNSOUND
  tmrpcm.speakerPin = speakerPin; 
  #endif 

  Serial.begin(9600);
  DEBUG_PRINT(F("Beginning startup"));
 
  /** start the Ethernet connection **/
  //Ethernet.begin(mac, ip, myDns, gateway);
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());

  /** uncomment code if using Jenkins notifications / called by Jenkins **/
  //server.begin();   // start listening for notifications
  #ifdef RUNSOUND  
  if (!SD.begin(SdChipSelect )) {  // see if the card is present and can be initialized:
       Serial.println("SD fail");  
       return;   // don't do anything more if not
  }
  #endif
  // give the ethernet time to initialize
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {

  checkFurbyState();
  runResult = queryJenkins();
  //runResult = calledByJenkins();

  if (runResult != unknown) {
    handleResult(runResult);
  }   
}

/**
 * Can only return one value, no struct.  So using build result.
 * Will document any other side-effect variables here...
 *   - runId: execution id (specific instance of execution)
 */
jenkins_result_enum queryJenkins() {

  if (nextJenkinsCheckTime < millis()) {
    if (!client.connect(jenkinsServer, jenkinsPort)) {
      Serial.print(F("exception: unable to connect"));
      //Serial.println(jenkinsServer);
      //Serial.println(jenkinsPort);
      // Serial.println("exception:: unable to connect");
      return unknown;
    }

    // call out to get results
    client.println("GET /job/MakeFurbyDoItsThing/lastBuild/api/json?tree=result,id HTTP/1.1");
    client.println(F("Host: whowhatware.com"));
    client.println(F("Connection: close"));
    client.println();
    
    char json[47];  
    readResult(client, json);
    DEBUG_PRINT("JSON result");
    DEBUG_PRINT(json);
    
    // runResultString = parseJson(json, "result");
    // runId = parseJson(json, "id");

    aJsonObject* jsonObject = aJson.parse(json);
    aJsonObject*  jenkinsResult= aJson.getObjectItem(jsonObject, "result");
    runResultString = jenkinsResult->valuestring;
    jenkinsResult = aJson.getObjectItem(jsonObject, "id");
    runId = jenkinsResult->valuestring;

    DEBUG_PRINT(runResultString);
    DEBUG_PRINT(runId);
    while (client.connected()) {
      delay(5);
    }
    
    client.stop();

    nextJenkinsCheckTime = millis() + (1000 * 10);  // wait n seconds before even trying again
    if (runId == prevRunId) {
      runResult = unknown;   
    } else {
      prevRunId = runId;
      runResult = convertResultValue(runResultString); 
    }
    DEBUG_PRINT("Result from Jenkins: ");
    DEBUG_PRINT(runResult);
    return runResult;
  } else {
    return unknown;   // too early to go out and check again
  }
}

// jenkins_result_enum calledByJenkins() {
//   // if an incoming client connects, there will be bytes available to read:
//   client = server.available();
//   if (client == true) {
//     char json[200];
//     readResult(client, json);
//     String resultString = parseJson(json, "status");
//     return convertResultValue(resultString); 
//   }
//   return unknown;
// }

String parseJson(char json[], char fieldToCheck[]) {
  DEBUG_PRINT("Checking");
  DEBUG_PRINT(fieldToCheck);
  aJsonObject* jsonObject = aJson.parse(json);
  aJsonObject*  jenkinsResult= aJson.getObjectItem(jsonObject, fieldToCheck);
  String resultString = jenkinsResult->valuestring;

  aJson.deleteItem(jsonObject); // clean up memory
  return resultString;
}

jenkins_result_enum convertResultValue(String resultString) {
  if (resultString.equals(SUCCESS)) return success;
  if (resultString.equals(FAILURE)) return failure;
  if (resultString.equals(ABORT)) return aborted;
  if (resultString.equals(UNSTABLE)) return unstable;

  return unknown;  
}

void readResult (EthernetClient client, char json[]) {
  while (!client.available()) {
      delay(1);
  }
  char c;
  int letterCount = 0;
  boolean noJSONYet = true;

  while (client.available()) {
    c= client.read();
    //DEBUG_PRINT(c);
    if (c!= '{' && noJSONYet) {
      continue;
    } else if (noJSONYet) {
      noJSONYet = false;   // we must have tripped - start collecting
    }
    // job names can contain # strings, which caused issues with aJSON parsing
    if (c != '#') { 
      json[letterCount++] = c;
    }
    // quit grabbing now... - found end
    if (c == '}') {
      break;
    }
  }
  if (sizeof(json) > 47) {
      DEBUG_PRINT("Too much!");

  }
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
      furbyYell(++badResultCount);
      break;
    case unstable:
      furbyYell(++badResultCount);
      break;
    // unknown, aborted - both treated as no-ops
  }
  previousRun = results;
}

/**
  Give positive reinforcement sound, and then turn off 
**/
void furbyCheer() { 
  runFurby(5, true);
}

/**
  Perform annoying behavior for a time which grows by the number
  of failing builds
**/
void furbyYell(int failCount) { 
  runFurby(failCount * 10, false);
}

/**
 Start running, if not already, and set time interval out for turning it off
 **/
void runFurby (int seconds, bool isGoodReaction) {
  if (furbyRunning) return;
  furbyRunning = true;
  furbyEndTime = millis() + (1000*seconds);
  furbyRetryVal = millis() + (1000 * 60 * 20);    // 20 minutes
  digitalWrite(runFurbyPin, HIGH);

  DEBUG_PRINT("Furby reaction: " + isGoodReaction);
  #ifdef RUNSOUND
  if (isGoodReaction) {
    tmrpcm.play("furbywheehee855.wav");
  } else {
    DEBUG_PRINT(seconds);
    tmrpcm.play("furbyvomit884.wav");
  }
  #endif
}

/**
  Is it time to turn the Furby off?
  Is it time to restart the Furby?
  **/
void checkFurbyState() {
  // Serial.println("Checking Furby state");
  if (furbyRunning && millis() > furbyEndTime) {
    DEBUG_PRINT("Turning Furby off");
    furbyEndTime = 0;
    digitalWrite(runFurbyPin, LOW);
    furbyRunning=false;
  }
  if ((badResultCount > 0) && (furbyRunning = false)) {
    if (millis() > furbyRetryVal) {
      runFurby(badResultCount, false);
    }
  }
}
