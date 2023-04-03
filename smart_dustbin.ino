#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);


// Giving wifi name and password to esp32
WiFiClientSecure client;
const char* ssid = "NILESH";
const char* password = "11112222";

// Telegram Bot
#define BOTtoken "5657684249:AAEA7P5iyZWVrEYRIhK594OkbZH0p8J7HrE"
#define CHAT_ID "1457594471"
UniversalTelegramBot bot("5657684249:AAEA7P5iyZWVrEYRIhK594OkbZH0p8J7HrE", client);

// Servo
#define servo_pin 13  // Defining servo motor pin
Servo myservo;        // create servo object to control a servo
int pos = 0;


// Defining pins for ultrasonic sensor
const int trig_pin = 33;
const int echo_pin = 32;
int di;  // Distance given by ultrasonic pin
long duration;


#define ir_pin 15  // Setting ir motor pin

#define height 20  // Height of the dustbin

// Defining flags
int f1;
int f2;
int f3;


// Calculating distance using ultrasonic sensor
int distance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(1);

  digitalWrite(trig, HIGH);
  delayMicroseconds(1);

  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  return duration * 0.034 / 2;
}


void setup() {

  Serial.begin(115200);

  pinMode(13, OUTPUT);  // Setting servo motor pin as output
  digitalWrite(13, HIGH);

  pinMode(trig_pin, OUTPUT);  // Setting ultrasonic sensor pins
  pinMode(echo_pin, INPUT);

  pinMode(ir_pin, INPUT);  // Pin for IR sensor

  // Initialize the LCD connected
  lcd.begin();

  // Turn on the backlight on LCD.
  lcd.backlight();
  myservo.attach(servo_pin);  // attaches the servo on servo_pin to the servo object

  //Initialisation of bot
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  // Initialisation of wifi
  lcd.print("Connecting WIFI: ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  lcd.clear();

  bot.sendMessage("1457594471", "Smart Dustbin Started", "");
  bot.sendMessage("1457594471", "Dustbin is empty", "");

  //Initializing flags
  f1 = 0;
  f2 = 0;
  f3 = 0;
}


void loop() {

  if (digitalRead(ir_pin) == LOW) {
    lcd.clear();
    lcd.print("READY TO PICKUP");

    for (pos = 0; pos <= 180; pos += 1) {
      myservo.write(pos);
      if (pos == 180)
        delay(2000);
    }

    for (pos = 180; pos >= 70; pos -= 1)
      myservo.write(pos);

    lcd.clear();
  }

  // Calculting di from ultrasonic sensor
  di = distance(trig_pin, echo_pin);
  if (di != 0) {
    int a = 0;
    if (di >= height)
      a = 0;
    else
      a = 100 - ((100 * di) / height);
    if (a >= 50 && f1 == 0) {
      bot.sendMessage("1457594471", "Dustbin is Half", "");
      f1 = 1;
    }


    if (a >= 90 && f2 == 0) {
      bot.sendMessage("1457594471", "Dustbin is 90% Full", "");
      f2 = 1;
    }

    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("USE ME");




    lcd.setCursor(3, 1);
    if (a > 70)
      a = 100;
    lcd.print(a);
    lcd.setCursor(6, 1);
    lcd.print("% FULL");
    delay(2000);


    if (a > 90 && di > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);

      if (f3 == 0) {
        bot.sendMessage("1457594471", "Dustbin is Full", "");
        f3 = 1;
      }
      lcd.print("DUSTBIN IS FULL");
      delay(2000);
    }
  }
}
