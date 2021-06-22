// Gebruikte bibliotheken
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

int button_1 = 12; //pin2
int button_2 = 18; //pin4
int button_3 = 4;  //pin5
int button_4 = 2;  //pin12

int timer = 30;

bool buttonPressed = false;

bool mqttConnected = false;

bool awnserpressed = false;

bool vbut1 = false;
bool vbut2 = false;
bool vbut3 = false;
bool vbut4 = false;

float currentTime;
float oldTime = 0;
float deltaTime;

bool started = false;

//Custom chars
byte phoneChar[] = {
    B11111,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11011,
    B11111};

const String UNIQUE_DEVICE_ID = "GJGFKHGJGYFG";

//RIDE ID
const String RIDE_ID = "07";
const String RIDE_NAME = "De Cobra";

// Zelf instellen voor je eigen WLAN
const char *WLAN_SSID = "Safehouse";
const char *WLAN_ACCESS_KEY = "WwiELp66!";

// CLIENT_ID moet uniek zijn, dus zelf aanpassen (willekeurige letters en cijfers)
const char *MQTT_CLIENT_ID = "vfduvfxjhuyrfdfvkjkffyeduhbnbfyt6665e3whiyr85yg5";
const int LINE_LENGTH = 16;
// Gegevens van de MQTT broker die we in TI-1.4 kunnen gebruiken
const char *MQTT_BROKER_URL = "sendlab.nl";
const int MQTT_PORT = 11884;
const char *MQTT_USERNAME = "ti";
const char *MQTT_PASSWORD = "tiavans";

// Definieer de MQTT topics

//Pushen
String concat_unique_id = "ti/1.4/a3/code/"+UNIQUE_DEVICE_ID;
char *mqtt_topic_code = (char*)concat_unique_id.c_str();
char *mqtt_topic_awnser = "ti/1.4/awnser";
char *mqtt_current_code = "not created";
String CODE = "";
int current_connected = 0;

// Definieer de te gebruiken Quality of Service (QoS)
const int MQTT_QOS = 0;
WiFiClient wifiClient;
//WiFiClientSecure wifiClient; // Om een met TLS beveiligde verbinding te kunnen gebruiken
PubSubClient mqttClient(wifiClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is het default I2C adres van de LCD module
bool subscribe(char *topic)
{
  return mqttClient.subscribe(topic, MQTT_QOS);
}

bool unsubscribe(char *topic)
{
  return mqttClient.unsubscribe(topic);
}

bool push_code(char *topic, String code)
{
  Serial.println("Pushing: Topic: " + (String)topic + " code: " + code);
  return mqttClient.publish(topic, code.c_str());
}

void (*resetFunc)(void) = 0;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // Logging
  Serial.print("MQTT callback called for topic ");
  Serial.println(topic);
  Serial.print("Payload length ");
  Serial.println(length);

  if (mqtt_current_code != " not created")
  {
    String code_topic = "ti/1.4/a3/" + CODE;
    mqtt_current_code = (char *)code_topic.c_str();
  }

  // Kijk welk topic is ontvangen en handel daarnaar
  if (strcmp(topic, mqtt_current_code) == 0)
  {
    char txt[LINE_LENGTH + 1];
    for (int i = 0; i < LINE_LENGTH + 1; i++)
    {
      txt[i] = '\0';
    }
    strncpy(txt, (const char *)payload, length > 16 ? 16 : length);
    if ((String)txt == "connect")
    {
      Serial.println("Connect request received");
      current_connected++;
      if (current_connected < 11)
      {
        if (current_connected == 10)
        {
          if (timer > 5)
          {
            timer = 6;
          }
        }

        if (current_connected < 10)
        {
          lcd.setCursor(12, 1);
          lcd.print(current_connected);
        }
        else
        {
          lcd.setCursor(11, 1);
          lcd.print(current_connected);
        }
        String code_topic = "ti/1.4/a3/" + CODE;
        mqtt_current_code = (char *)code_topic.c_str();
        push_code(mqtt_current_code, "accepted");
      }
      else
      {
        String code_topic = "ti/1.4/a3/" + CODE;
        mqtt_current_code = (char *)code_topic.c_str();
        push_code(mqtt_current_code, "FULL");
      }
    }
    else if ((String)txt == "correct")
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GOED ANTWOORD!");
      lcd.setCursor(0, 1);
      lcd.print("kijk in je app!");
      delay(5000);
      resetFunc();
    }
    else if ((String)txt == "incorrect")
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FOUT ANTWOORD!");
      lcd.setCursor(0, 1);
      lcd.print("Probeer opnieuw!");
      delay(5000);
      resetFunc();
    }
  }
  if (strcmp(topic, mqtt_topic_code) == 0)
  {
    char txt[LINE_LENGTH + 1];
    for (int i = 0; i < LINE_LENGTH + 1; i++)
    {
      txt[i] = '\0';
    }
    strncpy(txt, (const char *)payload, length > 16 ? 16 : length);
    // Laat de tekst zien in zowel log als op het LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vul code in op ");
    lcd.createChar(15, phoneChar);
    lcd.setCursor(15, 0);
    lcd.write(15);
    lcd.setCursor(0, 1);
    lcd.print("" + (String)txt);
    String code_topic = "ti/1.4/a3/" + (String)txt;
    CODE = (String)txt;
    mqtt_current_code = (char *)code_topic.c_str();
    current_connected = 0;
    if (subscribe(mqtt_current_code))
    {
      Serial.print("Subscribed to topic: ");
      Serial.println((String)mqtt_current_code);
      lcd.setCursor(7, 1);
      lcd.print(timer);
      lcd.setCursor(11, 1);
      lcd.print("00/10");
      return;
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Haal hulp!");
      lcd.setCursor(0, 1);
      lcd.print("Error: GROUP SUB");
      while (true)
      {
        /* code */
      }
    }
  }
}

