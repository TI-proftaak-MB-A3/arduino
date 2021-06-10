
// Gebruikte bibliotheken
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

<<<<<<< Updated upstream
int button_1 = 2; //pin2
int button_2 = 4; //pin4
int button_3 = 5; //pin5
int button_4 = 12; //pin12
=======
int button_1 = 12; //pin2
int button_2 = 18; //pin4
int button_3 = 4;  //pin5
int button_4 = 2;  //pin12

String UNIQUE_DEVICE_ID = "ENTER_UNIQUE_STRING";

int timer = 30;
>>>>>>> Stashed changes

bool buttonPressed = false;

bool mqttConnected = false;
  
bool vbut1 = false;
bool vbut2 = false;
bool vbut3 = false;
bool vbut4 = false;


//Custom chars
byte phoneChar[] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11011,
  B11111
};



//RIDE ID
<<<<<<< Updated upstream
const String RIDE_ID = "07";
=======
const String RIDE_ID = "02";
const String RIDE_NAME = "Astrolica";
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
 char* MQTT_TOPIC_CODE = "ti/1.4/a3/code";
 char* MQTT_TOPIC_AWNSER = "ti/1.4/awnser";
 char* MQTT_CURRENT_CODE;
 char* MQTT_CURRENT_CONNECTED;
 int CURRENT_CONNECTED = 0;
=======
char *MQTT_TOPIC_CODE = "ti/1.4/a3/code/"+UNIQUE_DEVICE_ID;
char *MQTT_TOPIC_AWNSER = "ti/1.4/awnser";
char *MQTT_CURRENT_CODE = "not created";
String CODE = "";
int CURRENT_CONNECTED = 0;
>>>>>>> Stashed changes
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
bool subscribe(char* topic){
  return mqttClient.subscribe(topic, MQTT_QOS);
}

bool unsubscribe(char* topic){
  return mqttClient.unsubscribe(topic);
}

bool push_code(char* topic, String code){
  return mqttClient.publish(topic, code.c_str());
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Logging
  Serial.print("MQTT callback called for topic ");
  Serial.println(topic);
  Serial.print("Payload length ");
  Serial.println(length);

  Serial.println(MQTT_CURRENT_CODE);
  
  // Kijk welk topic is ontvangen en handel daarnaar

     if(strcmp(topic, MQTT_TOPIC_CODE) == 0){
      char txt[LINE_LENGTH + 1];
      for (int i = 0; i < LINE_LENGTH + 1; i++) { txt[i] = '\0'; }
      strncpy(txt, (const char *) payload, length > 16 ? 16 : length);
       // Laat de tekst zien in zowel log als op het LCD
       Serial.print("Code: ");
       Serial.println(txt);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Enter code in ");
       lcd.createChar(14, phoneChar);
       lcd.setCursor(14,0);
       lcd.write(14);
       lcd.setCursor(0, 1);
       lcd.print(""+(String)txt);
       String code_topic = "ti/1.4/a3/"+(String)txt;
       String connected_topic = "ti/1.4/a3/"+(String)txt+"/connected";
       MQTT_CURRENT_CODE = (char*)code_topic.c_str();
       CURRENT_CONNECTED = 0;
       MQTT_CURRENT_CONNECTED = (char*)connected_topic.c_str();
      if (subscribe(MQTT_CURRENT_CODE)){
      Serial.print("Subscribed to topic: ");
      Serial.println(MQTT_CURRENT_CODE);
     } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Please call help");
        lcd.setCursor(0,1);
        lcd.print("Error: GROUP SUB");
     }
    
    //     lcd.clear();
    //     lcd.setCursor(0,0);
    //     lcd.print("Please call help");
    //     lcd.setCursor(0,1);
    //     lcd.print("Error: GROUP SUB");
    //   } else {
    //     lcd.setCursor(11,1);
    //     lcd.print("00/10");
    //   } 
     } //else{
      
    //   CURRENT_CONNECTED++;
    //   Serial.println(CURRENT_CONNECTED);
    //   if (CURRENT_CONNECTED <10)
    //   {
    //     lcd.setCursor(12,1);
    //     lcd.print(CURRENT_CONNECTED);
        
    //   } else {
    //     lcd.setCursor(11,1);
    //     lcd.print(CURRENT_CONNECTED);
    //   }
    //   push_code(MQTT_CURRENT_CONNECTED, ""+CURRENT_CONNECTED);
      
    //    /* code */
    // }
    
  }

  
//  if (strcmp(topic, MQTT_TOPIC_LCD) == 0) {
//    // De payload is een tekst voor op het LCD
//    // Let op, geen null-ter`cd.init();
//}

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
  lcd.print("Connecting to:");
  lcd.setCursor(0,1);
  lcd.print(WLAN_SSID);

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
  lcd.clear();
  lcd.setCursor(0,0);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.print("WiFi connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);

  // Om een met TLS beveiligde verbinding te kunnen gebruiken zonder certificaten
  //wifiClient.setInsecure(); // Skip de controle, niet ideaal maar werkt voorlopig wel

  
  // Zet de MQTT client op
  lcd.clear();
  lcd.setCursor(0,0);

  mqttClient.setServer(MQTT_BROKER_URL, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  // Maak verbinding met de MQTT broker
  if (!mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("Failed to connect to MQTT broker");
    lcd.print("Connect failed");
    lcd.setCursor(0,1);
    lcd.print("to MQTT broker");
    delay(2500);
  } else {
    Serial.println("Connected to MQTT broker");
    mqttConnected = true;
  }

  if (mqttConnected){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press button to");
    lcd.setCursor(0,1);
    lcd.print("start!");
    Serial.print("Subscribing to topic: ");
    Serial.println(MQTT_TOPIC_CODE);
    Serial.print("Subscribtion: ");
    Serial.println(subscribe(MQTT_TOPIC_CODE));
  
  }


}



void loop() {
// buttoncheck();
if (!mqttConnected)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please call help");
  lcd.setCursor(0,1);
  lcd.print("Error: MQTT fail");
  while (!mqttConnected)
  {
    Serial.println("not Connected");
  }
  
}
  entry();
// Nodig om de MQTT client zijn werk te laten doen
  mqttClient.loop();

}

void buttoncheck(){
   vbut1 = digitalRead(button_1);
   vbut2 = digitalRead(button_2);
   vbut3 = digitalRead(button_3);
   vbut4 = digitalRead(button_4);

   
// if (vbut1 == HIGH){
//   digitalWrite(testled, HIGH);
// }else if(vbut2 == HIGH){
//   digitalWrite(testled, LOW);}
//   else if(vbut3 == HIGH){
//   digitalWrite(testled, HIGH);}
//   else if(vbut4 == HIGH){
//   digitalWrite(testled, LOW);}
  

}

void entry(){
   
     vbut1 = digitalRead(button_1);
  if (!buttonPressed){
     
    if (vbut1 == HIGH){
      buttonPressed = true;
      lcd.clear();
      lcd.setCursor(7,0);
      String generated_code = code_gen();
      push_code(MQTT_TOPIC_CODE,generated_code);    

    }
  }
}
  
