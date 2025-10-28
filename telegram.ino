#include <UniversalTelegramBot.h>
#include "config.h"

extern UniversalTelegramBot bot;
extern float busVoltage;
extern float current_mA;
extern float power_mW;

void handleNewMessages(int numNewMessages) {
  Serial.println("Handling New Message");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = bot.messages[i].chat_id;
    // Print the received message
    String text = bot.messages[i].text;

    Serial.println(text);

    String your_name = bot.messages[i].from_name;

    if (your_name == "")
    {
      your_name = "Guest";
    } 

    //starting text after /start message receive
    if (text == "/start") {
      String msg = "Welcome, Education Solar tracking " + your_name + ".\n";
      msg += "Use the following commands to control your outputs.\n\n";
      msg += "/voltage - show voltage\n";
      msg += "/current - show current\n";
      msg += "/power - show power\n";
      msg += "/status - show all\n";
      bot.sendMessage(chat_id, msg, "");
    }
      String status = "Voltage: " + String(busVoltage, 2) + " V\n";
          status += "Current: " + String(current_mA, 2) + " mA\n";
          status += "Power: " + String(power_mW, 2) + " mW\n";

    // command message after receive text from telegram
    if (text == "/voltage") bot.sendMessage(chat_id, "Voltage: " + String(busVoltage,2) + " V", "");
    if (text == "/current") bot.sendMessage(chat_id, "Current: " + String(current_mA,2) + " mA", "");
    if (text == "/power")   bot.sendMessage(chat_id, "Power: " + String(power_mW,2) + " mW", "");
    if (text == "/status")  bot.sendMessage(chat_id, "Status:\n"  + status, "");
  }
}
