//********************************* DEFINES *******************************

// OLED Display Pins (Keep I2C on 16/17)
#define I2C_SDA 8
#define I2C_SCL 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SD1306_ADDRESS 0x3C

#define DHTPIN 4                  // Digital pin where data will be received from DHT22
#define DHTTYPE DHT22             // DHT22 (AM2302) temperature and relative himidity sensor.

#define LED_PIN 48                // Built-in LED for DOIT ESP32S3 DEVK MODULE (used to notify when the ESP32 is set in AP mode to configure the WiFi connection)
#define DISPLAY_BTN 5            // Button to change the data shown in the Oled display
#define WIFI_BTN 6               // Button to set WiFi connection for ESP32
#define NUM_LEDS    1
#define BRIGHTNESS  32

#define SD_CS 10                   // Define CS pin for the SD card module


//****************************** INCLUDES ********************************
#include <WiFi.h>

// RGB LED library
#include <FastLED.h>

// Add MQTT library:
#include <PubSubClient.h>
#include <WiFiClient.h>

// Libraries for WebServer and WiFiManager
#include <WebServer.h>
#include <WiFiManager.h>

// Libraries for OLED display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Libraries for SD card
#include <SPI.h> 
#include "FS.h"
#include "SD.h"

// Library to deal with time values
#include "time.h"
#include <WiFiUdp.h>

// Libraries for DHT22 sensor and DS3231 RTC module
#include <DHT.h>
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC

// Include the Secrets.h file to hold the Authorization Credentials
#include "Secrets.h"

// Include vector for dynamic buffer
#include <vector> 

//************************** GLOBAL VARIABLES  *****************************
unsigned long previousMillisForOledRefresh = 0;
const long intervalBetweenOledRefresh = 1*60000; // Time interval for data refresh for OLED display and Serial Monitor

// Variables to store the readings from DHT22 sensor
float temperature;
float humidity;

bool status_LED = 0;   // Variable to store the LED status
uint8_t oledState = 0;  // Variable do store the Oled counter

uint8_t displayBtnCounter = 0;   // Variable to store how many times the display button was pressed
bool displayBtnStatus = 0; // Variable to store the button status which increments the Oled counter

bool wifiBtnStatus = 0;    // Variable to store the button status which starts the Wifi connection setup
bool AP_mode_status = 0;   // Variable to flag if the ESP32 is in the Access Point (AP) mode
bool shouldSaveConfig = false; // Variable to flag if it was saved the WiFi setup
unsigned int WIFI_CONNECTION_CHECK_TIMEOUT = 0;
bool wifiStatus = false;      // Variable to store the statu of the WiFi connection (connected = true and disconnected = false)

// Variable to store te reading time of DHT22 and other parameters
String reading_time;

String dataMessage; // Variable to store the string composed by the measured parameters

// Define the constants to store the configuration for getting NTP data
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -4*3600;
const int   daylightOffset_sec = 3600;

bool setDateTimeDS3231RTCUsingNTPClientFlag = 0;

// Global buffer to store sensor readings
std::vector<String> dataBuffer;

// Global buffer for MQTT messages
std::vector<String> mqttDataBuffer;

const size_t BUFFER_SIZE = 15; // Define the buffer size


//***************** OBJECTS INSTANTIANTIONS *************

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Declaration for the instace myRTC for DS3232RTC
DS3232RTC myRTC;

// Declaration for the instance DHT for DHT22
DHT dht(DHTPIN, DHTTYPE);

// Add MQTT client WiFi objects:
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Define the LED array
CRGB leds[NUM_LEDS];
//******************* SUPPORT FUNCTIONS *************************

void setup(){
  
  // Initialize FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);                                  
  
  // Set the LED color
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  
  //Configure pins 45 and 48 as signal inputs
  pinMode(DISPLAY_BTN, INPUT);
  pinMode(WIFI_BTN, INPUT);

  Serial.begin(115200);                                         // Set and initialize the serial communication

  startOLEDDisplay();                                           // Starts Oled display

  connectToWifi();                                              // Try to connect to WiFi

  myRTC.begin();                                                // Start the Real Time Clock module RTC DS3231

  startSDCard();                                                // Initialize the SD Card

  dht.begin();                                                  // Initialize DHT22 sensor

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);                 // Configure MQTT
}

void loop() {

  refreshKeyboardReadings();                                     // Continuously check if some button was pressed

  if(WiFi.status() == WL_CONNECTED){                             // If WiFi connected successfuly, then try to pick the timestamp from an NTP client 
                                                                 // and configure (sincronize) the RTC DS3231 module 
    
    if(!setDateTimeDS3231RTCUsingNTPClientFlag){                 // Test the flag !setDateTimeDS3231RTCUsingNTPClientFlag to certifie the RTC module 
                                                                 // has already been sincronized with NTP client timestamp
        getNTPClientDateTimeAndSetDS3231RTC();                   // If it doesn't, call the funtion to to so.
        setDateTimeDS3231RTCUsingNTPClientFlag = 1;              // Set the flag !setDateTimeDS3231RTCUsingNTPClientFlag to prevent enter in sync mode again.
        // Blink the RGB LED
        leds[0] = CRGB(0, 255, 255);
        FastLED.show();
    }
  }

  // Refresh the data presented in the OLED display in intervals of 60 seconds
  if (!displayBtnCounter && (millis() - previousMillisForOledRefresh >= intervalBetweenOledRefresh)) {
      
      previousMillisForOledRefresh = millis();
      
      readTemperatureAndHumidityFromDHT22();
      reading_time = getDS3231DateTime();
      
      logSDCard();

      if(WiFi.status() == WL_CONNECTED){


          if(!mqttClient.connected()){
              mqttReconnect();                                               // Continuously check if it is needed to reconnect MQTT
          }
          mqttClient.loop();                                                // Maintain MQTT connection
          bufferMqttData();
          wifiStatus = true;
        }
      else{
          Serial.println("Failed to send data to server: Wifi Disconnected");
          wifiStatus = false;
          // Turn on the Red LED
          leds[0] = CRGB(255, 0, 0);
          FastLED.show();
        }
      oledState++;
      if(oledState > 2) {
        oledState = 0;
        // Turn off the RGB LED
        leds[0] = CRGB(0, 0, 0);
        FastLED.show();
      }
  }
  else displayOledData();
  
}
