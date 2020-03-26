// Libraries.
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
//#include <DHT.h> // Library van temperatuur en luchtvochtigheid sensor.
#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

// Definities.
#define SCREEN_WIDTH 128 // OLED display width, in pixels.
#define SCREEN_HEIGHT 64 // OLED display height, in pixels.
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin) // Onze OLED's hebben geen reset.
#define BLYNK_PRINT Serial

// Setup.
BH1750 BH(0x23);
Adafruit_BMP280 bmp; // I2C.
//DHT dht(5, DHT11); // Declareer dht model en de data pin.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HTTPClient http;
Servo myservo;  // create servo object to control a servo

// Variabelen.
const char* ssid = "WiFi-2.4-FE3E";
const char* password = "D9A4F32A84";
char auth[] = "FIL5zw-8c7Z8dcdnzt7ZrzakGHg4LlYu";
int pos = 0;    // variable to store the servo position
int servoPin = 18;
int ven = 0, sha = 0;

// put your setup code here, to run once:
void setup()
{
   Serial.begin(9600); // Seriele communicatie opzetten.
   pinMode(15, OUTPUT);
   
   // WiFi instellen.
   delay(4000);
   WiFi.begin(ssid, password);
   while(WiFi.status() != WL_CONNECTED)
   {
      delay(1000);
      Serial.println(F("Connecting to WiFi.."));
   }
   Serial.println(F("Connected to the WiFi network"));
   
   // Blynk instellen.
   Blynk.begin(auth, ssid, password, "https;//server.wyns.it", 9443);
   
   // OLED initialiseren.
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // Adres van OLED is 0x3C. 
   {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever.
   }
   
   // BH1750 initialiseren.
   if(BH.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
   {
      Serial.println(F("BH1750 Advanced begin"));
   }else
   {
      Serial.println(F("Error initialising BH1750"));
   }
   
   // BMP280 initialiseren.
   if(!bmp.begin())
   {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
      while(1);
   }
   bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                   Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                   Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                   Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                   Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
   
   // DHT11 initialiseren.
   //dht.begin(); // Klaar zetten sensor pin voor lezen en pull timing.
   
   // Servo initialiseren.
   myservo.setPeriodHertz(50);    // standard 50 hz servo
   myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
   
   display.display(); // De OLED library initialiseerd met het Adafruit logo.
   delay(2000); // Pause for 2 seconds.
   display.clearDisplay();
}

// put your main code here, to run repeatedly:
void loop()
{
   Blynk.run();
   if((WiFi.status() == WL_CONNECTED)) // Check the current connection status.
   {
      display.clearDisplay();
      WiSy();
      http.begin("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=H2OT5QSJWN5Q57PS"); // Specify the brussels time URL.
      int httpCode = http.GET(); // Make the request.
      Brutime(httpCode);
      http.end(); // Free the resources.
   }else
   {
      display.clearDisplay();
      display.setTextSize(1); // Normal 1:1 pixel scale.
      display.setCursor(0, 0);
      display.println(F("No WiFi"));
   }
   dataP(); // Metingen maken en displayen.
   delay(4000);
}

// Brussel tijd display.
void Brutime(int httpConf)
{
   if(httpConf > 0) // Check for the returning code.
   {
      String payload = http.getString();
      String Btijd = payload.substring(40, 44); // Brussel tijd uit payload halen.
      Btijd.concat(' '); // Spatie.
      Btijd.concat(payload.substring(48, 50)); // AM PM uit string halen en samenvoegen.
      Btijd.concat('\0'); // NULL karakter aan strings toevoegen.
      
      // Display tijd via Serial.
      Serial.print("Brussel time: ");
      Serial.println(Btijd);
      
      // Display tijd via OLED.
      display.setCursor(80, 0);
      display.println(Btijd);
      display.display();
   }else
   {
      Serial.println("No Clock.");
      display.println(F("Problem with HTTP request."));
   }
}

// Metingen uitvoeren en resultaten displayen en doorsturen.
void dataP()
{
   float lux = BH.readLightLevel();
   float temp = bmp.readTemperature();
   //float hum = dht.readHumidity();
   
   // Serial data tonen.
   Serial.print("Licht: ");
   Serial.print(lux);
   Serial.println(" lux");
   Serial.print(F("Temperature = "));
   Serial.print(temp);
   Serial.println(" *C");
   
   // OLED data tonen.
   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
   display.println(F("Lichtlevel"));
   display.setTextColor(SSD1306_WHITE);
   display.print(lux);
   display.println(" lux");
   display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
   display.println(F("Temperatuur"));
   display.setTextColor(SSD1306_WHITE);
   display.print(temp);
   display.println(" *C");
   //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
   //display.println(F("Luchtvochtigheid"));
   //display.setTextColor(SSD1306_WHITE);
   //display.print(hum);
   //display.print(" %");
   display.display();
   
   // Blynk data tonen.
   Blynk.virtualWrite(V5, lux);
   Blynk.virtualWrite(V2, temp);
   
   // Controleren waarde voor schaduw en ventilator.
   if(sha == 0)
   {
      if((int)lux > 5000)
      {
         myservo.write(180);  // Tell servo to go to position in variable 'pos'.
      }else
      {
         myservo.write(0);
      }
   }
   if(ven == 0)
   {
      if((int)temp > 23) // || (int)hum > 40
      {
         digitalWrite(15, HIGH);
      }else
      {
         digitalWrite(15, LOW);
      }
   }
}

// Blynk Arduino <-- App.
BLYNK_WRITE(V1) // Schaduw.
{
   int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
   if(pinValue)
   {
      Serial.println(F("Shade is active."));
      sha = 1;
      myservo.write(180);
   }else
   {
      Serial.println(F("Shade is deactivated."));
      sha = 0;
      myservo.write(0);
   }
   // You can also use:
   // String i = param.asStr();
   // double d = param.asDouble();
}
BLYNK_WRITE(V3) // Ventilator.
{
   int pinValue2 = param.asInt(); // assigning incoming value from pin V3 to a variable
   if(pinValue2)
   {
      Serial.println(F("Ventilator is on."));
      ven = 1;
      digitalWrite(5, HIGH);
   }else
   {
      Serial.println(F("Ventilator is off."));
      ven = 0;
      digitalWrite(5, LOW);
   }
}

// Teken wifi symbool.
void WiSy()
{
   int16_t i, j = 5;
   
   display.setCursor(0, 0);
   display.setTextSize(1); // Normal 1:1 pixel scale.
   display.setTextColor(SSD1306_WHITE);
   display.println(F("W"));
   display.drawPixel(8, 6, SSD1306_WHITE);
   for(i = 10; i <= 14; i += 2)
   {
      display.drawLine(i, 6, i, j, SSD1306_WHITE);
      j -= 2;
   }
   display.display();
}
