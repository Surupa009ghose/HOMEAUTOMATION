
#include <DHT.h>/*it is used to include dht11 libary*/
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "SSID"
#define WLAN_PASS       "PASSWORD"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883                  
#define AIO_USERNAME    "mano009das"
#define AIO_KEY         "2450f209b3044bb98064a237cea0c171"
WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp");/*Setup a feed called 'photocell' for Temp.*/
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");


Adafruit_MQTT_Subscribe Everything = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Everything");/*Setup a feed called 'Everything' for subscribing to changes.*/
Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Light");
Adafruit_MQTT_Subscribe automate = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/automate");
Adafruit_MQTT_Subscribe Fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fan");


#define DHTTYPE           DHT11  
DHT dht(D7, DHTTYPE);/*it is used to define the input pin*/
uint32_t delayMS;

void MQTT_connect();


void setup() {


  
  Serial.begin(115200);
  delay(10);

  pinMode(D0,OUTPUT);/*it is used to define the output pin*/
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  Serial.println(F("HOME AUTOMATION USING MQTT AND GOOGLE ASSISTANCE VOICE CONTROL"));
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&Light);/*it is used subscribe to a topic*/
  mqtt.subscribe(&automate);
  mqtt.subscribe(&Fan);
  mqtt.subscribe(&Everything);
}
uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(15000))) {
  
  if (subscription == &Everything) { /*here the subscribed topics are controlled and the pins and functions are defined */
      Serial.print(F("Got_Everything: "));
      Serial.println((char *)Everything.lastread);
      uint16_t num = atoi((char *)Everything.lastread);
       digitalWrite(D0,num);
       digitalWrite(D1,num);
  }
  if (subscription == &Light) {
      Serial.print(F("Got_light: "));
      Serial.println((char *)Light.lastread);
      uint16_t num = atoi((char *)Light.lastread);
      digitalWrite(D0,num);
    }
     if (subscription == &automate) {
      Serial.print(F("Got_auto: "));
      Serial.println((char *)automate.lastread);
      uint16_t num = atoi((char *)automate.lastread);
      digitalWrite(D2,num);
    }

     if (subscription == &Fan) {
      Serial.print(F("Got_Fan: "));
      Serial.println((char *)Fan.lastread);
      uint16_t num = atoi((char *)Fan.lastread);
      digitalWrite(D1,num);
    }

  }

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  {if ( atoi((char *)automate.lastread) == 1)/*if the automate pin is high the automate loop will trigger*/
{
   float t = dht.readTemperature();
   Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
    
    if (t>=29)				/*if the temp is >=29 the D1 pin would be set to high*/
   digitalWrite(D1,HIGH);
   if (t<29)
   digitalWrite(D1,LOW);
   else if 
   ( atoi((char *)automate.lastread) == 0)
   {digitalWrite(5,LOW);}
   }
   
}
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    
  }
  if (! Humidity.publish(h)) { /*if the humidity data is not published it will show Failed in the serial monitor */
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! Temp.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
    // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(10000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
