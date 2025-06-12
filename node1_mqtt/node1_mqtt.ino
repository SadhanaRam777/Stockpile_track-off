
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 4    
#define IR 15
#define Fire 5

const int gas = 36;
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "Sadhana";
const char* password = "sadhanaram";
const char* mqtt_server = "192.168.54.41";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int BUILTIN_LED=2;
int count;

void setup_wifi() {

  delay(10);
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
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  pinMode(IR,INPUT);
  pinMode(Fire,INPUT);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
  
 
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {Serial.println(F("Failed to read from DHT sensor!"));return;}
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
//  Serial.print(F("Humidity: "));
//  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

int G=analogRead(gas);
G=(G*100)/4095;
Serial.println(G);
String hum=String(h);
String tem=String(t);
String GAS=String(G);

  char buff1[10]="";
char buff2[10]="";
char buff3[10]="";
char buff4[10]="";
  // Convert String to char array
  int str_len = hum.length() + 1;
  char textmessage[str_len];
  hum.toCharArray(textmessage,str_len);
  strcpy(buff1,textmessage);

   int str_len1 = GAS.length() + 1;
  char textmessage1[str_len1];
  GAS.toCharArray(textmessage1,str_len1);
  strcpy(buff3,textmessage1);

   int str_len2 = tem.length() + 1;
  char textmessage2[str_len2];
  tem.toCharArray(textmessage2,str_len2);
  strcpy(buff2,textmessage2);
    Serial.print("Publish message: ");
    Serial.println(msg);
    if(digitalRead(Fire)==0)
    {
      Serial2.begin(9600);
      SendSms("7975706981 ","Fire detected");
      delay(2000);
      }
      if(t>35)
      {
        Serial2.begin(9600);
      SendSms("7975706981","High Teperature");
      delay(2000);
      }
if(digitalRead(IR)==0)
{
  Serial.println("IR Sensor on");
   count=count+1;
   String c=String(count);
  int str_len4 = c.length() + 1;
  char textmessage4[str_len4];
  c.toCharArray(textmessage4,str_len4);
  strcpy(buff4,textmessage4);
  
//    client.publish("outTopic", msg);
    client.publish("device/temp", buff2);
    client.publish("device/humidity", buff1);
    client.publish("device/gas", buff3);
   
    client.publish("device/count", buff4);
    delay(500);
    if(count==30)
    {
      count=0;
      Serial2.begin(9600);
      SendSms("7975706981","Over time Storage");
      delay(2000); 
     }
    if(G>20)
    {
      Serial2.begin(9600);
      SendSms("7975706981","High Methane");
      delay(2000); 
     }
}
else
{
    
//    client.publish("outTopic", msg);
    client.publish("device/temp", "0");
    client.publish("device/humidity", "0");
    client.publish("device/gas", "0");
    delay(1000);
}  
}

void SendSms( char *num1, char * str1 )
{
    char buff[10],i=0;
    Serial2.write('A');
    delay(100);
    Serial2.write('T');
    delay(100);
    Serial2.write('E');
    delay(100);
    Serial2.write('0');
    delay(100);
    Serial2.write('\r');
    
    //***************
    Serial2.write("AT+CMGF=1\n\r"); //Initialize GSM For mobile
    delay(2000);
    Serial2.write("AT+CMGS=\"");
    delay(2000);
    Serial2.write(num1);
    delay(2000);
    Serial2.write("\"\r");
    Serial2.write(str1);
    delay(2000);
    Serial2.write(26);
    delay(2000);
    Serial.print("sms sent");

}
