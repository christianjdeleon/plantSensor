 
#include <DHT.h>  
#include <WiFi.h> 
#include <WiFiClientSecure.h>  
#include <ESP_Google_Sheet_Client.h> 
#include "time.h"



#define ENABLE_SMTP
#define ENABLE_DEBUG
#include <ReadyMail.h>  


#define SOIL_PIN 36
#define DHT_PIN 23 
#define DHT_TYPE DHT11  
#define LIGHT_PIN 39   

const char* ssid = "ashas02"; 
const char*  password = "sharon4angels";  
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCvBMrnDD3uaBTV\nQuXzKNNetoCp+aSoNvcRGIORzFK/6eAaOtjJ59GRvVG3nWtK/63bDV3l66iy5tv3\nrBQ6rexulU/Qae6AGxCoN5G+qFYSqLZdwsv4aRnaaMgk6J3yW656W+i1iKCqnY4R\n//GI3MCk/gTZ3DIqUIGAYqzHM9GDc9f4218TrK0Nr/xRqlw+Q+kytwsQYjj1Y/zD\nxctXphWBoPJxvY3eiewPpKxQnls5JMu9Vg8bSiavT2zuSkM9oVoq5n1LZGL+bpBb\nt3pGsmPTTZG+DUif3YRKznXfCyLP6B3ppMMBqpt6svORk+o3hBpiFi6m8GQskcue\ncrihp+4XAgMBAAECggEADFgWhi9F2vUlAFmkkFRBJuf7sSwLYgCRHSKRJ7e1NRX7\nbr4sdikkjdaVgebXdMW4bxUamfyCEdmDNY0XLCsCMavUTPXREuCrJGzZmbc+bY3Q\nDFLl7Ke1DBjh15Xp4KMjbjvh8Okps9RC+0qwWsp/HTlRVvBXQBwc1ipKLLlJ6K8l\nYzhEqmhnh8FPYAP7yxW2rnrF6ldIfX6QxAgaIUQYWgSrMy22rHx2caanxEiuYdEv\nK1l8Es6IYFET1GGBOeMcAstHzcJ1QEIvIBBigurthN30Z+b5FYEbpibj02V9gq/q\ne92RTRAEWggb/E4U2GggIBGhsW+jnljvg60H4RqrKQKBgQDjI2Tbxvdvi9a5cy3E\ncPrOg/Rm4XqSJVizOdLC+6CCqsrgIma1j1PINGacVEwJ9eKBHvvb7Mc2zr+ifamY\nUcR3bo3m1js0/XolW/4Qv2+YVGjXcQ3gMpj4XAnIslNJbklWWFctdS7B0FK7Bv30\niZSXUfTon9eUNFVxQuVkC75MCwKBgQDFQf8Ot6F4y6DIEPn0UzrKlnVzvCh/D06j\nK9xHljeWYKDyZi4u76+sXrLJip0C1LN1b+xjbD+sUWOkZzZfOkRY6RkalNzOOY0D\nu3a8XXDQDGiPz65mweH+J9Hzg5jW8/47fzVV5Z1ju72dVZ4AGfP8rIcB4lKOP7kt\nE1uClUqhpQKBgF0QT27NIuyNOzlYu9PVBq4+2pCcqf/fVWx5sKU7PBTNrboP3k1d\noFJTRgefclhSaq/E5iTB6w3FwDbvjeFe/0SAA5wlArU/diCAhKSQC0U6sIo1RB4s\nHQkFjOyyQ/W+PkYmg/GYJcQ7euoufzcGd99YMSRu+YE7wBZb9mj7dsjpAoGAcC/V\nuPAVRNdC7SZCkoOXh0rFJ/qWaZUyGzlBwfEcUklIoDQfBhDJr9X4BwnvvIFuQ1dR\ncUZKfF2ZtxIAaW6BGrQP0/9gCWXP3pSKthzKoXi9KzVpsgWI23yYOs/YaEBCkmIJ\n2n2J4wQOlmHwR4x7KHSTIC4w17hEP++ndwAjfwUCgYA1GoYwyvpMWZsh/17KuyhH\nQ6n0iSJWECYR+DKYF5hQaJI9arhelZLUAmgGTWlt/vOSm+qFzqc3Q8cb6gbvTno1\nCG2s7XZPA/9I+mJ4hbyeesMtIfJ+mdqtauSuk1G70N9lFelXmdKA7nIQdY35OXC7\nQ8Xa6zQSTOxnPbAozTOjMg==\n-----END PRIVATE KEY-----\n"; 
const char* ntpServer = "time.google.com";  
const char spreadsheetId[] = "1SJIrPp-Q1DS9ZHu0Fm_hcO_EmcPECpAHxKC-5dDKAtE";




#define APP_PASSWORD "xkix obqo vojk jmsl"
#define SENDER_EMAIL "cjdeleonelectronics@gmail.com"
#define SENDER_NAME "CJ"
#define RECIPIENT_EMAIL "anjelicadeleon10@gmail.com" 
#define RECIPIENT_NAME "Ateh"
#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define PROJECT_ID "plant-datalogging"  
#define CLIENT_EMAIL "christian@plant-datalogging.iam.gserviceaccount.com" 


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
  // put your setup code here, to run once: 
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
  // put your main code here, to run repeatedly:
  
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
    // msg.html.body("<html><body><h1>Hello, Mom! This is your plant data.</h1></body></html>"); 
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
