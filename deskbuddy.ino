// watch complete making video from here , please subscribe and support us
// you can buy components and this kit from www.esclabs.in 

// ==================================================
// EDISON SCIENCE CORNER  - ESCLABS
// ==================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "time.h"
#include <math.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define WIFI_TIMEOUT_MS   15000
#define AP_SSID           "Deskbuddy-Setup"
#define AP_PASS           ""
#define CONFIG_NAMESPACE  "deskbuddy"
#define BMP280_SDA        2
#define BMP280_SCL        5

// ==================================================
// 1. ASSETS & CONFIG
// ==================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PIN_SCL  10  // Clock
#define PIN_SI   9   // MOSI
#define PIN_CS   8   // Chip Select
#define PIN_DC   7   // Data/Command
#define PIN_RST  6   // Reset
#define TOUCH_PIN 3

// --- DISPLAY CONTRAST SETTINGS ---
#define CONTRAST_HIGH   180  // 
#define CONTRAST_MEDIUM 180  // 
#define CONTRAST_LOW    180 // 
#define CONTRAST_MIN    180  // 
#define DEFAULT_CONTRAST CONTRAST_HIGH  // Default contrast

// Дисплей GMG12864-06D (SPI) - ST7565
U8G2_ST7565_JLX12864_F_4W_SW_SPI u8g2(
  U8G2_R2,  // Поворот на 90 градусов
  PIN_SCL,  // SCL
  PIN_SI,   // SI (MOSI)
  PIN_CS,   // CS
  PIN_DC,   // DC
  PIN_RST   // RST
);

