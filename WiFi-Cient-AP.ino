#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"
#include <PubSubClient.h>
#define LED_PIN D1
int speakerPin = 14;
StaticJsonDocument<300> doc;

const char* ssid = "Phenikaa 167 HN";     // your network SSID (name)::QUOC DAT
const char* pass = "phenikaa2018"; // your network password::66668888

class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
     char data_str[length+1];
     os_memcpy(data_str, data, length);
     data_str[length] = '\0';  
     DeserializationError error  = deserializeJson(doc, data);//data = json
     if (error)
     {
         Serial.println(" deserrialzeJon()failed ");

     }
     int led = doc["led"];
     int speaker = doc["speaker"]; 
     
     if(led == 0 && speaker==0) 
     {
        digitalWrite(LED_PIN, LOW); 
        Serial.println(" Turn Off LED! " );
        tone(speakerPin,100,1000);
     } 

     if(led==1 && speaker==1)
     {
        digitalWrite(LED_PIN, HIGH);
        delay(3000);
        digitalWrite(LED_PIN, LOW); 
        Serial.println(" Turn On LED! " );
        tone(speakerPin,1000,500);
      
     }
     

      Serial.println("received topic '"+topic+"' with data '"+(String)data_str+"'");
     }
};

myMQTTBroker myBroker;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void startWiFiClient()
{
  Serial.println("Connecting to "+(String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(5000);
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi client disconnected");
    startWiFiAP();
  }
  Serial.println("");
  
  Serial.println("WiFi client connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}


void startWiFiAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP("TUAN-^^","88888888");
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_PIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("led", "hello world");
      // ... and resubscribe
      client.subscribe("led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
//  WiFi.mode(WIFI_AP);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
//  startWiFiAP();

  startWiFiClient();
  

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();


  myBroker.subscribe("led");
//
  client.setServer("192.168.4.1", 1883);
  client.setCallback(callback);  
//
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  digitalWrite(LED_PIN, LOW);
 

}

//int counter = 1;

  
void loop()
{

//   myBroker.publish("led", (String)counter++);
 Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
  // wait a second
delay(1000);
}
