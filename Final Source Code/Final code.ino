#include <DHT.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <HardwareSerial.h>
#include <TinyGPS.h>

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

typedef struct gps_data_str{
  float flat;
  float flon;
}data_str;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
TinyGPS gps;
HardwareSerial mySerial = HardwareSerial(2);

bool newData_s = false;
int jin = 3;
int cnt = 0;

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

data_str gps_data(bool newData)
{
  float flat, flon;
  unsigned long date, time;
  data_str struc;

  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (mySerial.available())
    {
      char c = mySerial.read();
      Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    struc.flat = flat;
    struc.flon = flon;
  }

  else
  {
    struc.flat = 0;
    struc.flon = 0;
  }

  return struc;
}

int jin_data()
{
  if(digitalRead(jin) == HIGH) {
    cnt += 1;
    delay(500);
  }

  return cnt;
}

void publishMessage()
{
  data_str result;
  result = gps_data(newData_s);
  int jin_res = jin_data();

  StaticJsonDocument<200> doc;
  doc["lat"] = result.flat;
  doc["lon"] = result.flon;
  doc["jin"] = jin_res;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, payload);
//  const char* message = doc["message"];
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(jin, INPUT);
  connectAWS();
}

void loop() {
  publishMessage();
  client.loop();
  delay(1000);
}
