#include<Adafruit_GFX.h>
#include<Adafruit_ST7735.h>
#include<SPI.h>
#include<DHT.h>
#include<ESP32Servo.h>


#define TFT_CS 16
#define TFT_DC 17
#define TFT_RST 5
#define DHTPIN 25
#define DHTTYPE DHT11
#define LDRLPIN 12
#define LDRMPIN 14
#define LDRRPIN 27
#define DryServoPIN 26
#define Arm1PIN 33
#define Arm2PIN 32
#define FanPIN 34
#define HumidifierPIN 35
#define PumpPIN 15
#define LightPIN 2

//TFT: SCL-18, SDA-23

// 用于控制温湿度读取的定时器
unsigned long lastHTUpdate = 0;
const unsigned long HT_INTERVAL = 1000;  // 每秒更新一次

typedef struct {
  float h;
  float t;
} HT;

typedef struct {
  int L;
  int M;
  int R;
} LDRVAL;

HT ht;
LDRVAL LDRVal;

int HTcount;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
ESP32PWM pwm;
Servo Arm1Servo;
Servo Arm2Servo;
Servo DryServo;
int pos = 0;
int currentPos = 0;

void getHT() {
  ht.h = dht.readHumidity();
  ht.t = dht.readTemperature();
}

void displayHT() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(10,10);
  tft.println("Humidity: ");
  tft.println(ht.h);
  tft.println();
  tft.println("Temp: ");
  tft.println(ht.t);
  Serial.print(ht.h);
  Serial.print(" ");
  Serial.println(ht.t);
}

void getLDRVal() {
  LDRVal.L = analogRead(LDRLPIN);
  LDRVal.M = analogRead(LDRMPIN);
  LDRVal.R = analogRead(LDRRPIN);
  Serial.print(LDRVal.L);
  Serial.print(" ");
  Serial.print(LDRVal.M);
  Serial.print(" ");
  Serial.println(LDRVal.R);
}

void toggleLight() {
  if(digitalRead(LightPIN) == HIGH){
    digitalWrite(LightPIN, LOW);
    Serial.println("LightOFF");
  }
  else if (digitalRead(LightPIN) == LOW)
  {
    digitalWrite(LightPIN, HIGH);
    Serial.println("LightON");
  }
}

void toggleFan() {
  if(digitalRead(FanPIN) == HIGH){
    digitalWrite(FanPIN, LOW);
    Serial.println("FanOFF");
  }
  else if (digitalRead(FanPIN) == LOW)
  {
    digitalWrite(FanPIN, HIGH);
    Serial.println("FanON");
  }
}

void toggleHumidifier() {
  if(digitalRead(HumidifierPIN) == HIGH){
    digitalWrite(HumidifierPIN, LOW);
    Serial.println("HumidifierOFF");
  }
  else if (digitalRead(HumidifierPIN) == LOW)
  {
    digitalWrite(HumidifierPIN, HIGH);
    Serial.println("HumidifierON");
  }
}

void togglePump() {
  if(digitalRead(PumpPIN) == HIGH){
    digitalWrite(PumpPIN, LOW);
    Serial.println("PumpOFF");
  }
  else if (digitalRead(PumpPIN) == LOW)
  {
    digitalWrite(PumpPIN, HIGH);
    Serial.println("PumpON");
  }
}


void setup() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  Arm1Servo.setPeriodHertz(50);
  Arm2Servo.setPeriodHertz(50);
  DryServo.setPeriodHertz(50);
  Arm1Servo.attach(Arm1PIN, 500, 2500);
  Arm2Servo.attach(Arm2PIN, 500, 2500);
  DryServo.attach(DryServoPIN, 500, 2500);
  tft.initR(INITR_BLACKTAB);
  Serial.begin(9600);
  dht.begin();
  tft.setRotation(4);
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(2);
  getLDRVal();
  pinMode(LDRLPIN, INPUT);  
  pinMode(LDRMPIN, INPUT);
  pinMode(LDRRPIN, INPUT);
  pinMode(FanPIN, OUTPUT);
  pinMode(LightPIN, OUTPUT);
  pinMode(PumpPIN, OUTPUT);
  pinMode(HumidifierPIN, OUTPUT);
  digitalWrite(FanPIN, LOW);
  digitalWrite(LightPIN, LOW);
  digitalWrite(PumpPIN, LOW); 
  digitalWrite(HumidifierPIN, LOW);
  // for(pos = 0; pos <= 45; ++pos) {
  //   Arm1Servo.write(pos);
  //   Arm2Servo.write(pos);
  //   if(analogRead(LDRMPIN) > LDRVal.M) 
  //   {
  //     currentPos = pos;
  //     goto endArmSetup;
  //   }
  //   else
  //   {
  //     getLDRVal();
  //   }
  //   delay(50);
  // }
  // endArmSetup:
}


void loop() {
  if (millis() - lastHTUpdate >= HT_INTERVAL) {
    getHT();
    displayHT();
    lastHTUpdate = millis();
    // getLDRVal();
    // if(LDRVal.M < LDRVal.L || LDRVal.M < LDRVal.R) {
    //   if (analogRead(LDRMPIN) < analogRead(LDRLPIN)) {
    //     for (pos = currentPos; pos <= 45; ++pos){
    //       Arm1Servo.write(pos);
    //       Arm2Servo.write(pos);
          
    //     }
    //   }
    // }
  }
  ++HTcount;
}