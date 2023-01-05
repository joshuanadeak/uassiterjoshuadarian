#include <ESP8266WiFi.h>
#include <ThingsBoard.h>

const int MockUnoOutput = D0;
const int Input = D1;
int flag = 0;
int count = 0;

char strbuf[128];
char thingsboardServer[] = "thingsboard.cloud";

const char *topic = "v1/devices/me/telemetry";
const char *ssid = "MP-155";
const char *password = "zzii1733";
const char *token = "sOfjJcnyKLdpiTSdCThL";

WiFiClient espClient;
ThingsBoard client(espClient);

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
  if (!client.connected()){
    reconnect();
  }
  
  pinMode(MockUnoOutput, OUTPUT);
  pinMode(Input, INPUT);
}

void loop() {
  client.loop();

  // Mock Arduino Uno Output - uncomment below for debugging
  //digitalWrite(MockUnoOutput, LOW);
  //delay(1000);
  //digitalWrite(MockUnoOutput, HIGH);
  //delay(1000);

// if ESPsignal pin (4) on Arduino Uno is High (train passes), increment
  if (digitalRead(Input) == HIGH && flag == 0)
  {  
    flag = 1;
    count++;
    Serial.print("Jumlah Kereta: ");
    Serial.println(count);
    client.sendTelemetryFloat("jumlah kereta",count);
    delay(10000);
    flag = 0;
    delay(10);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard node...");
    if (client.connect(thingsboardServer, token)){
      Serial.println("[DONE]");
    } else {
      Serial.print("[FAILED]");
      Serial.println(" : retrying in 5 seconds");
      delay(5000);
    }
  }  
}