String code_gen()
{
  int random_num = random(0, 99);
  String number = (String)random_num;
  String STRING_QUESTION_ID = "";
  if (random_num < 10)
  {
    String temp_num = "" + number;
    STRING_QUESTION_ID = "0" + temp_num;
  }
  else
  {
    STRING_QUESTION_ID = number;
  }
  String ride = RIDE_ID;
  String random_last = (String)random(10, 99);
  Serial.print("Ride: ");
  Serial.println(ride);
  Serial.print("Question id: ");
  Serial.println(STRING_QUESTION_ID);
  Serial.print("Random last: ");
  Serial.println(random_last);
  return ride + "" + STRING_QUESTION_ID + "" + random_last;
}

void time_out()
{
  String code_topic = "ti/1.4/a3/" + CODE;
  mqtt_current_code = (char *)code_topic.c_str();
  push_code(mqtt_current_code, "E");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oops tijd is op.");
  lcd.setCursor(0, 1);
  lcd.print("Probeer Opnieuw!");

  delay(5000);
  lcd.clear();
  resetFunc();
}

void start_game()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 0);
  lcd.print("Tijd over: ");
  lcd.setCursor(11, 0);
  lcd.print(timer);
  lcd.setCursor(0, 1);
  lcd.print("A    B    C    D");
  String code_topic = "ti/1.4/a3/" + CODE;
  mqtt_current_code = (char *)code_topic.c_str();
  push_code(mqtt_current_code, "start");
}

void setup()
{
  pinMode(button_1, INPUT);
  pinMode(button_2, INPUT);
  pinMode(button_3, INPUT);
  pinMode(button_4, INPUT);

  // Open de verbinding naar de seriële terminal
  Serial.begin(115200);
  Serial.println("LOG:");

  // Zet de LCD op
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welkom bij:");
  lcd.setCursor(0, 1);
  lcd.print(RIDE_NAME);

  // Zet de WiFi verbinding op
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_ACCESS_KEY);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.print("Verbinding");
  lcd.setCursor(0, 1);
  lcd.print("gemaakt!");
  delay(2000);

  // Om een met TLS beveiligde verbinding te kunnen gebruiken zonder certificaten
  //wifiClient.setInsecure(); // Skip de controle, niet ideaal maar werkt voorlopig wel

  // Zet de MQTT client op
  lcd.clear();
  lcd.setCursor(0, 0);

  mqttClient.setServer(MQTT_BROKER_URL, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  // Maak verbinding met de MQTT broker
  if (!mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
  {
    Serial.println("Failed to connect to MQTT broker");
    lcd.print("Verbinding fout");
    lcd.setCursor(0, 1);
    lcd.print("MQTT broker");
    delay(2500);
  }
  else
  {
    Serial.println("Verbonden");
    mqttConnected = true;
  }

  if (mqttConnected)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Druk op knop");
    lcd.setCursor(0, 1);
    lcd.print("om te starten!");
    Serial.print("Subscribing to topic: ");
    Serial.println(mqtt_topic_code);
    Serial.print("Subscribtion: ");
    Serial.println(subscribe(mqtt_topic_code));
  }
}

