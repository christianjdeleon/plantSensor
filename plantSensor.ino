 
#include <DHT.h>  
#include <WiFi.h> 
#include <WiFiClientSecure.h>  
#include <ESP_Google_Sheet_Client.h> 
#include "time.h" 
#include "config.h"


#define ENABLE_SMTP
#define ENABLE_DEBUG
#include <ReadyMail.h>  


#define SOIL_PIN 36
#define DHT_PIN 23 
#define DHT_TYPE DHT11  
#define LIGHT_PIN 39   

const char* ntpServer = "time.google.com";  

WiFiClientSecure ssl_client; 
SMTPClient smtp(ssl_client);  


DHT dht(DHT_PIN, DHT_TYPE);  

bool emailSent = false;  
int soilBaseline = 3000; 
int rawMoisture; 
int moisturePercent;  
int lightPercent;  
float temperature; 
float humidity; 
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;  
unsigned long epochTime;  
FirebaseJson valueRange;  
FirebaseJson response; 
struct tm timeinfo;

void tokenStatusCallback(TokenInfo info);



void setup() {

  Serial.begin(115200); 
  dht.begin();    


  WiFi.setAutoReconnect(true); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) delay(500);  

  configTime(-7 * 3600, 0, ntpServer);

  Serial.println("Waiting for time sync...");
  while (!getLocalTime(&timeinfo)) {
  delay(500);
  }
 
  GSheet.setTokenCallback(tokenStatusCallback);
  GSheet.setPrerefreshSeconds(10*60); 
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);  

 
} 



void loop() {
  
  rawMoisture = analogRead(SOIL_PIN);   
  moisturePercent = map(rawMoisture, 3400, 1150, 0, 100);
  int rawLight = analogRead(LIGHT_PIN); 
  lightPercent = map(rawLight, 0, 4095, 0, 100);

  Serial.print("Moisture: ");
  Serial.println(String(moisturePercent) + "%");  
  Serial.println(rawMoisture);
  Serial.print("Temp: "); 
  Serial.println(temperature); 
  Serial.print("Humidity: "); 
  Serial.println(humidity); 
  Serial.print("Light: "); 
  Serial.println(String(lightPercent) + "%"); 

  delay(5000);  

  if (!emailSent && rawMoisture > soilBaseline) { 
    sendPlantEmail();  
    emailSent = true; 
  }  

  while (!getLocalTime(&timeinfo)) {
  delay(500);
  }


  googleSheetLog();

} 

void sendPlantEmail() {   
  
  ssl_client.setInsecure();

  auto statusCallback = [](SMTPStatus status) {
    Serial.println(status.text);
  };

  smtp.connect(SMTP_SERVER, SMTP_PORT, statusCallback);

  if (smtp.isConnected()) {
    smtp.authenticate(SENDER_EMAIL, APP_PASSWORD, readymail_auth_password);

    SMTPMessage msg;
    msg.headers.add(rfc822_from, String(SENDER_NAME) + " <" + SENDER_EMAIL + ">");
    msg.headers.add(rfc822_to, String(RECIPIENT_NAME)  + " <" + RECIPIENT_EMAIL + ">");
    msg.headers.add(rfc822_subject, "Hello from ReadyMail");
    msg.text.body("Hello, Dad! This is your plant data. Moisture: " + String(moisturePercent) + "% Light: " + String(lightPercent) + "%");

    configTime(0, 0, "pool.ntp.org");
    while (time(nullptr) < 100000) delay(100);
    msg.timestamp = time(nullptr);

    smtp.send(msg);
  }  
} 

void googleSheetLog() { 
  bool ready = GSheet.ready(); 

  

  if (ready && millis() - lastTime > timerDelay) { 
    lastTime = millis(); 


    temperature = dht.readTemperature(true); 
    humidity = dht.readHumidity();  
    epochTime = getTime(); 
    Serial.println(epochTime);

    valueRange.add("majorDimension", "COLUMNS"); 
    valueRange.set("values/[0]/[0]", epochTime); 
    valueRange.set("values/[1]/[0]", temperature); 
    valueRange.set("values/[2]/[0]", humidity); 
    valueRange.set("values/[3]/[0]", moisturePercent);  

    bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!A2", &valueRange);
    if (success) { 
      response.toString(Serial, true); 
      valueRange.clear();
    } else { 
      Serial.println(GSheet.errorReason()); 
    } 
    Serial.println(); 
    Serial.println(ESP.getFreeHeap());
  } 

} 

unsigned long getTime() { 
  time_t now; 
  if (!getLocalTime(&timeinfo)) { 
    return(0);
  } 
  time(&now); 
  return now; 
} 


void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}