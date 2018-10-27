#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

// Thông tin Wifi
const char* ssid ="Airtel-E5573-2E5B";
const char* password ="0n4q619r";
const char* mqtt_server = "m15.cloudmqtt.com";
const char* mqtt_user = "yrmtcgrh";
const char* mqtt_pass = "Cnv2Bj8SigYi";
const int mqtt_port = 17121;
const char* mqtt_client_name = "iothutech"; // Client connections cant have the same connection name

#define BUILTIN_LED 21

// ClientID 
const char* clientID = "khanhneyiothutechzs";

//Thiết lập output xuất(public) và nhận(subscibe) dữ liệu với MQTT
const char* outTopic = "public/esp/outputkhanhney";
const char* inTopic = "public/esp/inputkhanhney";


unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
const int threshold = 600;
// thiết lập ngưỡng do chúng ta tự setup
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int count = 0;

// Hàm kết nối Wifi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//  pinMode(D2, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
//  khai báo sử dụng D2 làm gốc test
}

// Hàm call back được gọi khi nhận được thông tin từ subcribing topic
void callback(char* topic, byte* payload, unsigned int length) {
  // xử lý dữ liệu nhận được
  /*
   * convert string to int giá trị cường độ sáng
   * if/else điều khiển bật tắt relay dựa trên ngưỡng
  */
  
  Serial.println("HELLO KHANHNEY_TESTY/");
  char* message = (char*) payload;
  int value = atoi(message);
//  hàm convert string sang number thôi ae
  Serial.println(value);
  if (value > threshold) {
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.println("HIGH");
      Serial.println(message);
  } else {
    digitalWrite(BUILTIN_LED, LOW); 
    Serial.println("LOW");
    Serial.println(message);
  }
}

// Reconnect đến MQTT Broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, clientID);
      // ... and resubscribe
      client.subscribe(inTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//vòng đời kiểm tra kết nối
void publishValue() {
  count++;
  dtostrf(count, 2, 2, msg);
  //Serial.println(msg);
  dtostrf(analogRead(A0), 2, 2, msg);
  //Serial.println(msg);
  //client.publish(outTopic, msg);
  client.publish(outTopic, msg);
  delay(2000);
//  nếu sử dụng public dữ liệu thì sau 2s sẽ gửi tín hiệu lên topic/server 1 lần
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) {
      Serial.println("connected");  
      client.subscribe(inTopic);   
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
   //publishValue();
  client.loop();
}
//vòng lập chính của chương trình
