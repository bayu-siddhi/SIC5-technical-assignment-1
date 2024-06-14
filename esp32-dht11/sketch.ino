//Libraries
#include <DHT.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 19
#define DHTTYPE DHT11

// Inisialisasi DHT22 dan HTTP
DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

// Server (Ganti ke IP localhost nya)
const char* postServer = "http://192.168.0.1:5000/sensor/data";
const char* ntpServer = "pool.ntp.org";

// Wi-Fi (Ganti ssid dan password)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Variabel lain
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

  // Menunggu ESP32 terkoneksi ke WiFI
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
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    currentMillis = 0;
  }
  currentMillis = time(&now);
  if (currentMillis < 1000000000 ) {
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
    Serial.println("Temperature: " + String(temperature) + "°C");
    Serial.println("Humidity: " + String(humidity) + "%");
    Serial.println("Millis: " + String(currentMillis) + " milliseconds");
  }
}

void postHTTP() {
  http.begin(postServer);
  http.addHeader("Content-Type", "Content-Type: application/json"); 
  jsonData = "{ temperature: " + String(temperature) + ", humidity: " + String(humidity) + ", millis: " + String(currentMillis) + " }";
  
  httpResponseCode = http.POST(jsonData);

  if(httpResponseCode > 0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
    Serial.println();
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    Serial.println();
    http.end();
  }
}
