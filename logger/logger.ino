#include <WiFiClientSecure.h> 
#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 5
#define DHTTYPE    DHT22


DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;

#define ONE_WIRE_BUS 18
#define SENSER_BIT    9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid     = "hoge";    
const char* password = "huga";
const String host = "https://script.google.com/";
const int httpPort = 80;
const char* server = "script.google.com"; 

LiquidCrystal_I2C lcd(0x27,16,2); 

void setup() {
  Serial.begin(115200); 
  Serial.println("connecting");
   WiFiServer server(80);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("connecting");
  }
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.println(WiFi.localIP());
  digitalWrite(13, HIGH);
 
  
  server.begin();
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  sensors.setResolution(SENSER_BIT);
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("start logging!");
  lcd.setCursor(0,1);
  lcd.print("C and %");
}

void(* resetFunc) (void) = 0;

void sendDate(const char* name, float status) {
  client.setInsecure();
  // サイトにアクセス
  Serial.println("\nStarting connection to server...");
    if (!client.connect(server, 443)){
      Serial.println("Connection failed!");
      resetFunc();
    } else {
      Serial.println("Connected to server!\n");
      client.println("GET " + host + "?id=" + name +  "&value=" + status);
      client.stop();
      Serial.println("finish.");
    }
}


void loop() {
  delay(6000);
  lcd.clear(); 
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  digitalWrite(12, HIGH);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    digitalWrite(12, LOW);
  }
  else {
    Serial.print(F("sakama_Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    lcd.setCursor(0, 0);
    lcd.print(event.temperature);

    sendDate("Koi_Temperature", event.temperature);
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    digitalWrite(12, LOW);
  }
  else {
    Serial.print(F("sakama_Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));

    sendDate("Koi_Humidity", event.relative_humidity);
  }

  sensors.requestTemperatures();              // 温度取得要求
  if(isnan(sensors.getTempCByIndex(0))){
    Serial.println(F("Error reading water temperature!"));
    digitalWrite(12, LOW);
  } else {
    Serial.print(F("sakama_water_temperature: "));
    Serial.print(sensors.getTempCByIndex(0));
    Serial.println(F("°C"));
    lcd.setCursor(0, 1);
    lcd.print(sensors.getTempCByIndex(0));

    sendDate("Koi_water_temperature", sensors.getTempCByIndex(0));
  }
  digitalWrite(12, LOW);
}
