
/****Includes****/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the &onfiguration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>          //Allow custom URL
#include <Bounce2.h>
#include "html.h"

#define BEATS_DATA_MAX 15
#define GOOD_BEAT_ERROR_MARGIN 100
#define MS_IN_1_MIN 60000

#define CREATE_BEAT_DATA(suc)    BeatData bd; \
  bd.success = suc; \
  bd.time = currentTime; \
  beatsData[beatDataIndex++] = bd;\
  beatDataCount++; 
#define RESET_INTERVAL_BTW_NOTES()  MS_IN_1_MIN / bpm;
#define RESET_BEAT_DATA() beatDataIndex = 0;\
    beatDataCount = 0;

  void setupServer();
  void setupWifi();
  void metronome();
  void handleRoot();
  void setBpm();
  void getBpm();
  void beatCheck();
  unsigned char pulse_count(unsigned long *p_count);
  void pulse_init();
  void setTimeDefinition();
  void getTimeDefinition();
  void getGoodBeats();
  void getBadBeats();
  void getBeatsData();
  void resetBeatsData();

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
  long intervalBtwNotes = RESET_INTERVAL_BTW_NOTES()  // time beetwen each note in ms
  long lastBeatTime = 0;                    // Last time the metronome beated
  long unsigned int beatInterval = 0;       // Used to count the interval beetwen two beats
  int timeDefinition = 4;                   // Length of a measure

  //Related to the saving of beats data
  BeatData beatsData[BEATS_DATA_MAX];
  int beatDataIndex = 0;
  int beatDataCount = 0;

  //Used to debounce D0
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
    beatCheck();
    server.handleClient();
  }

  void handleRoot() {
    server.send(200, "text/html", html_index);
  }

  void setBpm() {
    String bpm_str;
    if ( server.hasArg("bpm") ) {

      bpm_str = server.arg(0);
      
      long old_bpm = bpm;
      bpm = bpm_str.toInt();

      if (bpm <= 0 || bpm > 250)  {
        bpm = old_bpm;
        server.send(400, "text/html", "error");
        return;
      }

        RESET_INTERVAL_BTW_NOTES()
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

    for (int i = 0; i < beatDataCount; i++) {

      toReturn += String(" { \"time\" : ") += String(beatsData[i].time) += String(",");
      toReturn += String(" \"success\" : ") += String(beatsData[i].success) += String(" }");

      //if this is not the last data
      if (i < beatDataCount - 1) {
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

    if (beatInterval >= intervalBtwNotes) {

      if (beatCounter % timeDefinition == 0) {
        tone(D2, 440, 50);
        digitalWrite(D4, HIGH);
      }
      else {
        tone(D2, 300, 50);
        digitalWrite(D5, HIGH);
      }

      delay (20);
      //Turning off both LED is cheaper than checking which one to turn off
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

    if (pulse_count(&beatInterval) == HIGH)
    {
      long currentTime = millis();

      if (beatDataIndex > BEATS_DATA_MAX) {
        RESET_BEAT_DATA()
      }

      if (beatInterval > intervalBtwNotes - GOOD_BEAT_ERROR_MARGIN
          && beatInterval < intervalBtwNotes + GOOD_BEAT_ERROR_MARGIN )
      {
        //if beetwen -margin and + margin from beat
        if (currentTime >= lastBeatTime + beatInterval - GOOD_BEAT_ERROR_MARGIN
            || currentTime <= lastBeatTime + GOOD_BEAT_ERROR_MARGIN)
        {
          goodBeats++;
          CREATE_BEAT_DATA(true)
        }
      }
      else {
        badBeats++;
        CREATE_BEAT_DATA(false)
      }
    }
  }

  void pulse_init() {
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






