#include "images.h"
#include "config.h"
#include <Wire.h>
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include <TimeLib.h>
#include <Adafruit_INA219.h>
#include <ESP8266WiFi.h>

// externs (defined in SolarBot.ino)
extern SSD1306Wire display;
extern OLEDDisplayUi ui;
extern Adafruit_INA219 ina219;

// Splash: show "☀ Solar Bot" and run animations (WiFi, gear, progress)
void showSplashScreen() {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "☀ Solar Bot");
  display.display();
  delay(700);

  // WiFi animation while attempting connection (will be managed here)
  unsigned long wifiStart = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    #ifdef ESP32
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    // client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    #endif

    #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    secured_client.setTrustAnchors(&cert);
    // client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
    #endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  int wfFrame = 0;
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 8000) {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 12, "☀ Solar Bot");
    // draw wi-fi frame
    display.drawXbm(56, 30, wifiFrame_width, wifiFrame_height, wifiFrames[wfFrame]);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 50, "Connecting WiFi...");
    display.display();
    wfFrame = (wfFrame + 1) % 3;
    delay(350);
  }

  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(64, 20, "☀ Solar Bot");
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 46, WiFi.localIP().toString());
    display.display();
    delay(900);
  } else {
    display.drawString(64, 20, "☀ Solar Bot");
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 46, "WiFi Failed");
    display.display();
    delay(900);
  }

  // Gear animation for sensor init
  int gframe = 0;
  bool sensorOK = false;
  unsigned long sensorStart = millis();
  while (millis() - sensorStart < 4000) {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 12, "☀ Solar Bot");
    display.drawXbm(58, 30, gearFrame_width, gearFrame_height, gearFrames[gframe]);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 50, "Loading Sensors...");
    display.display();
    gframe = (gframe + 1) % 4;
    delay(180);
    if (!sensorOK) {
      sensorOK = ina219.begin();
      if (sensorOK) break;
    }
  }

  // Finalizing with progress bar
  unsigned long finalizeStart = millis();
  while (millis() - finalizeStart < 1500) {
    int elapsed = millis() - finalizeStart;
    int progress = map(elapsed, 0, 1500, 0, 100);
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 12, "☀ Solar Bot");
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 40, sensorOK ? "Finalizing..." : "Retrying Sensor...");
    display.drawProgressBar(14, 52, 100, 6, progress);
    display.display();
    delay(80);
  }

  // Ready
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "☀ Solar Bot");
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 46, sensorOK ? "System Ready" : "Startup Error");
  display.display();
  delay(900);
  display.clear();
  display.display();
}

// UI frames
void drawSensorFrame(OLEDDisplay *disp, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  float v = ina219.getBusVoltage_V();
  float i = ina219.getCurrent_mA();
  float p = ina219.getPower_mW();
  disp->setTextAlignment(TEXT_ALIGN_LEFT);
  disp->setFont(ArialMT_Plain_10);
  disp->drawString(0 + x, 0 + y, "🔋 Solar Sensor");
  disp->drawString(0 + x, 14 + y, "V: " + String(v,2) + " V");
  disp->drawString(0 + x, 26 + y, "I: " + String(i,1) + " mA");
  disp->drawString(0 + x, 38 + y, "P: " + String(p,1) + " mW");
  disp->drawString(0 + x, 52 + y, "WiFi: " + WiFi.localIP().toString());
}

void drawClockFrame(OLEDDisplay *disp, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  String timenow = String(hour()) + ":" + (minute() < 10 ? "0" : "") + String(minute()) + ":" + (second() < 10 ? "0" : "") + String(second());
  disp->setTextAlignment(TEXT_ALIGN_CENTER);
  disp->setFont(ArialMT_Plain_24);
  disp->drawString(64 + x, 18 + y, timenow);
  disp->setFont(ArialMT_Plain_10);
  disp->drawString(64 + x, 48 + y, "✅ System OK | Running");
}

// Register frames and update
FrameCallback frames[] = { drawSensorFrame, drawClockFrame };
int frameCount = 2;

void initOLED() {
  display.init();
  display.flipScreenVertically();
  ui.setTargetFPS(60);
  ui.setTimePerFrame(5000); // 5 seconds
  ui.setIndicatorPosition(TOP);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, frameCount);
  ui.init();
  showSplashScreen();
}

void updateOLED() {
  ui.update();
}
