// hardware consistence
//  OLED1306 - Arduino Pro Mini - FTDI Comm - (USB Serial) - MacBook Pro 

// hardware requirement
// 1. Arduino Pro Mini
// 2. DHT22
// 3. OLED1306 I2C type

// software library requirement
//  SPI.h (for I2C connection)
//  Wire.h
//  DHT.h
//  Adafruit_GFX.h
//  Adafruit_SSD1306.h

// hardware wiring to DHT22...
//  1 VCC - VCC5V
//  2 Output - Sensor Data 
//  4 GND - GND

// hardware wiring of OLED1306...
//  VCC - VCC5V
//  GND - GND
//  SCL - (BLUE)  - A7
//  SDA - (WHITE) - A6

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define OLED_RESET 4
#define DHT22_to_Arduino_PIN 4
#define DHTTYPE DHT22

Adafruit_SSD1306 display(OLED_RESET);
DHT dht(DHT22_to_Arduino_PIN, DHTTYPE);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {                
  Wire.begin(0, 2); // SDA = IO00, SCL = IO02

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  display.display();
  delay(2000);

  display.clearDisplay();
}

void loop() {
  disp8factor();

  display.setCursor(0,0);
  display.println("                    |");
  display.display();
  delay(1000);
  
  display.setCursor(0,0);
  display.println("                    /");
  display.display();
  delay(1000);
  
  display.setCursor(0,0);
  display.println("                    -");
  display.display();
  delay(1000);
  
  display.setCursor(0,0);
  display.println("                    *");
  display.display();
  delay(1000);

  display.clearDisplay();
}

void disp8factor(void) {
  
  float tempC = dht.readTemperature();
  float humid = dht.readHumidity();
  float DewPt = Dew_Point(tempC, humid);
  float moist = Moisture_Pressure(tempC, humid);
  float mixrt = Mixing_Ratio(tempC, humid);
  float AbsHu = Saturated_Water_Vapor_Pressure(tempC, humid);
  float SpEnt = Specific_Enthalpy(tempC, humid);

  // 0. display Title
  display.setCursor(0, 0);
  display.println("Psychrometric data");

  // 1. display Dry-bulb Temparature
  display.print("1.Temp:"); display.print(tempC, 1); display.println("C");
  
  // 2. display Relative Humidity
  display.print("2.Humi:"); display.print(humid, 1); display.println("%");

  // 3. calculate Dewpoint(CDP) temparature from Temp and Humi
  display.print("3.DewP:"); display.print(DewPt, 1); display.println("CDP");

  // 4. caliculate Moisture Pressure
  display.print("4.Moist:"); display.print(moist, 1); display.println("kPa");

  // 5. caliculate Mixing Ratio
  display.print("5.MixRt:"); display.print(mixrt, 1); display.println("g/m3");

  // 6. Caliculate Specific Enthalpy
  display.print("6.SpEnt:"); display.print(SpEnt, 1); display.println("kJ/kgDA");
}

double Dew_Point(double temp_celsius, double humidity_percent)
{
  double RATIO = 373.15 / (273.15 + temp_celsius);
  double RHS = -7.90298 * (RATIO - 1);

  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  double VP = pow(10, RHS - 3) * humidity_percent;
  double T = log(VP/0.61078);
  
  return (241.88 * T) / (17.558 - T);
}

double Saturated_Water_Vapor_Pressure(double theta_celcius, double humidity_percent)
{
  double saturated_water_vapor_pressure_fs;
  double index;

  index = 7.5 * theta_celcius / (theta_celcius + 237.3);
  saturated_water_vapor_pressure_fs = 6.1078 * pow(10, index);

  return saturated_water_vapor_pressure_fs;
}

double Moisture_Pressure(double Saturated_Water_Vapor_Pressure_fs, double humidity_percent)
{
  double moisture_pressure_f;

  moisture_pressure_f = (Saturated_Water_Vapor_Pressure_fs * 101.3245 / 760) * humidity_percent / 100;

  return moisture_pressure_f;
}

double Mixing_Ratio(double theta_celcius, double humidity_percent)
{
  double volumetric_humidity_phai_w;
  double Tab;
  double t1, t2, t3, t4, t5;
  double mixing_ratio_x;

  Tab = 273.15 + theta_celcius;
  t1 = -5800.2206 / Tab;
  t2 = 0.048640239 * Tab;
  t3 = 0.41764768 * pow(10, -4) * Tab * Tab;
  t4 = 0.14452093 * pow(10, -7) * Tab * Tab * Tab;
  t5 = 6.5459673 * log(Tab);

  volumetric_humidity_phai_w = exp(t1 + 1.3914993 - t2 + t3 - t4 + t5) / 1000;
  mixing_ratio_x = volumetric_humidity_phai_w * humidity_percent / 100;

  return mixing_ratio_x;
}

double Specific_Enthalpy(double theta_celcius, double humidity_percent)
{
  double moisture_pressure_f, absolute_humidity_x, specific_enthalpy_h;

  moisture_pressure_f = Saturated_Water_Vapor_Pressure(theta_celcius, humidity_percent);
  absolute_humidity_x = 0.622 * moisture_pressure_f / (760 - moisture_pressure_f);
  specific_enthalpy_h = (0.24 * theta_celcius + (0.431 * theta_celcius + 597.3) * absolute_humidity_x) * 4.18605;

  return specific_enthalpy_h;
}
