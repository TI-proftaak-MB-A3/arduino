
// Gebruikte bibliotheken
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

int button_1 = 2; //pin2
int button_2 = 4; //pin4
int button_3 = 5; //pin5
int button_4 = 12; //pin12
  
bool vbut1 = false;
bool vbut2 = false;
bool vbut3 = false;
bool vbut4 = false;
//RIDE ID
const String RIDE_ID = "07";

// Zelf instellen voor je eigen WLAN
const char* WLAN_SSID = "iPhone van Robin";
const char* WLAN_ACCESS_KEY = "R0biny0!@";

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
 char* MQTT_TOPIC_CODE = "ti/1.4/a3/code";
 char* MQTT_TOPIC_AWNSER = "ti/1.4/awnser";

//Subscribe
 char* MQTT_TOPIC_QUESTION = "ti/1.4/a3/question";

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

  if(strcmp(topic, MQTT_TOPIC_CODE) == 0){
    //code jasper lcd);
    char txt[LINE_LENGTH + 1];
    for (int i = 0; i < LINE_LENGTH + 1; i++) { txt[i] = '\0'; }
    strncpy(txt, (const char *) payload, length > 16 ? 16 : length);
    // Laat de tekst zien in zowel log als op het LCD
    Serial.print("Code: ");
    Serial.println(txt);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(MQTT_TOPIC_CODE);
    lcd.setCursor(0, 1);
    lcd.print(txt);
  }

  
//  if (strcmp(topic, MQTT_TOPIC_LCD) == 0) {
//    // De payload is een tekst voor op het LCD
//    // Let op, geen null-ter`cd.init();
}

String code_gen(){
  int random_num = random(0,99);
  String number = (String)random_num;
  Serial.print("random number: ");
  Serial.println(number);
  String STRING_QUESTION_ID = "";
  if(random_num < 10){
     String temp_num = ""+number;
     STRING_QUESTION_ID = "0" + temp_num;
  } else {
     STRING_QUESTION_ID = number;
  }
  String ride = RIDE_ID;
  String random_last = (String)random(10,99);
  Serial.print("Ride: ");
  Serial.println(ride);
  Serial.print("Question id: ");
  Serial.println(STRING_QUESTION_ID);
  Serial.print("Random last: ");
  Serial.println(random_last);
  return ride+""+STRING_QUESTION_ID+""+random_last;
}

bool subscribe(char* topic){
  return mqttClient.subscribe(topic, MQTT_QOS);
}

bool unsubscribe(char* topic){
  return mqttClient.unsubscribe(topic);
}


void setup() {
  pinMode (button_1, INPUT);
pinMode (button_2, INPUT);
pinMode (button_3, INPUT);
pinMode (button_4, INPUT);
  
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

  Serial.print("Subscribing to topic: ");
  Serial.println(MQTT_TOPIC_CODE);
  Serial.print("Subscribtion: ");
  Serial.println(subscribe(MQTT_TOPIC_CODE));

  
  String code = code_gen();

  Serial.print("Subscribing to topic: ");
  Serial.println(MQTT_TOPIC_CODE);
  Serial.print("Subscribtion: ");
  Serial.println(mqttClient.publish(MQTT_TOPIC_CODE, code.c_str()));
  
  //code








void loop() {
// buttoncheck();
  entry();


}
void buttoncheck(){
   vbut1 = digitalRead(button_1);
   vbut2 = digitalRead(button_2);
   vbut3 = digitalRead(button_3);
   vbut4 = digitalRead(button_4);

   
if (vbut1 == HIGH){
  digitalWrite(testled, HIGH);
}else if(vbut2 == HIGH){
  digitalWrite(testled, LOW);}
  else if(vbut3 == HIGH){
  digitalWrite(testled, HIGH);}
  else if(vbut4 == HIGH){
  digitalWrite(testled, LOW);}
  

}

void entry(){
   
     vbut1 = digitalRead(button_1);
      entrycode = random(9999);
  if (!buttonpressed){
     
  if (vbut1 == HIGH){
    lcd.clear();
    lcd.print("Code: ");
    lcd.setCursor(7,0);
   lcd.print(entrycode); 
   
     buttonpressed = true;
     lcdwritetimer();
     

}
}
}
   

   // Nodig om de MQTT client zijn werk te laten doen
  mqttClient.loop();



}
