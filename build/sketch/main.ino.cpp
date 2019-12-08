#include <Arduino.h>
#line 1 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <Wire.h>

#define MeasureBreak 1000
#define NumberOfMeasurements 10

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
int relay_pump = 8;
SimpleDHT11 dht11;
int sum_of_moisture = 0; // initial value of the sum of the moisture measurements
int mesurement = 0;      // initial value of the measurement item
unsigned long LastTime = 0;

const int AirValue = 520;   //you need to replace this value with Value_1
const int WaterValue = 260; //you need to replace this value with Value_2

int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;
#line 27 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void setup();
#line 35 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void loop();
#line 27 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void setup()
{
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  lcd.begin();        // initialize the LCD
  lcd.backlight();
  pinMode(relay_pump, OUTPUT);
  digitalWrite(relay_pump, HIGH);
}
void loop()
{
  byte temperature = 0;
  byte humidity = 0;
  unsigned long CurrentTime = millis(); // time from start of the sketch

  if (dht11.read(pinDHT11, &temperature, &humidity, NULL))
  {
    Serial.print("Read DHT11 failed.");
    return;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Temperatura:");
  lcd.setCursor(12, 0);
  lcd.println((int)temperature);
  lcd.setCursor(14, 0);
  lcd.println((char)223);
  lcd.setCursor(15, 0);
  lcd.println("C");
  lcd.setCursor(0, 1);
  lcd.println("Wilgotnosc: ");
  lcd.setCursor(12, 1);
  lcd.println((int)humidity);
  lcd.setCursor(14, 1);
  lcd.println(" ");
  lcd.setCursor(15, 1);
  lcd.println((char)37);
  Serial.println(CurrentTime);
  // DHT11 sampling rate is 1HZ.
  delay(2000);

  if (CurrentTime - LastTime > MeasureBreak) // if the time of the mesurements is longer than MeasurementBreak value:
  {
    LastTime = CurrentTime;
    mesurement += 1;                   // add the current mesurement to the total measurement no
    sum_of_moisture += analogRead(A0); // add the current moisture value to the sum of the moisture measurement
    if (mesurement >= NumberOfMeasurements)
    {
      int mean_moisture = ceil(sum_of_moisture / mesurement);
      lcd.clear();
      Serial.println(CurrentTime);
      //soilMoistureValue = analogRead(A0); //put Sensor insert into soil
      if (mean_moisture > WaterValue && mean_moisture < (WaterValue + intervals))
      {
        Serial.println("Very Wet");
        Serial.println(mean_moisture);
        lcd.setCursor(0, 5);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 5);
        lcd.print("bardzo mokra");
        digitalWrite(relay_pump, HIGH);
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.println(mean_moisture);
        delay(5000);
      }
      else if (mean_moisture > (WaterValue + intervals) && mean_moisture < (AirValue - intervals))
      {
        Serial.println("Wet");
        Serial.println(mean_moisture);
        lcd.setCursor(0, 5);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 5);
        lcd.print("Mokra");
        digitalWrite(relay_pump, LOW);
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.println(mean_moisture);
        delay(5000);
      }
      else if (mean_moisture < AirValue && mean_moisture > (AirValue - intervals))
      {
        Serial.println("Dry");
        Serial.println(mean_moisture);
        lcd.setCursor(0, 5);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 5);
        lcd.print("Sucha");
        digitalWrite(relay_pump, LOW);
        delay(3000);
        lcd.setCursor(0, 1);
        lcd.println(mean_moisture);
        delay(5000);
      }
      else
      {
        Serial.println("Blad pomiaru");
        lcd.println("Blad pomiaru");
        delay(5000);
      }
    }
  }
  mesurement = sum_of_moisture = 0;
}
