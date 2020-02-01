#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <Wire.h>

#define MeasureBreak 600000
#define NumberOfMeasurements 6

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
SimpleDHT11 dht11;
int pinDHT11 = 2;
int relay_pump = 8;
int sum_of_moisture = 0; // initial value of the sum of the moisture measurements
int mesurement = 0;      // initial value of the measurement item
int mean_moisture = 0;
int w = 0;
String wilgotnosc[] = {"Sucha", "Mokra", "Bardzo mokra"};

unsigned long LastTime = 0;

const int AirValue = 520;   //you need to replace this value with Value_1
const int WaterValue = 260; //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue) / 3;

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
  Serial.println("-----------------------");
  Serial.println(CurrentTime);
  Serial.println(LastTime);
  Serial.println(mesurement);
  Serial.println(ceil(sum_of_moisture / mesurement));
  // DHT11 sampling rate is 1HZ.
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wilgotnosc gleby");
  lcd.setCursor(1, 1);
  lcd.print(wilgotnosc[w]);
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

      if (mean_moisture > WaterValue && mean_moisture < (WaterValue + intervals))
      {
        lcd.clear();
        w = 2;
        Serial.println(wilgotnosc[w]);
        Serial.println(mean_moisture);
        lcd.setCursor(0, 0);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 1);
        lcd.print(wilgotnosc[w]);
        digitalWrite(relay_pump, HIGH);
        delay(1000);
        lcd.setCursor(10, 1);
        //lcd.println(mean_moisture);
        delay(5000);
      }
      else if (mean_moisture > (WaterValue + intervals) && mean_moisture < (AirValue - intervals))
      {
        lcd.clear();
        w = 1;
        Serial.println(wilgotnosc[w]);
        Serial.println(mean_moisture);
        lcd.setCursor(0, 0);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 1);
        lcd.print(wilgotnosc[w]);
        digitalWrite(relay_pump, LOW);
        delay(10000);
        lcd.setCursor(10, 1);
        //lcd.println(mean_moisture);
      }
      else if (mean_moisture < AirValue && mean_moisture > (AirValue - intervals))
      {
        lcd.clear();
        w = 0;
        Serial.println(wilgotnosc[w]);
        Serial.println(mean_moisture);
        lcd.setCursor(0, 0);
        lcd.print("Wilgotnosc gleby");
        lcd.setCursor(1, 1);
        lcd.print(wilgotnosc[w]);
        digitalWrite(relay_pump, LOW);
        delay(20000);
        lcd.setCursor(10, 1);
        //lcd.println(mean_moisture);
      }
      else
      {
        lcd.clear();
        Serial.println("Blad pomiaru");
        lcd.println("Blad pomiaru");
        delay(5000);
      }
      digitalWrite(relay_pump, HIGH);
      mesurement = sum_of_moisture = 0;
    }
  }
}