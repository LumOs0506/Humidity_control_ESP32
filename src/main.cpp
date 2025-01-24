#include<Adafruit_GFX.h>
#include<Adafruit_ST7735.h>
#include<SPI.h>
#include<DHT.h>
#include<ESP32Servo.h>


#define TFT_CS 14
#define TFT_DC 27
#define TFT_RST 5
#define DHTPIN 25
#define DHTTYPE DHT11
#define LDRLPIN 12
#define LDRMPIN 14
#define LDRRPIN 27
#define ServoPIN 26

//SCL-18, SDA-23

typedef struct {
  float h;
  float t;
} HT;

HT ht;
int HTcount;
int LDRLVal;
int LDRMVal;
int LDRRVal;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
ESP32PWM pwm;

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

void setup() {
  // put your setup code here, to run once:
  tft.initR(INITR_BLACKTAB);
  Serial.begin(9600);
  dht.begin();
  tft.setRotation(4);
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(2);
  pinMode(12, INPUT);
  pwm.attachPin(ServoPIN, 500, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (HTcount >= 200000){
    getHT();
    displayHT();
    HTcount = 0;

  }
  ++HTcount;
}