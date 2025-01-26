#define BLINKER_PRINT Serial
#define BLINKER_BLE

#include<Adafruit_GFX.h>
#include<Adafruit_ST7735.h>
#include<SPI.h>
#include<DHT.h>
#include<ESP32Servo.h>
#include <Blinker.h>
#include <Sr04.h>

#define TFT_CS 16
#define TFT_DC 17
#define TFT_RST 5
#define DHTPIN 25
#define DHTTYPE DHT11
// #define LDRLPIN 12
// #define LDRMPIN 14
// #define LDRRPIN 27
#define DryServoPIN 26
// #define Arm1PIN 33
// #define Arm2PIN 32
#define FanPIN 32 //0
#define HumidifierPIN 13
#define PumpPIN 15
#define LightPIN 2
#define SR04_trig 22
#define SR04_echo 21
#define twHumidifierPIN 27
#define twFanPIN 33 //14
#define twLightPIN 12

//TFT: SCL-18, SDA-23

// 用于控制温湿度读取的定时器
unsigned long lastHTUpdate = 0;
const unsigned long HT_INTERVAL = 1000;  // 每秒更新一次

#define HUMIDITY_THRESHOLD_LOW 40    // 湿度低于此值启动加湿器
#define HUMIDITY_THRESHOLD_HIGH 70   // 湿度高于此值启动风扇和干燥机

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
int DryServoPos = 0;

SR04 sr04 = SR04(SR04_echo, SR04_trig);
int waterLevel = 0;
#define waterLevelMax 3
#define waterLevelMin 8

// 状态标志：1 表示设备处于蓝牙控制模式，0 表示自动控制模式
int controlMode = 0;  // 默认是自动控制模式

// 初始化蓝牙设备
BlinkerButton Button1("btn-humidifier");
BlinkerButton Button2("btn-fan");
BlinkerButton Button3("btn-light");
BlinkerButton Button4("btn-dryservo");
BlinkerButton Button5("btn-pump");

void getHT() {
    ht.h = dht.readHumidity();
    ht.t = dht.readTemperature();
    if (isnan(ht.h) || isnan(ht.t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
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

// void getLDRVal() {
//   LDRVal.L = analogRead(LDRLPIN);
//   LDRVal.M = analogRead(LDRMPIN);
//   LDRVal.R = analogRead(LDRRPIN);
//   Serial.print(LDRVal.L);
//   Serial.print(" ");
//   Serial.print(LDRVal.M);
//   Serial.print(" ");
//   Serial.println(LDRVal.R);
// }

void getWaterLevel() {
  waterLevel = sr04.Distance();
  // Serial.print("Distance: ");
  // Serial.println(waterLevel);
}

void addWater() {
  if (waterLevel >= waterLevelMin) {
    digitalWrite(PumpPIN, HIGH);
  }
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
  addWater();
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

void toggleDry() {
  if(DryServoPos == 90) {
    DryServo.write(0);
    DryServoPos = 0;
    Serial.println("Dry ON");
  }
  else if(DryServoPos == 0) {
    DryServo.write(90);
    DryServoPos = 90;
    Serial.println("Dry OFF");
  }
}

// 自动控制设备（湿度低于阈值开启加湿器，高于阈值开启风扇和干燥机）
void autoControl() {
    if (ht.h < HUMIDITY_THRESHOLD_LOW) {
        addWater();
        digitalWrite(HumidifierPIN, HIGH);  // 启动加湿器
        Serial.println("Auto: Humidifier ON");
    } else {
        digitalWrite(HumidifierPIN, LOW);   // 关闭加湿器
        Serial.println("Auto: Humidifier OFF");
    }

    if (ht.h > HUMIDITY_THRESHOLD_HIGH) {
        digitalWrite(FanPIN, HIGH);   // 启动风扇
        DryServo.write(0);           // 打开干燥机盒
        Serial.println("Auto: Fan ON, DryServo OPEN");
    } else {
        digitalWrite(FanPIN, LOW);    // 关闭风扇
        DryServo.write(90);            // 关闭干燥机盒
        Serial.println("Auto: Fan OFF, DryServo CLOSE");
    }
}



// 按键触发回调函数，控制加湿器
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    toggleHumidifier();
    controlMode = 1;
    if(digitalRead(HumidifierPIN) == HIGH) {
      Serial.println("Humidifier ON");
    } else {
      Serial.println("Humidifier OFF");
    }
}

// 按键触发回调函数，控制风扇
void button2_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    toggleFan();
    controlMode = 1;
    if(digitalRead(FanPIN) == HIGH) {
      Serial.println("Fan ON");
    } else {
      Serial.println("Fan OFF");
    }
}

// 按键触发回调函数，控制灯
void button3_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    toggleLight();
    if(digitalRead(LightPIN) == HIGH) {
      Serial.println("Light ON");
    } else {
      Serial.println("Light OFF");
    }
}

// 按键触发回调函数，控制干燥机
void button4_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    toggleDry();
    controlMode = 1;
    if (DryServoPos == 90){
      Serial.println("Dry OFF");
    } else {
      Serial.println("Dry ON");
    }
}