// --- WEATHER ICONS ---
const unsigned char bmp_clear[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xc0, 0x80, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x01, 0x0f, 0xf0, 0x80, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_clouds[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0xe0, 0x1c, 0x00, 0x00, 0xc0, 0x18, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x00, 0x60, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_sun[] PROGMEM = { 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x10, 0x08, 0x04, 0x20, 0x03, 0xc0, 0x27, 0xe4, 0x07, 0xe0, 0x07, 0xe0, 0x27, 0xe4, 0x03, 0xc0, 0x04, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00 };
const unsigned char mini_cloud[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11 };
const unsigned char bmp_tiny_drop[] PROGMEM = { 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x7c, 0x38, 0x00 };

// --- EMOTION PARTICLES (16x16) ---
const unsigned char bmp_heart[] PROGMEM = { 0x00, 0x00, 0x0c, 0x60, 0x1e, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_zzz[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_anger[] PROGMEM = { 0x00, 0x00, 0x11, 0x10, 0x2a, 0x90, 0x44, 0x40, 0x80, 0x20, 0x80, 0x20, 0x44, 0x40, 0x2a, 0x90, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// --- GLOBALS ---
int currentPage = 0;
int currentContrast = DEFAULT_CONTRAST;  // Now level contrast
int contrastLevel = 0;  // 0=HIGH, 1=MEDIUM, 2=LOW, 3=MIN
int tapCounter = 0;
unsigned long lastTapTime = 0;
bool lastPinState = false;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;
const unsigned long LONG_PRESS_TIME = 800;
const unsigned long DOUBLE_TAP_DELAY = 300;
unsigned long lastPageSwitch = 0;
const unsigned long PAGE_INTERVAL = 8000;

// MOODS
#define MOOD_NORMAL 0
#define MOOD_HAPPY 1
#define MOOD_SURPRISED 2
#define MOOD_SLEEPY 3
#define MOOD_ANGRY 4
#define MOOD_SAD 5
#define MOOD_EXCITED 6
#define MOOD_LOVE 7
#define MOOD_SUSPICIOUS 8
int currentMood = MOOD_NORMAL;

String city;
String countryCode;
String apiKey;
unsigned long lastWeatherUpdate = 0;
float temperature = 0.0;
float feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Loading";
String weatherDesc = "Wait...";

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
};
ForecastDay fcast[3];

String wifiSsid;
String wifiPassword;
String tzString;
const char* ntpServer = "pool.ntp.org";

// World clock: 2 cities, name + UTC offset (hours, e.g. 5.5 for India +5:30)
String wc1Name, wc2Name;
float wc1Offset = 5.5f, wc2Offset = 11.0f;

Preferences prefs;
WebServer server(80);
DNSServer dnsServer;
bool configMode = false;  // true = AP mode, showing config portal

Adafruit_BMP280 bmp;
bool bmp280Ok = false;
float bmpTemp = 0.0f;
float bmpPressure = 0.0f;

// ==================================================
// 2. ULTRA PRO PHYSICS ENGINE
// ==================================================

struct Eye {
  float x, y;  // Current position (Top-Left)
  float w, h;  // Current size
  float targetX, targetY, targetW, targetH;

  // Pupil Physics (Secondary Motion)
  float pupilX, pupilY;
  float targetPupilX, targetPupilY;

  // Physics constants
  float velX, velY, velW, velH;
  float pVelX, pVelY;
  float k = 0.12;   // Eye Spring
  float d = 0.60;   // Eye Damping (Heavier feel)
  float pk = 0.08;  // Pupil Spring (Softer/Laggier)
  float pd = 0.50;  // Pupil Damping

  bool blinking;
  unsigned long lastBlink;
  unsigned long nextBlinkTime;

  void init(float _x, float _y, float _w, float _h) {
    x = targetX = _x;
    y = targetY = _y;
    w = targetW = _w;
    h = targetH = _h;
    pupilX = targetPupilX = 0;
    pupilY = targetPupilY = 0;
    nextBlinkTime = millis() + random(1000, 4000);
  }

  void update() {
    // 1. Main Eye Physics
    float ax = (targetX - x) * k;
    float ay = (targetY - y) * k;
    float aw = (targetW - w) * k;
    float ah = (targetH - h) * k;

    velX = (velX + ax) * d;
    velY = (velY + ay) * d;
    velW = (velW + aw) * d;
    velH = (velH + ah) * d;

    x += velX;
    y += velY;
    w += velW;
    h += velH;

    // 2. Pupil Physics (Dragging behind)
    float pax = (targetPupilX - pupilX) * pk;
    float pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd;
    pVelY = (pVelY + pay) * pd;
    pupilX += pVelX;
    pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;
float breathVal = 0;

// ==================================================
// 3. LOGIC & NETWORK
// ==================================================
const unsigned char* getBigIcon(String w) {
  if (w == "Clear") return bmp_clear;
  if (w == "Clouds") return bmp_clouds;
  if (w == "Rain" || w == "Drizzle") return bmp_rain;
  return bmp_clouds;
}
const unsigned char* getMiniIcon(String w) {
  if (w == "Clear") return mini_sun;
  if (w == "Rain" || w == "Drizzle" || w == "Thunderstorm") return mini_rain;
  return mini_cloud;
}

void updateMoodBasedOnWeather() {
  int m = MOOD_NORMAL;
  if (weatherMain == "Clear") m = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") m = MOOD_SAD;
  else if (weatherMain == "Thunderstorm") m = MOOD_SURPRISED;
  else if (weatherMain == "Clouds") m = MOOD_NORMAL;
  else if (temperature > 25) m = MOOD_EXCITED;
  else if (temperature < 5) m = MOOD_SLEEPY;
  currentMood = m;
}

void handleTouch() {
  bool currentPinState = digitalRead(TOUCH_PIN);
  unsigned long now = millis();
  if (currentPinState && !lastPinState) {
    pressStartTime = now;
    isLongPressHandled = false;
  } else if (currentPinState && lastPinState) {
    if ((now - pressStartTime > LONG_PRESS_TIME) && !isLongPressHandled) {
      lastPageSwitch = now;
      if (currentPage == 0) {
        currentMood++;
        if (currentMood > MOOD_SUSPICIOUS) currentMood = 0;
        lastSaccade = 0;  // Trigger move
      } else if (currentPage == 1) currentPage = 3;
      else if (currentPage == 2) currentPage = 4;
      isLongPressHandled = true;
    }
  } else if (!currentPinState && lastPinState) {
    if ((now - pressStartTime < LONG_PRESS_TIME) && !isLongPressHandled) {
      tapCounter++;
      lastTapTime = now;
    }
  }
  lastPinState = currentPinState;
  if (tapCounter > 0) {
    if (now - lastTapTime > DOUBLE_TAP_DELAY) {
      lastPageSwitch = now;
      if (tapCounter == 2) {
        // Переключение между уровнями контраста
        contrastLevel++;
        if (contrastLevel > 3) contrastLevel = 0;
        
        switch (contrastLevel) {
          case 0: currentContrast = CONTRAST_HIGH; break;
          case 1: currentContrast = CONTRAST_MEDIUM; break;
          case 2: currentContrast = CONTRAST_LOW; break;
          case 3: currentContrast = CONTRAST_MIN; break;
        }
        
        u8g2.setContrast(currentContrast);
        u8g2.sendBuffer();
      } else if (tapCounter == 1) {
        if (currentPage == 3) currentPage = 1;
        else if (currentPage == 4) currentPage = 2;
        else {
          currentPage++;
          if (currentPage > 2) currentPage = 0;
        }
      }
      tapCounter = 0;
    }
  }
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  // URL-encode city: space -> %20 (so "Saint Petersburg" works in query)
  String cityEnc = city;
  cityEnc.replace(" ", "%20");
  String url = "https://api.openweathermap.org/data/2.5/weather?q=" + cityEnc + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      temperature = doc["main"]["temp"].as<float>();
      feelsLike = doc["main"]["feels_like"].as<float>();
      humidity = doc["main"]["humidity"].as<int>();
      weatherMain = doc["weather"][0]["main"].as<String>();
      weatherDesc = doc["weather"][0]["description"].as<String>();
      if (weatherDesc.length() > 0) {
        weatherDesc[0] = toupper(weatherDesc[0]);
      }
      updateMoodBasedOnWeather();
    }
  }
  http.end();
  url = "http://api.openweathermap.org/data/2.5/forecast?q=" + cityEnc + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      struct tm t;
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      int indices[3] = { 7, 15, 23 };
      for (int i = 0; i < 3; i++) {
        int idx = indices[i];
        fcast[i].temp = doc["list"][idx]["main"]["temp"].as<int>();
        fcast[i].iconType = doc["list"][idx]["weather"][0]["main"].as<String>();
        int nextDayIndex = (today + i + 1) % 7;
        fcast[i].dayName = days[nextDayIndex];
      }
    }
  }
  http.end();
}

// ==================================================
// 3b. WEB CONFIG (Preferences + AP/STA server)
// ==================================================

void loadConfig() {
  prefs.begin(CONFIG_NAMESPACE, true);
  wifiSsid = prefs.getString("ssid", "");
  wifiPassword = prefs.getString("pass", "");
  apiKey = prefs.getString("apikey", "");
  city = prefs.getString("city", "Saint Petersburg");
  countryCode = prefs.getString("country", "RU");
  tzString = prefs.getString("tz", "MSK-3");
  wc1Name = prefs.getString("wc1n", "India");
  wc2Name = prefs.getString("wc2n", "Sydney");
  wc1Offset = prefs.getFloat("wc1o", 5.5f);
  wc2Offset = prefs.getFloat("wc2o", 11.0f);
  prefs.end();
  if (wc1Name.length() == 0) wc1Name = "India";
  if (wc2Name.length() == 0) wc2Name = "Sydney";
}

void saveConfig(const String& ssid, const String& pass, const String& apikey,
                const String& cityName, const String& country, const String& tz,
                const String& w1n, float w1o, const String& w2n, float w2o) {
  prefs.begin(CONFIG_NAMESPACE, false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.putString("apikey", apikey);
  prefs.putString("city", cityName);
  prefs.putString("country", country);
  prefs.putString("tz", tz);
  prefs.putString("wc1n", w1n);
  prefs.putFloat("wc1o", w1o);
  prefs.putString("wc2n", w2n);
  prefs.putFloat("wc2o", w2o);
  prefs.end();
  wifiSsid = ssid;
  wifiPassword = pass;
  apiKey = apikey;
  city = cityName;
  countryCode = country;
  tzString = tz;
  wc1Name = w1n;
  wc1Offset = w1o;
  wc2Name = w2n;
  wc2Offset = w2o;
}

String escapeAttr(const String& s) {
  String r;
  for (unsigned int i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '&') r += "&amp;";
    else if (c == '\'') r += "&#39;";
    else if (c == '"') r += "&quot;";
    else if (c == '<') r += "&lt;";
    else r += c;
  }
  return r;
}

void sendConfigPage(bool isPost) {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><title>Deskbuddy</title></head><body style='font-family:sans-serif;max-width:400px;margin:20px'>";
  html += "<h1>Deskbuddy</h1>";
  if (isPost) {
    html += "<p style='color:green'>Saved. Rebooting...</p></body></html>";
    server.send(200, "text/html", html);
    delay(500);
    ESP.restart();
    return;
  }
  html += "<form method='post' action='/save'>";
  html += "<h2>WiFi</h2>";
  html += "SSID <input name='ssid' value='" + escapeAttr(wifiSsid) + "' style='width:100%'><br><br>";
  html += "Password <input type='password' name='pass' value='" + escapeAttr(wifiPassword) + "' style='width:100%'><br><br>";
  html += "<h2>Weather (OpenWeatherMap)</h2>";
  html += "City <input name='city' value='" + escapeAttr(city) + "' placeholder='Saint Petersburg' style='width:100%'><br><br>";
  html += "Country <input name='country' value='" + escapeAttr(countryCode) + "' placeholder='RU' maxlength='2' style='width:60px'><br><br>";
  html += "API Key <input name='apikey' value='" + escapeAttr(apiKey) + "' placeholder='your key' style='width:100%'><br><br>";
  html += "<h2>Timezone (local clock)</h2>";
  html += "TZ <input name='tz' value='" + escapeAttr(tzString) + "' placeholder='MSK-3' style='width:100%'><br><br>";
  html += "<h2>World Clock (2 cities)</h2>";
  html += "City 1 <input name='wc1n' value='" + escapeAttr(wc1Name) + "' placeholder='India'> UTC offset <input type='number' step='0.5' name='wc1o' value='" + String(wc1Offset, 1) + "' style='width:60px'> h<br><br>";
  html += "City 2 <input name='wc2n' value='" + escapeAttr(wc2Name) + "' placeholder='Sydney'> UTC offset <input type='number' step='0.5' name='wc2o' value='" + String(wc2Offset, 1) + "' style='width:60px'> h<br><br>";
  html += "<button type='submit'>Save & Reboot</button></form>";
  html += "<p><small>UTC offset: e.g. Moscow +3, India +5.5, Sydney +11, London 0, NY -5</small></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.method() != HTTP_POST) { server.send(405, "text/plain", "Method Not Allowed"); return; }
  String ssid = server.hasArg("ssid") ? server.arg("ssid") : wifiSsid;
  String pass = server.hasArg("pass") ? server.arg("pass") : wifiPassword;
  String apikey = server.hasArg("apikey") ? server.arg("apikey") : apiKey;
  String cityName = server.hasArg("city") ? server.arg("city") : city;
  String country = server.hasArg("country") ? server.arg("country") : countryCode;
  String tz = server.hasArg("tz") ? server.arg("tz") : tzString;
  String w1n = server.hasArg("wc1n") ? server.arg("wc1n") : wc1Name;
  String w2n = server.hasArg("wc2n") ? server.arg("wc2n") : wc2Name;
  float w1o = wc1Offset, w2o = wc2Offset;
  if (server.hasArg("wc1o")) w1o = server.arg("wc1o").toFloat();
  if (server.hasArg("wc2o")) w2o = server.arg("wc2o").toFloat();
  if (cityName.length() == 0) cityName = "Saint Petersburg";
  if (country.length() == 0) country = "RU";
  if (w1n.length() == 0) w1n = "City1";
  if (w2n.length() == 0) w2n = "City2";
  saveConfig(ssid, pass, apikey, cityName, country, tz, w1n, w1o, w2n, w2o);
  sendConfigPage(true);
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.on("/", []() { sendConfigPage(false); });
  server.on("/save", handleSave);
  server.onNotFound([]() { sendConfigPage(false); });
  server.begin();
  configMode = true;
}

void setupWebServerSTA() {
  server.on("/", []() { sendConfigPage(false); });
  server.on("/save", handleSave);
  server.begin();
}

// ==================================================
// 4. DRAWING & ANIMATION
// ==================================================

void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft) {
  int ix = (int)x;
  int iy = (int)y;
  int iw = (int)w;
  int ih = (int)h;
  u8g2.setDrawColor(0);  // Black

  // ANGRY: Sharp slanted cut
  if (mood == MOOD_ANGRY) {
    if (isLeft)
      for (int i = 0; i < 16; i++) u8g2.drawLine(ix, iy + i, ix + iw, iy - 6 + i);
    else
      for (int i = 0; i < 16; i++) u8g2.drawLine(ix, iy - 6 + i, ix + iw, iy + i);
  }
  // SAD: Inverse slanted cut
  else if (mood == MOOD_SAD) {
    if (isLeft)
      for (int i = 0; i < 16; i++) u8g2.drawLine(ix, iy - 6 + i, ix + iw, iy + i);
    else
      for (int i = 0; i < 16; i++) u8g2.drawLine(ix, iy + i, ix + iw, iy - 6 + i);
  }
  // HAPPY/LOVE: Cheek push up
  else if (mood == MOOD_HAPPY || mood == MOOD_LOVE || mood == MOOD_EXCITED) {
    u8g2.drawBox(ix, iy + ih - 12, iw, 14);
    u8g2.drawDisc(ix + iw / 2, iy + ih + 6, iw / 1.3, U8G2_DRAW_ALL);  // Round cut
  }
  // SLEEPY: Heavy lids
  else if (mood == MOOD_SLEEPY) {
    u8g2.drawBox(ix, iy, iw, ih / 2 + 2);
  }
  // SUSPICIOUS: One eye squint, one open
  else if (mood == MOOD_SUSPICIOUS) {
    if (isLeft) u8g2.drawBox(ix, iy, iw, ih / 2 - 2);
    else u8g2.drawBox(ix, iy + ih - 8, iw, 8);
  }
  u8g2.setDrawColor(1);  // Reset to white
}

void drawUltraProEye(Eye& e, bool isLeft) {
  int ix = (int)e.x;
  int iy = (int)e.y;
  int iw = (int)e.w;
  int ih = (int)e.h;

  // 1. Draw Sclera (White base)
  u8g2.setDrawColor(1);  // White
  u8g2.drawBox(ix, iy, iw, ih);

  // 2. Draw Pupil (Black inner eye) - Constrained to be inside
  // Calculate center of eye
  int cx = ix + iw / 2;
  int cy = iy + ih / 2;

  // Pupil size is proportional to eye size
  int pw = iw / 2.2;
  int ph = ih / 2.2;

  // Apply pupil offset (Gaze)
  int px = cx + (int)e.pupilX - (pw / 2);
  int py = cy + (int)e.pupilY - (ph / 2);

  // Clamp pupil to ensure it doesn't leave the eye completely
  if (px < ix) px = ix;
  if (px + pw > ix + iw) px = ix + iw - pw;
  if (py < iy) py = iy;
  if (py + ph > iy + ih) py = iy + ih - ph;

  u8g2.setDrawColor(0);  // Black
  u8g2.drawBox(px, py, pw, ph);

  // 3. Draw Specular Highlight (The "Glint" of life)
  // A tiny white dot in the top-right of the pupil
  if (iw > 15 && ih > 15) {
    u8g2.setDrawColor(1);  // White
    u8g2.drawDisc(px + pw - 4, py + 4, 2, U8G2_DRAW_ALL);
  }

  // 4. Apply Eyelid Masks (Expressions)
  drawEyelidMask(e.x, e.y, e.w, e.h, currentMood, isLeft);
}

void updatePhysicsAndMood() {
  unsigned long now = millis();
  breathVal = sin(now / 800.0) * 1.5;  // Breathing effect

  // --- BLINK LOGIC ---
  if (now > leftEye.nextBlinkTime) {
    leftEye.blinking = true;
    leftEye.lastBlink = now;
    rightEye.blinking = true;
    leftEye.nextBlinkTime = now + random(2000, 6000);
  }
  if (leftEye.blinking) {
    leftEye.targetH = 2;
    rightEye.targetH = 2;  // Close
    if (now - leftEye.lastBlink > 120) {
      leftEye.blinking = false;
      rightEye.blinking = false;
    }
  }

  // --- SACCADE (Gaze) LOGIC ---
  if (!leftEye.blinking && now - lastSaccade > saccadeInterval) {
    lastSaccade = now;
    saccadeInterval = random(500, 3000);

    // Pick direction
    int dir = random(0, 10);
    float lx = 0, ly = 0;

    if (dir < 4) {
      lx = 0;
      ly = 0;
    }  // Center
    else if (dir == 4) {
      lx = -6;
      ly = -4;
    }  // TL
    else if (dir == 5) {
      lx = 6;
      ly = -4;
    }  // TR
    else if (dir == 6) {
      lx = -6;
      ly = 4;
    }  // BL
    else if (dir == 7) {
      lx = 6;
      ly = 4;
    }  // BR
    else if (dir == 8) {
      lx = 8;
      ly = 0;
    }  // R
    else if (dir == 9) {
      lx = -8;
      ly = 0;
    }  // L

    // Move pupil target relative to center
    leftEye.targetPupilX = lx;
    leftEye.targetPupilY = ly;
    rightEye.targetPupilX = lx;
    rightEye.targetPupilY = ly;

    // Move eye container slightly (Head follow)
    leftEye.targetX = 18 + (lx * 0.3);
    leftEye.targetY = 14 + (ly * 0.3);
    rightEye.targetX = 74 + (lx * 0.3);
    rightEye.targetY = 14 + (ly * 0.3);
  }

  // --- MOOD SHAPES (Overrides targets) ---
  if (!leftEye.blinking) {
    float baseW = 36;
    float baseH = 36;

    // Breathing effect applied to height
    baseH += breathVal;

    switch (currentMood) {
      case MOOD_NORMAL:
        leftEye.targetW = baseW;
        leftEye.targetH = baseH;
        rightEye.targetW = baseW;
        rightEye.targetH = baseH;
        break;
      case MOOD_HAPPY:
      case MOOD_LOVE:
        leftEye.targetW = 40;
        leftEye.targetH = 32;
        rightEye.targetW = 40;
        rightEye.targetH = 32;
        break;
      case MOOD_SURPRISED:
        leftEye.targetW = 30;
        leftEye.targetH = 45;
        rightEye.targetW = 30;
        rightEye.targetH = 45;
        // Jitter pupil
        leftEye.targetPupilX += random(-1, 2);
        break;
      case MOOD_SLEEPY:
        leftEye.targetW = 38;
        leftEye.targetH = 30;
        rightEye.targetW = 38;
        rightEye.targetH = 30;
        break;
      case MOOD_ANGRY:
        leftEye.targetW = 34;
        leftEye.targetH = 32;
        rightEye.targetW = 34;
        rightEye.targetH = 32;
        break;
      case MOOD_SAD:
        leftEye.targetW = 34;
        leftEye.targetH = 40;
        rightEye.targetW = 34;
        rightEye.targetH = 40;
        break;
      case MOOD_SUSPICIOUS:
        leftEye.targetW = 36;
        leftEye.targetH = 20;  // Left Squint
        rightEye.targetW = 36;
        rightEye.targetH = 42;  // Right Wide
        break;
    }
  }

  leftEye.update();
  rightEye.update();
}

void drawEmoPage() {
  updatePhysicsAndMood();

  // Draw Floating Particles based on Mood
  u8g2.setDrawColor(1);  // White
  if (currentMood == MOOD_LOVE) {
    u8g2.drawXBMP(56, 0, 16, 16, bmp_heart);
  } else if (currentMood == MOOD_SLEEPY) {
    u8g2.drawXBMP(110, 0, 16, 16, bmp_zzz);
  } else if (currentMood == MOOD_ANGRY) {
    u8g2.drawXBMP(56, 0, 16, 16, bmp_anger);
  }

  drawUltraProEye(leftEye, true);
  drawUltraProEye(rightEye, false);
}

// --- STANDARD PAGES ---
void drawForecastPage() {
  u8g2.setDrawColor(1);  // White
  u8g2.drawBox(0, 0, 128, 16);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setDrawColor(0);  // Black text on white background
  u8g2.setCursor(15, 12);
  u8g2.print("3-DAY FCST");
  u8g2.setDrawColor(1);  // White
  u8g2.drawLine(42, 16, 42, 64);
  u8g2.drawLine(85, 16, 85, 64);
  for (int i = 0; i < 3; i++) {
    int xStart = i * 43;
    int centerX = xStart + 21;
    u8g2.setFont(u8g2_font_6x10_tr);
    String d = fcast[i].dayName;
    if (d == "") d = "Wait";
    u8g2.setCursor(centerX - (u8g2.getStrWidth(d.c_str()) / 2), 24);
    u8g2.print(d);
    u8g2.drawXBMP(centerX - 8, 30, 16, 16, getMiniIcon(fcast[i].iconType));
    u8g2.setFont(u8g2_font_ncenB10_tr);
    String tempStr = String(fcast[i].temp);
    int w = u8g2.getStrWidth(tempStr.c_str());
    u8g2.setCursor(centerX - (w / 2) - 2, 56);
    u8g2.print(tempStr);
    u8g2.drawDisc(centerX + (w / 2) + 1, 48, 2, U8G2_DRAW_ALL);
  }
}
void drawClock() {
  struct tm t;
  if (!getLocalTime(&t)) {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(40, 32);
    u8g2.print("Syncing");
    return;
  }
  String ampm = (t.tm_hour >= 12) ? "PM" : "AM";
  int h12 = t.tm_hour % 12;
  if (h12 == 0) h12 = 12;
  u8g2.setDrawColor(1);  // White
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(105, 9);
  u8g2.print(ampm);
  u8g2.setFont(u8g2_font_ncenB18_tr);
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", h12, t.tm_min);
  int w = u8g2.getStrWidth(timeStr);
  u8g2.setCursor((SCREEN_WIDTH - w) / 2, 32);
  u8g2.print(timeStr);
  u8g2.setFont(u8g2_font_ncenR10_tr);
  char dateStr[20];
  strftime(dateStr, 20, "%a, %b %d", &t);
  w = u8g2.getStrWidth(dateStr);
  u8g2.setCursor((SCREEN_WIDTH - w) / 2, 50);
  u8g2.print(dateStr);

  // BMP280: temperature and pressure at bottom
  if (bmp280Ok) {
    bmpTemp = bmp.readTemperature();
    bmpPressure = bmp.readPressure() / 100.0f;  // Pa -> hPa
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_6x10_tr);
    char buf[24];
    snprintf(buf, sizeof(buf), "%.0fC  %.0f hPa", bmpTemp, bmpPressure);
    w = u8g2.getStrWidth(buf);
    u8g2.setCursor((SCREEN_WIDTH - w) / 2, 62);
    u8g2.print(buf);
  }
}
void drawWeatherCard() {
  if (WiFi.status() != WL_CONNECTED) {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(40, 32);
    u8g2.print("No WiFi");
    return;
  }
  u8g2.setDrawColor(1);  // White
  u8g2.drawXBMP(96, 0, 32, 32, getBigIcon(weatherMain));
  u8g2.setFont(u8g2_font_ncenB10_tr);
  String c = city;
  c.toUpperCase();
  u8g2.setCursor(0, 10);
  // Обрезаем название города если не помещается
  int cityW = u8g2.getStrWidth(c.c_str());
  if (cityW > 90) {
    while (cityW > 85 && c.length() > 0) {
      c = c.substring(0, c.length() - 1);
      cityW = u8g2.getStrWidth(c.c_str());
    }
    c += ".";
  }
  u8g2.print(c);
  u8g2.setFont(u8g2_font_ncenB18_tr);
  int tempInt = (int)temperature;
  String tempStr = String(tempInt);
  u8g2.setCursor(0, 38);
  u8g2.print(tempInt);
  int w = u8g2.getStrWidth(tempStr.c_str());
  u8g2.drawDisc(0 + w + 5, 16, 4, U8G2_DRAW_ALL);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawXBMP(88, 32, 8, 8, bmp_tiny_drop);
  u8g2.setCursor(100, 38);
  u8g2.print(humidity);
  u8g2.print("%");
  u8g2.setCursor(88, 48);
  u8g2.print("~");
  u8g2.print((int)feelsLike);
  u8g2.drawLine(0, 50, 128, 50);
  u8g2.setCursor(0, 58);
  // Обрезаем описание если слишком длинное
  String desc = weatherDesc;
  int descW = u8g2.getStrWidth(desc.c_str());
  if (descW > 128) {
    while (descW > 120 && desc.length() > 0) {
      desc = desc.substring(0, desc.length() - 1);
      descW = u8g2.getStrWidth(desc.c_str());
    }
    desc += "...";
  }
  u8g2.print(desc);
}
void drawWorldClock() {
  time_t now;
  time(&now);
  time_t t1 = now + (long)(wc1Offset * 3600);
  time_t t2 = now + (long)(wc2Offset * 3600);
  struct tm* tm1 = gmtime(&t1);
  struct tm* tm2 = gmtime(&t2);
  int h1 = tm1->tm_hour, m1 = tm1->tm_min;
  int h2 = tm2->tm_hour, m2 = tm2->tm_min;
  u8g2.setDrawColor(1);  // White
  u8g2.drawBox(0, 0, 128, 16);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setDrawColor(0);  // Black text on white background
  u8g2.setCursor(30, 12);
  u8g2.print("WORLD CLOCK");
  u8g2.setDrawColor(1);  // White
  u8g2.drawLine(64, 18, 64, 54);
  u8g2.setFont(u8g2_font_6x10_tr);
  String n1 = wc1Name;
  String n2 = wc2Name;
  if (n1.length() > 6) n1 = n1.substring(0, 6);
  if (n2.length() > 6) n2 = n2.substring(0, 6);
  u8g2.setCursor(20, 26);
  u8g2.print(n1);
  u8g2.setFont(u8g2_font_ncenB10_tr);
  char s1[10], s2[10];
  sprintf(s1, "%02d:%02d", h1, m1);
  sprintf(s2, "%02d:%02d", h2, m2);
  u8g2.setCursor(10, 48);
  u8g2.print(s1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(75, 26);
  u8g2.print(n2);
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(70, 48);
  u8g2.print(s2);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(35, 58);
  u8g2.print("Tap to Exit");
}

// ==================================================
// 5. BOOT & MAIN
// ==================================================

void playBootAnimation() {
  u8g2.setDrawColor(1);  // White
  int cx = 64;
  int cy = 32;
  // Expanding Circle Wipe
  for (int r = 0; r < 80; r += 4) {
    u8g2.clearBuffer();
    u8g2.drawDisc(cx, cy, r, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    delay(10);
  }
  // Inverse Wipe
  for (int r = 0; r < 80; r += 4) {
    u8g2.clearBuffer();
    u8g2.drawDisc(cx, cy, 80, U8G2_DRAW_ALL);
    u8g2.setDrawColor(0);  // Black
    u8g2.drawDisc(cx, cy, r, U8G2_DRAW_ALL);
    u8g2.setDrawColor(1);  // Reset to white
    u8g2.sendBuffer();
    delay(10);
  }

  // Text Display - "ESCLabs"
  u8g2.setFont(u8g2_font_ncenB10_tr);
  String bootText = "ESCLabs";

  // Calculate width to center it perfectly
  int w = u8g2.getStrWidth(bootText.c_str());

  u8g2.clearBuffer();
  u8g2.setCursor((SCREEN_WIDTH - w) / 2, 36);
  u8g2.print(bootText);
  u8g2.sendBuffer();
  delay(2000);
}

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  u8g2.begin();
  u8g2.setContrast(DEFAULT_CONTRAST);
  currentContrast = DEFAULT_CONTRAST;
  u8g2.setDrawColor(1);  // White

  // Init Eyes Center
  leftEye.init(18, 14, 36, 36);
  rightEye.init(74, 14, 36, 36);

  playBootAnimation();

  loadConfig();

  if (wifiSsid.length() == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(5, 20);
    u8g2.print("No WiFi config.");
    u8g2.setCursor(5, 36);
    u8g2.print("Connect to:");
    u8g2.setCursor(5, 48);
    u8g2.print(AP_SSID);
    u8g2.sendBuffer();
    startAPMode();
    return;
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(25, 32);
  u8g2.print("Connecting");
  u8g2.sendBuffer();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start < WIFI_TIMEOUT_MS)) {
    delay(200);
  }
  if (WiFi.status() != WL_CONNECTED) {
    u8g2.clearBuffer();
    u8g2.setCursor(5, 20);
    u8g2.print("WiFi failed.");
    u8g2.setCursor(5, 36);
    u8g2.print("Connect to");
    u8g2.setCursor(5, 48);
    u8g2.print(AP_SSID);
    u8g2.sendBuffer();
    startAPMode();
    return;
  }
  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  Wire.begin(BMP280_SDA, BMP280_SCL);
  bmp280Ok = bmp.begin(0x77) || bmp.begin(0x76);  // try both I2C addresses
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
  setupWebServerSTA();
}

void loop() {
  if (configMode) {
    dnsServer.processNextRequest();
    server.handleClient();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(5, 20);
    u8g2.print("Config: connect");
    u8g2.setCursor(5, 36);
    u8g2.print("to " AP_SSID);
    u8g2.setCursor(5, 48);
    u8g2.print("Then open 192.168.4.1");
    u8g2.sendBuffer();
    return;
  }

  unsigned long now = millis();
  handleTouch();
  server.handleClient();
  if (now - lastWeatherUpdate > 600000) {
    getWeatherAndForecast();
    lastWeatherUpdate = now;
  }

  if (currentPage < 3 && now - lastPageSwitch > PAGE_INTERVAL) {
    currentPage++;
    if (currentPage > 2) currentPage = 0;
    lastPageSwitch = now;
    lastSaccade = 0;
  }

  u8g2.clearBuffer();
  switch (currentPage) {
    case 0: drawEmoPage(); break;
    case 1: drawClock(); break;
    case 2: drawWeatherCard(); break;
    case 3: drawWorldClock(); break;
    case 4: drawForecastPage(); break;
  }
  u8g2.sendBuffer();
}