void loop()
{
  if (oldTime == 0)
  {
    oldTime = millis();
  }

  currentTime = millis();
  // buttoncheck();
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_ACCESS_KEY);
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Verbinding maken");
    lcd.setCursor(0, 1);
    lcd.print("Moment A.U.B.");
    delay(5000);
    resetFunc();
  }
  if (!mqttConnected)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Haal hulp!");
    lcd.setCursor(0, 1);
    lcd.print("Error: MQTT fail");
    while (!mqttConnected)
    {

      while (!mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
      {
        Serial.print("Connecting mqtt");
      }

      mqttConnected = true;
    }
  }

  deltaTime = currentTime - oldTime;
  if (buttonPressed && current_connected > 0)
  {

    if (deltaTime >= 1000)
    {
      oldTime = currentTime;


      if (started && !awnserpressed)
      {
      timer--;
        lcd.setCursor(0, 0);
        lcd.print("Tijd over: ");
        lcd.setCursor(11, 0);
        if (timer < 10)
        {
          lcd.print("0");
          lcd.setCursor(12, 0);
        }
        lcd.print(timer);
      }
      else if (!started)
      {
      timer--;
        lcd.setCursor(7, 1);
        if (timer < 10)
        {
          lcd.print("0");
          lcd.setCursor(8, 1);
        }
        lcd.print(timer); 
      }
    }
    if (timer == 0)
    {
      timer = 30;
      if (started)
      {
        time_out();
      }
      else
      {
        started = true;
        start_game();
      }
    }
  }
  else
  {
    oldTime = currentTime;
  }

  entry();
  // Nodig om de MQTT client zijn werk te laten doen
  mqttClient.loop();
}

void buttoncheck()
{
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

void entry()
{

  vbut1 = digitalRead(button_1);
  vbut2 = digitalRead(button_2);
  vbut3 = digitalRead(button_3);
  vbut4 = digitalRead(button_4);

  if (!buttonPressed)
  {

    if (vbut1 == HIGH || vbut2 == HIGH || vbut3 == HIGH || vbut4 == HIGH)
    {
      buttonPressed = true;
      lcd.clear();
      lcd.setCursor(7, 0);
      String generated_code = code_gen();
      push_code(mqtt_topic_code, generated_code);
    }
  }

  if (started)
  {
    if (!awnserpressed)
    {

      if (vbut1 == HIGH)
      {
        if (!awnserpressed)
        {
          String code_topic = "ti/1.4/a3/" + CODE;
          mqtt_current_code = (char *)code_topic.c_str();
          push_code(mqtt_current_code, "A");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Jouw keuze: A");
          lcd.setCursor(0,1);
          lcd.print("Even Controleren");
          awnserpressed = true;
        }
      }
      else if (vbut2 == HIGH)
      {
        if (!awnserpressed)
        {
          String code_topic = "ti/1.4/a3/" + CODE;
          mqtt_current_code = (char *)code_topic.c_str();
          push_code(mqtt_current_code, "B");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Jouw keuze: B");
          lcd.setCursor(0,1);
          lcd.print("Even Controleren");

          awnserpressed = true;
        }
      }
      else if (vbut3 == HIGH)
      {
        if (!awnserpressed)
        {
          String code_topic = "ti/1.4/a3/" + CODE;
          mqtt_current_code = (char *)code_topic.c_str();
          push_code(mqtt_current_code, "C");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Jouw keuze: C");
          lcd.setCursor(0,1);
          lcd.print("Even Controleren");
          awnserpressed = true;
        }
      }
      else if (vbut4 == HIGH)
      {
        if (!awnserpressed)
        {
          String code_topic = "ti/1.4/a3/" + CODE;
          mqtt_current_code = (char *)code_topic.c_str();
          push_code(mqtt_current_code, "D");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Jouw keuze: D");
          lcd.setCursor(0,1);
          lcd.print("Even Controleren");
          awnserpressed = true;
        }
      }
    }
  }
}
