#include <Arduino.h>
#line 1 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
#include <LiquidCrystal_I2C.h>

#include <SimpleDHT.h>
#include <Wire.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;
int czas = 0;

const int AirValue = 520;   //you need to replace this value with Value_1
const int WaterValue = 260; //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;
#line 21 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void setup();
#line 27 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void loop();
#line 21 "c:\\Users\\tkogut\\Documents\\Arduino\\BIURKO_1\\main.ino"
void setup()
{
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  lcd.begin();        // initialize the LCD
  lcd.backlight();
}
void loop()
{
  byte temperature = 0;
  byte humidity = 0;
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
  Serial.println(int(czas));
  // DHT11 sampling rate is 1HZ.
  delay(2000);
  czas = czas + 1;

  if (czas == 5)
  {
    lcd.clear();
    Serial.println(int(czas));
    soilMoistureValue = analogRead(A0); //put Sensor insert into soil
    if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
    {
      Serial.println("Very Wet");
      Serial.println(soilMoistureValue);
      lcd.setCursor(0, 5);
      lcd.print("Wilgotnosc gleby");
      lcd.setCursor(1, 5);
      lcd.print("bardzo mokra");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.println(soilMoistureValue);
      delay(5000);
    }
    else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
    {
      Serial.println("Wet");
      Serial.println(soilMoistureValue);
      lcd.setCursor(0, 5);
      lcd.print("Wilgotnosc gleby");
      lcd.setCursor(1, 5);
      lcd.print("Mokra");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.println(soilMoistureValue);
      delay(5000);
    }
    else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
    {
      Serial.println("Dry");
      Serial.println(soilMoistureValue);
      lcd.setCursor(0, 5);
      lcd.print("Wilgotnosc gleby");
      lcd.setCursor(1, 5);
      lcd.print("Sucha");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.println(soilMoistureValue);
      delay(5000);
    }
    else
    {
      Serial.println("Blad pomiaru");
      lcd.println("Blad pomiaru");
      delay(5000);
    }
    czas = 0;
  }
}
