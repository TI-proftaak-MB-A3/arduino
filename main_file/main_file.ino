#include <LiquidCrystal_I2C.h>

int button_1 = 2;
int button_2 = 4;
int button_3 = 5;
int button_4 = 12;
int testled = 18;
int entrycode = 0;
int lcdColumns = 16;
int lcdRows = 2;
int countdown = 20;
int currenttime = 0;
int deltatime = 0;
int oldtime = 0;
bool timer = true;
bool vbut1 = false;
bool vbut2 = false;
bool vbut3 = false;
bool vbut4 = false;
bool buttonpressed = false;
const unsigned long eventinterval =1000;
unsigned long previoustime = 0;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  



void setup() {
  Serial.begin(9600);
pinMode (button_1, INPUT);
pinMode (button_2, INPUT);
pinMode (button_3, INPUT);
pinMode (button_4, INPUT);
pinMode (testled, OUTPUT);
lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
   lcd.print("Push a button to start");
}

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
void lcdwritetimer(){  

  while(timer){ 
    time();

  lcd.setCursor(12,1);
  lcd.print(countdown);
  Serial.println(countdown);
  if(countdown == 9){
        lcd.setCursor(13,1);
        lcd.print( "    ");
      }else if(countdown == 0){
        timer = false;
      }
}
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lets start!");}
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
   

  int time(){
  unsigned long currenttime = millis();
  
  if (currenttime - previoustime >= eventinterval){
    previoustime = currenttime;  
    countdown --;
    Serial.print("hoi");

    return countdown;
   }
  

}
