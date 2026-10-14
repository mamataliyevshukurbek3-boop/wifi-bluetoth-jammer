#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// use 12 bit precission for LEDC timer
#define LEDC_TIMER_12_BIT  12
// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// jamming output pin
#define OUT_PIN 4
// built-in LED pin
#define LED 2

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// SSID & Password
const char* ssid = "salom";  // Enter your SSID here
const char* password = "87654321";  //Enter your Password here

// turn on html page
const char * turn_on_html PROGMEM = "<!DOCTYPE html>"
"<html>"
"<head>"
"<title> 433Mhz Jam Control Panel </title>"
"<style>"
".button {"
"  border: none;"
"  color: white;"
"  padding: 15px 32px;"
"  text-align: center;"
"  display: inline-block;"
"  font-size: 58px;"
"  margin: 4px 2px;"
"  border-radius: 15px;"
"  font-family: sans-serif;"
"  cursor: pointer;"
"}"
""
".center {"
"  position:absolute;"
"  top:50%;"
"  left:50%;"
"  width :200px;"
"  height:200px;"
"  margin-left:-140px;"
"  margin-top:-100px;"
"}"
""
".button_green {background-color: green;}"
"</style>"
"</head>"

"<div class=\"center\">"
" <a href=\"turn_off\">"
"    <button class=\"button button_green\">"
"    Start jamming"
"    </button>"
"  </a>"
"</div>"
""
"</body>"
"</html>";


// turn off html page
const char * turn_off_html PROGMEM = "<!DOCTYPE html>"
"<html>"
"<head>"
"<title>  433Mhz Jam Control Panel </title>"
"<style>"
".button {"
"  border: none;"
"  color: white;"
"  padding: 15px 32px;"
"  text-align: center;"
"  display: inline-block;"
"  font-size: 58px;"
"  margin: 4px 2px;"
"  border-radius: 15px;"
"  font-family: sans-serif;"
"  cursor: pointer;"
"}"
""
".center {"
"  position:absolute;"
"  top:50%;"
"  left:50%;"
"  width :200px;"
"  height:200px;"
"  margin-left:-140px;"
"  margin-top:-100px;"
"}"
""
".button_red {background-color: red;}"
"</style>"
"</head>"

"<div class=\"center\">"
" <a href=\"turn_on\">"
"    <button class=\"button button_red\">"
"    Stop jamming"
"    </button>"
"  </a>"
"</div>"
""
"</body>"
"</html>";


// IP Address details
IPAddress local_ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

int jam_flag = 0;

// Funksiya prototipi (main loop oldin e'lon qilinishi kerak)
void toggle_jamming(int jam_flag);


void setup()
{
  pinMode(LED, OUTPUT);

  #ifdef ESP32
    // --- YANGI KOD ---
    // ledcAttachPin() va ledcSetup() funksiyalari o'rniga bitta ledcAttach() ishlatildi.
    // Chastota va rezolyutsiya pinning o'ziga biriktiriladi.
    ledcAttach(OUT_PIN, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
    // ------------------
  #else
    pinMode(OUT_PIN, OUTPUT);
  #endif
    
  // Serial port for debugging purposes
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); // AP rejimini aniq belgiladik
  Serial.println();
  Serial.println("[+] Configuring access point...");

  // --- TUZATILGAN QATOR ---
  if (!WiFi.softAP(ssid, password)) // Faqat SSID va parolni ishlatadi
  // -------------------------
  {
    Serial.print("[!] Soft AP creation failed.");
    while(1);
  }
  WiFi.softAPConfig(local_ip, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[*] SSID: ");
  Serial.println(ssid);
  Serial.print("[*] password: ");
  Serial.println(password);

  // Routes for web pages
  // root "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", turn_on_html);
  });

  // turn on page "/turn_on"
  server.on("/turn_on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    jam_flag = 0;
    toggle_jamming(jam_flag);
    request->send_P(200, "text/html", turn_on_html);
  });

  // turn off page "/turn_off"
  server.on("/turn_off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    jam_flag = 1;
    toggle_jamming(jam_flag);
    request->send_P(200, "text/html", turn_off_html);
  });

  // Start server
  server.begin();

  Serial.println("[+] Server started");

}

void toggle_jamming(int jam_flag)
{
  if(jam_flag == 1)
  {
    #ifdef ESP32
      // --- YANGI KOD ---
      // ledcWrite funksiyasiga kanal raqami emas, balki OUT_PIN raqami beriladi.
      ledcWrite(OUT_PIN, 2047);        // Using Duty Cycle results in better jamming
      // ------------------
    #elif defined(ESP8266)
      tone(OUT_PIN, 2047);
    #endif
      
    digitalWrite(LED, HIGH);
    Serial.println("[+] Started jamming");
  }
  else
  {
    tone(OUT_PIN, 0);   // send nothing to pin
    digitalWrite(LED, LOW);
    Serial.println("[+] Stopped jamming");
  }

}
void loop() 
{
}