// 按键触发回调函数，控制水泵
void button5_callback(const String & state) {
  BLINKER_LOG("get button state: ", state);
  togglePump();
  controlMode = 1;
  if (digitalRead(PumpPIN) == HIGH) {
    Serial.println("Pump ON");
  } else {
    Serial.println("Pump OFF");
  }
}

void twcontrol() {
  if (digitalRead(twFanPIN) == HIGH) {
    digitalWrite(FanPIN, HIGH);
    controlMode = 1;
  } else {
    digitalWrite(FanPIN, LOW);
  }

  if (digitalRead(twLightPIN) == HIGH) {
    digitalWrite(LightPIN, HIGH);
    controlMode = 1;
  } else {
    digitalWrite(LightPIN, LOW);
  }

  if (digitalRead(twHumidifierPIN) == HIGH) {
    digitalWrite(HumidifierPIN, HIGH);
    controlMode = 1;
  } else {
    digitalWrite(HumidifierPIN, LOW);
  }
}

void setup() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  // Arm1Servo.setPeriodHertz(50);
  // Arm2Servo.setPeriodHertz(50);
  DryServo.setPeriodHertz(50);
  // Arm1Servo.attach(Arm1PIN, 500, 2500);
  // Arm2Servo.attach(Arm2PIN, 500, 2500);
  DryServo.attach(DryServoPIN, 500, 2500);
  tft.initR(INITR_BLACKTAB);
  Serial.begin(115200);
  dht.begin();
  tft.setRotation(4);
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(2);
  // getLDRVal();
  DryServo.write(90);
  DryServoPos = 90;
  // pinMode(LDRLPIN, INPUT);  
  // pinMode(LDRMPIN, INPUT);
  // pinMode(LDRRPIN, INPUT);
  pinMode(FanPIN, OUTPUT);
  pinMode(LightPIN, OUTPUT);
  pinMode(PumpPIN, OUTPUT);
  pinMode(HumidifierPIN, OUTPUT);
  pinMode(twFanPIN, INPUT);
  pinMode(twLightPIN, INPUT);
  pinMode(twHumidifierPIN, INPUT);
  digitalWrite(FanPIN, LOW);
  digitalWrite(LightPIN, LOW);
  digitalWrite(PumpPIN, LOW); 
  digitalWrite(HumidifierPIN, LOW);

  #if defined(BLINKER_PRINT)
    BLINKER_DEBUG.stream(BLINKER_PRINT);
  #endif



  //绑定按钮
  Blinker.begin();
  Button1.attach(button1_callback);  // 控制加湿器
  Button2.attach(button2_callback);  // 控制风扇
  Button3.attach(button3_callback);  // 控制灯
  Button4.attach(button4_callback);  // 控制干燥机
  Button5.attach(button5_callback);
  
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
  digitalWrite(FanPIN, LOW);
  Serial.println(digitalRead(twFanPIN));
  delay(4000);
  digitalWrite(FanPIN, LOW);
}


void loop() {
  Blinker.run();
  if (millis() - lastHTUpdate >= HT_INTERVAL) {
    // 获取湿度和温度
    getHT();
    displayHT();

    // 如果处于自动控制模式，则根据湿度阈值自动控制设备
    if (controlMode == 0) {
      autoControl();
    }
    lastHTUpdate = millis();
  }

  getWaterLevel();
  if (waterLevel <= waterLevelMax) {
    digitalWrite(PumpPIN, LOW);
  }

  twcontrol();
  
}

    // getLDRVal();
    // if(LDRVal.M < LDRVal.L || LDRVal.M < LDRVal.R) {
    //   if (analogRead(LDRMPIN) < analogRead(LDRLPIN)) {
    //     for (pos = currentPos; pos <= 45; ++pos){
    //       Arm1Servo.write(pos);
    //       Arm2Servo.write(pos);
          
    //     }
    //   }
    // }