#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define socket 23

String chat_id;
String your_name;
int numNewMessages;
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
//CHANGE THE "NAME" with your name
const char* ssid = "NAME";
const char* password = "WIFI_PASSWORD";

//CHANGE THE "TELEGRAM_BOT_TOKEN_"
#define BOTtoken "TELEGRAM_BOT_TOKEN"
#define CHAT_ID "738651906"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    
    if (text == "/socketon") {
      bot.sendMessage(chat_id, "SOCKET ACTIVE", "");
      digitalWrite(socket, LOW);
    }
    else if (text == "/socketoff") {
      bot.sendMessage(chat_id, "SOCKET DEACTIVE", "");
      digitalWrite(socket, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(socket, OUTPUT);
  digitalWrite(socket, HIGH);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
  Serial.println("START");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
    lastTimeBotRan = millis();
  }
}
