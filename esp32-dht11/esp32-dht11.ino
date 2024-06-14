// Libraries
#include <DHT.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Initialization of DHT22 and HTTP
#define DHTPIN 19
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

// Server (Change to your localhost IP)
const char* postServer = "http://192.168.0.105:5000/sensor/data";
const char* ntpServer = "pool.ntp.org";

// Wi-Fi (Change to your SSID and password)
const char* ssid = "KOS BU MARTA";
const char* password = "mulyosari19";

// Variables
float temperature = 0;
float humidity = 0;
unsigned long currentMillis = 0; 
String jsonData = "";
int httpResponseCode = 0;

void setup() {
  Serial.begin(115200);
  setupWiFi();
	dht.begin();
  configTime(0, 0, ntpServer);
}

void loop() {
  delay(2000);
  getDHTData();
  postHTTP();
}

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void getTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    currentMillis = 0;
  }
  currentMillis = time(NULL);
  if(currentMillis < 1000000000 ) {
    currentMillis = 0;
  }
}

void getDHTData() {
  getTime();
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Millis: ");
    Serial.print(currentMillis);
    Serial.println(" milliseconds");
  }
}

void postHTTP() {
  http.begin(postServer);
  http.addHeader("Content-Type", "application/json");
  jsonData = "{\"temperature\":\"" + String(temperature) + "\",\"humidity\":\"" + String(humidity) + "\",\"millis\":\"" + String(currentMillis) + "\"}";

  httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
