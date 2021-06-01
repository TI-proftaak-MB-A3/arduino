// Gebruikte bibliotheken
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

// Zelf instellen voor je eigen WLAN
const char* WLAN_SSID = "Safehouse";
const char* WLAN_ACCESS_KEY = "WwiELp66!";

// CLIENT_ID moet uniek zijn, dus zelf aanpassen (willekeurige letters en cijfers)
const char* MQTT_CLIENT_ID = "vfduvfxjhuyrfdfvkjkffyeduhbnbfyt6665e3whiyr85yg5";
const int LINE_LENGTH = 16;
// Gegevens van de MQTT broker die we in TI-1.4 kunnen gebruiken
const char* MQTT_BROKER_URL = "sendlab.nl";
const int   MQTT_PORT = 11884;
const char* MQTT_USERNAME = "ti";
const char* MQTT_PASSWORD = "tiavans";

// Definieer de MQTT topics

//Pushen
const char* MQTT_TOPIC_CODE = "ti/1.4/a3/code";
const char* MQTT_TOPIC_AWNSER = "ti/1.4/awnser";

//Subscribe
const char* MQTT_TOPIC_QUESTION = "ti/1.4/a3/question";

// Push & Subscribe




//
//const char* MQTT_TOPIC_BUTTON1 = "ti/1.4/demo/btn1";
//const char* MQTT_TOPIC_BUTTON2 = "ti/1.4/demo/btn2";
//const char* MQTT_TOPIC_LEDS[NR_OF_LEDS] = {
//  "ti/1.4/demo/led/blue",
//  "ti/1.4/demo/led/red",
//  "ti/1.4/demo/led/yellow",
//  "ti/1.4/demo/led/green"
//};

// Definieer de te gebruiken Quality of Service (QoS)
const int MQTT_QOS = 0;

WiFiClient wifiClient;
//WiFiClientSecure wifiClient; // Om een met TLS beveiligde verbinding te kunnen gebruiken
PubSubClient mqttClient(wifiClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is het default I2C adres van de LCD module

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Logging
  Serial.print("MQTT callback called for topic ");
  Serial.println(topic);
  Serial.print("Payload length ");
  Serial.println(length);
  
  // Kijk welk topic is ontvangen en handel daarnaar

  if(strcmp(topic, MQTT_TOPIC_QUESTION) == 0){
    //code jasper lcd);
    char txt[LINE_LENGTH + 1];
    for (int i = 0; i < LINE_LENGTH + 1; i++) { txt[i] = '\0'; }
    strncpy(txt, (const char *) payload, length > 16 ? 16 : length);
    // Laat de tekst zien in zowel log als op het LCD
    Serial.print("Text: ");
    Serial.println(txt);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(MQTT_TOPIC_QUESTION);
    lcd.setCursor(0, 1);
    lcd.print(txt);
  }

  
//  if (strcmp(topic, MQTT_TOPIC_LCD) == 0) {
//    // De payload is een tekst voor op het LCD
//    // Let op, geen null-ter`cd.init();
}

void setup() {
  
  
  // Gekleurde LEDs worden aangestuurd met de ESP32 LED control (LEDC) library
  // Initialiseer de LED control kanalen
//  for (int led = 0; led < NR_OF_LEDS; led++) {
//    ledcAttachPin(LED_PINS[led], LED_CHANNELS[led]);
//    ledcSetup(LED_CHANNELS[led], 12000, 8); // 12 kHz PWM, 8-bit resolutie
//    ledcWrite(LED_CHANNELS[led], ledIntensities[led]);
//  }
  
  // Open de verbinding naar de seriële terminal
  Serial.begin(115200);
  Serial.println("MQTT output:");
  
  // Zet de LCD op
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("booted");

  // Zet de WiFi verbinding op
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_ACCESS_KEY);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Om een met TLS beveiligde verbinding te kunnen gebruiken zonder certificaten
  //wifiClient.setInsecure(); // Skip de controle, niet ideaal maar werkt voorlopig wel

  
  // Zet de MQTT client op
  mqttClient.setServer(MQTT_BROKER_URL, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  // Maak verbinding met de MQTT broker
  if (!mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("Failed to connect to MQTT broker");
  } else {
    Serial.println("Connected to MQTT broker");
  }

 if (!mqttClient.subscribe(MQTT_TOPIC_QUESTION, MQTT_QOS)) {
    Serial.print("Failed to subscribe topic: ");
    Serial.println(MQTT_TOPIC_QUESTION);
  } else {
    Serial.print("Subscribed to topic: ");
    Serial.println(MQTT_TOPIC_QUESTION);
  }
  //code

}

void loop() {
   // Nodig om de MQTT client zijn werk te laten doen
  mqttClient.loop();


}
