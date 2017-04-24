#include <ESP8266WiFi.h>
const int sensorIn = A0;  //The data from the ACS712 sensor is received at this port on the NodeMCU
int mVperAmp = 66;  // Multiplication factor to set sensitivity in mV/A of sensor. Use 100 for 20A Module and 185 for 5A Module
//Initialization of variables
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
double power = 0;
double powerWh = 0;
double ID = 1;
const char* ssid = "K4"; // Enter own router name.
const char* password = "Re@dy2go"; // Enter own router password
const char* host = "sparketix.000webhostapp.com";  //Link of your website
void setup() 
{     // Initializing serial port for debugging purposes
      Serial.begin(9600);
      // Connecting to WiFi network
      Serial.println();
      Serial.print("Connecting to SSID: ");
      Serial.println(ssid);
      // Sending WiFi SSID & Password
      WiFi.begin(ssid, password);
      delay(500);
      //On Successful connection to WiFi
      Serial.println("");
      Serial.println("WiFi successfully connected!");
      delay(500);
      // Printing the obtained IP address
      Serial.println("Obtained IP is: ");
      Serial.println(WiFi.localIP());
      delay(500); }
float getVPP()
{ //Conversion of received analog input i.e. counts to Ampere
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 1024;
  float start_time = millis();
  while((millis()-start_time) < 1000)
{ readValue = analogRead(sensorIn);
  if(readValue > maxValue)
{ maxValue = readValue; }
  if (readValue < minValue  )
{ minValue = readValue; } }
result = ((maxValue - minValue) * 5.0)/1024.0;
return result;  }
void loop() 
{       //Conversion of Ampere to power
        Voltage = getVPP();
        VRMS = (Voltage/2.0) *0.707;    
        AmpsRMS = (VRMS *1000)/mVperAmp;
        power=AmpsRMS*220;
        powerWh = (power / 3600) + powerWh; //conversion of power (watts) to watts/hr
        Serial.print("Connecting to ");
        Serial.println(host);
        delay(1000);
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(host, httpPort)) 
        { Serial.println("connection failed");
          return; }
        //Conversion of intput data i.e. float to string, appending it with the host website.
        String url = "http://sparketix.000webhostapp.com";
        url += "/index.php?";
        url += "&value=";
        url += ID;
        
        String url1 = "http://sparketix.000webhostapp.com";
        url1 += "/index.php?";
        url1 += "&value1=";
        url1 += AmpsRMS;

        String url2 = "http://sparketix.000webhostapp.com";
        url2 += "/index.php?";
        url2 += "&value2=";
        url2 += powerWh;
        
        Serial.print("Requesting URL: ");
        Serial.println(url);
        // This will send the request to the server
        client.print(String("GET ") + url + url1 + url2 + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Connection: close\r\n\r\n");
        delay(10);   
        while(client.available())
        { String line = client.readStringUntil('\r');
          Serial.print(line); } } 
