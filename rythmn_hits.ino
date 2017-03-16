
/****Includes****/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the &onfiguration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>          //Allow custom URL
#include <Bounce2.h>
#include "html.h"

#define BEATS_DATA_MAX 100

void setupServer();
void setupWifi();
void metronome();
void flash(int pin);
void handleRoot();
void setBpm();
void getBpm();
void beatCheck();
unsigned char pulse_count(unsigned long *p_count);
void pulse_init(void);
void setTimeDefinition();
void getTimeDefinition();
void getGoodBeats();
void getBadBeats();
void getBeatsData();
void flushBeatsData();

typedef struct S_BEAT_DATA {
  bool success;
  long time;
} BeatData;

ESP8266WebServer server(80);
const char *ssid = "metronome_server";

long badBeats = 0;                         // Counting the bad beats
long goodBeats = 0;                        // Counting the good beats
long bpm = 60;                            // Beats Per Minute
long beatCounter = 0;                     // Counting the beats for a measure (4/4 has 4 times)
const long msCountIn1Min = 60000;         // Numbers of ms in 1 sec
long waitBtwNotes = msCountIn1Min / bpm;  // time beetwen each note in ms
long lastBeatTime = 0;
long lastBeat = 0;
long unsigned int beatInterval = 0;
int timeDefinition = 4;

BeatData beatsData[BEATS_DATA_MAX];
int beatDataIndex = 0;
int beatDataCount = 0;

Bounce debouncer = Bounce();

// the setup routine runs once when you press reset:
void setup() {

  //setting the serial connection
  Serial.begin(9600);

  //Settings the pinout
  pinMode(D0, INPUT_PULLUP);   // Vibration sensor
  pinMode(D5, OUTPUT);  // B -> RGB LED
  pinMode(D4, OUTPUT);  // G -> RGB LED
  pinMode(D2, OUTPUT);  // Piezzo Speaker
  setupWifi();
  setupServer();
  debouncer.attach(D0);
  debouncer.interval(30);
  pulse_init();
}

void loop() {
  debouncer.update();
  metronome();
  server.handleClient();
  beatCheck();
}

void handleRoot() {
  server.send(200, "text/html", html_index);
}

void setBpm() {
  String bpm_str;
  if ( server.hasArg("bpm") ) {

    bpm_str = server.arg(0);
    //add function to init
    long old_bpm = bpm;
    bpm = bpm_str.toInt();

    if (bpm <= 0 || bpm > 250)  {
      bpm = old_bpm;
      server.send(400, "text/html", "error");
      return;
    }

    waitBtwNotes = msCountIn1Min / bpm;
  }

  server.send(200, "text/html", "OK");
}

void getBpm() {
  server.send(200, "text/html", String(bpm));
}

void setTimeDefinition() {
  
  String timeDefStr;
  if ( server.hasArg("timedefinition") ) {

    timeDefStr = server.arg(0);
    //add function to init
    long oldTimeDef = timeDefinition;
    timeDefinition = timeDefStr.toInt();

    if (timeDefinition <= 0 || timeDefinition > 32)  {
      timeDefinition = oldTimeDef;
      server.send(400, "text/html", "error");
      return;
    }
  }

  server.send(200, "text/html", "OK");
}

void getTimeDefinition() {
    server.send(200, "text/html", String(timeDefinition));
}

void getGoodBeats() {
    server.send(200, "text/html", String(goodBeats));
}

void getBadBeats() {
    server.send(200, "text/html", String(badBeats));
}

void getBeatsData() {
  String toReturn =  "[";
  
   for(int i = 0; i < beatDataCount; i++) {

    toReturn += String(" { \"time\" : ") += String(beatsData[i].time) += String(",");
    toReturn += String(" \"success\" : ") += String(beatsData[i].success) += String(" }");
    
    if(i < beatDataCount -1){
      toReturn += String(",");
    }
  }

  toReturn += String("]");
  Serial.println(toReturn);
  server.send(200, "text/json", toReturn);
}

void setupWifi() {

  Serial.println("Starting wifi");
  WiFiManager wifiManager;
  wifiManager.setAPCallback([](WiFiManager * manager) {});

  wifiManager.autoConnect(ssid);
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println("Wifi is setup");
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/setbpm", setBpm);
  server.on("/getbpm", getBpm);
  server.on("/settimedefinition", setTimeDefinition);
  server.on("/gettimedefinition", getTimeDefinition);
  server.on("/getbadbeats", getBadBeats);
  server.on("/getgoodbeats", getGoodBeats);
  server.on("/getbeatsdata", getBeatsData);
  server.begin();
  Serial.println("HTTP server started");
}

void metronome() {

  long beatInterval = millis() - lastBeatTime;

  if (beatInterval >= waitBtwNotes) {

    if (beatCounter % timeDefinition == 0) {
      tone(D2, 440, 50);
      digitalWrite(D4, HIGH);
    }
    else {
      tone(D2, 300, 50);
      digitalWrite(D5, HIGH);
    }

    delay (20);
    digitalWrite(D5, LOW);
    digitalWrite(D4, LOW);

    beatCounter++;

    if (beatCounter > timeDefinition - 1) {
      beatCounter = 0;
    }

     lastBeatTime = millis();
  }

}

void beatCheck() {
  long sensor = debouncer.read();
  long errorMargin = 100;

  if (pulse_count(&beatInterval) == 1)
  {
    long currentTime = millis();

    if(beatDataIndex > BEATS_DATA_MAX) {
      flushBeatsData();
    }
    
    if (beatInterval > waitBtwNotes - errorMargin && beatInterval < waitBtwNotes + errorMargin ) {
      //if beetwen -margin and + margin from beat
      if (currentTime >= lastBeatTime + beatInterval - errorMargin || currentTime <= lastBeatTime + errorMargin) {
        goodBeats++;

        //create a function
        BeatData bd;
        bd.success = true;
        bd.time = currentTime; 
        beatsData[beatDataIndex++] = bd;
        beatDataCount++;
        
      }
    }
    else {
      badBeats++;

        BeatData bd;
        bd.success = false;
        bd.time = currentTime; 
        beatsData[beatDataIndex++] = bd;
        beatDataCount++;
    }
  }
}

void pulse_init(void) {
  pinMode(D0, INPUT);
}

unsigned char pulse_count(unsigned long *p_count) {
  static unsigned char pin_prev = 0; //previous state of the pin
  static unsigned long start_time = millis();
  unsigned long time;
  unsigned char tmp = debouncer.read();

  if (tmp ^ pin_prev) {  //pin state has changed
    time = millis();       //record current time
    pin_prev = tmp;    //save the pin state
    if (start_time) {    //already in a counting session now
      *p_count = time - start_time;  //calculate time elapsed
      start_time = 0;    //indicating end of a counting session
      return 1;          //we have new data
    } else {             //start of a counting session
      start_time = time; //record the start time
      return 0;          //no count yet
    }
  } else {               //pin state has not changed
    return 0;            //no new count
  }
}

void flushBeatsData(){
  
  for(int i = 0; i < BEATS_DATA_MAX; i++) {
    //flush it ...
  }

  beatDataIndex = 0;
  beatDataCount = 0;
}





