#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_INA219.h>
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "config.h"
#include "images.h"

Adafruit_INA219 ina219; 
SSD1306Wire display(0x3c, 14, 12);
OLEDDisplayUi ui(&display);


#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// extern functions
extern void initOLED();
extern void updateOLED();
extern void initINA219Module();
extern void readINA219();
extern void handleNewMessages(int numNewMessages);

unsigned long lastBotTime;
const unsigned long BOT_DELAY = 200;

void setup() {
  Serial.begin(115200);
    #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    secured_client.setTrustAnchors(&cert);
    // client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
    #endif

  // Initialize OLED and run splash (this will attempt WiFi and sensor init)
  initOLED();

  // Ensure INA219 initialized
  initINA219Module();

  Serial.println("System Ready.");
}

void loop() {
  // Telegram polling
  if (millis() - lastBotTime + BOT_DELAY) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastBotTime = millis();
  }
  //update serial monitor for sensor.
  readINA219();

  // Update OLED UI (frames)
  updateOLED();
}
