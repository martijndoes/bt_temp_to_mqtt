#include <ArduinoBLE.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoMqttClient.h>


//these values must be changed
const char broker[] = "192.168.1.11";               // connection setting for mqtt broker
int        port     = 1883;                         // port number for mqtt broker
const char* wifiName = "[NameOfWifi]";              // name of the wifi access point
const char* wifiPass = "[PasswordOfWifi]";                 // password of the wifi access point
bool       HAAutoDiscovery = true;                  // whether or not you want to publish to mqtt autodiscovery topic

//the mqtt topics that the values are being written to
const char topic1[]  = "GrillBT50/grilltemp1";
const char topic2[]  = "GrillBT50/grilltemp2";
const char topic3[]  = "GrillBT50/grilltemp3";
const char topic4[]  = "GrillBT50/grilltemp4";
const char topic5[]  = "GrillBT50/grilltemp5";
const char topic6[]  = "GrillBT50/grilltemp6";
const char HADiscoveryBase[] = "homeassistant/sensor/grillbt50_temperature/temp";


//only change these values if you know what you're doing
String nameOfDevice = "Grill BT5.0";
char* nameOfCharacteristic = "ffb2";
int delayBetweenMeasurements = 10000;

WiFiMulti WiFiMulti;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(wifiName, wifiPass);
  Serial.println();
  Serial.print("Waiting for WiFi... ");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  publishToHomeAssistantForDiscovery();

  while (!Serial);
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central scan");
  // start scanning for peripheral
  BLE.scan();
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  if (peripheral) {
    // discovered a peripheral. Check if it is the grill
    if (peripheral.localName() == nameOfDevice)
    {
      Serial.println("Discovered a peripheral");
      Serial.println("-----------------------");

      // print address
      Serial.print("Address: ");
      Serial.println(peripheral.address());

      // print the advertised service UUIDs, if present
      if (peripheral.hasAdvertisedServiceUuid()) {
        Serial.print("Service UUIDs: ");
        for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
          Serial.print(peripheral.advertisedServiceUuid(i));
          String uuid = peripheral.advertisedServiceUuid(i);
          Serial.print(" ");
        }
        Serial.println();
      }

      // print the RSSI
      Serial.print("RSSI: ");
      Serial.println(peripheral.rssi());

      Serial.println();
      BLE.stopScan();
      
      controlPeripheral(peripheral);
    }
  }
}

void controlPeripheral(BLEDevice peripheral) {

  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");

  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  while (peripheral)
  {
      BLECharacteristic characteristic = peripheral.characteristic(nameOfCharacteristic);
        if (characteristic && characteristic.canSubscribe()) {
          characteristic.subscribe();

          // repeat the loop
          while (peripheral.connected()) {
            if (characteristic.valueUpdated()) {
              // read the value if it is updated
              byte value[characteristic.valueLength()];
              characteristic.readValue(value, characteristic.valueLength());

              //get the correct byte
              int16_t tempRaw1 = (value[2] << 8) | value[3];
              int16_t tempRaw2 = (value[4] << 8) | value[5];
              int16_t tempRaw3 = (value[6] << 8) | value[7];
              int16_t tempRaw4 = (value[8] << 8) | value[9];
              int16_t tempRaw5 = (value[10] << 8) | value[11];
              int16_t tempRaw6 = (value[12] << 8) | value[13];

              //convert to float and divide by ten
              float tempCelsius1 = tempRaw1 / 10.0;
              String temp1 = String(tempCelsius1);
              float tempCelsius2 = tempRaw2 / 10.0;
              String temp2 = String(tempCelsius2);
              float tempCelsius3 = tempRaw3 / 10.0;
              String temp3 = String(tempCelsius3);
              float tempCelsius4 = tempRaw4 / 10.0;
              String temp4 = String(tempCelsius4);
              float tempCelsius5 = tempRaw5 / 10.0;
              String temp5 = String(tempCelsius5);
              float tempCelsius6 = tempRaw6 / 10.0;
              String temp6 = String(tempCelsius6);


              //print to sysout
              Serial.print("Temperature from channel 1: ");
              Serial.println(tempCelsius1);
              Serial.print("Temperature from channel 2: ");
              Serial.println(tempCelsius2);
              Serial.print("Temperature from channel 3: ");
              Serial.println(tempCelsius3);
              Serial.print("Temperature from channel 4: ");
              Serial.println(tempCelsius4);
              Serial.print("Temperature from channel 5: ");
              Serial.println(tempCelsius5);
              Serial.print("Temperature from channel 6: ");
              Serial.println(tempCelsius6);


              // send values to mqtt
              mqttClient.beginMessage(topic1);
              mqttClient.print("{\"temperature\": "+ temp1 +"}");
              mqttClient.endMessage();

              mqttClient.beginMessage(topic2);
              mqttClient.print("{\"temperature\": "+ temp2 +"}");
              mqttClient.endMessage();    

              mqttClient.beginMessage(topic3);
              mqttClient.print("{\"temperature\": "+ temp3 +"}");
              mqttClient.endMessage();    

              mqttClient.beginMessage(topic4);
              mqttClient.print("{\"temperature\": "+ temp4 +"}");
              mqttClient.endMessage();    

              mqttClient.beginMessage(topic5);
              mqttClient.print("{\"temperature\": "+ temp5 +"}");
              mqttClient.endMessage();    

              mqttClient.beginMessage(topic6);
              mqttClient.print("{\"temperature\": "+ temp6 +"}");
              mqttClient.endMessage();                                                                                    
            }
          }
        }
      delay(delayBetweenMeasurements);
  }  
}


void publishToHomeAssistantForDiscovery() {
  Serial.println("publishing autodiscovery json");
  for (int i = 1; i<7; i++)
  {
    String iStr = String(i);
    String jsonString = "{"
      "\"device_class\": \"temperature\","
      "\"state_topic\": \"GrillBT50/grilltemp"+iStr+"\","
      "\"name\":\"grilltemp"+iStr+"\","
      "\"unit_of_measurement\": \"°C\","
      "\"value_template\": \"{{ value_json.temperature }}\","
      "\"unique_id\": \"grillbt-temp0"+iStr+"\","
      "\"device\": {"
        "\"identifiers\": ["
          "\"grillbt-temp\""
        "],"
        "\"name\": \"GrillBT50\","
        "\"manufacturer\": \"MvD\""
      "}"
    "}";

    String discoveryTopic = HADiscoveryBase + iStr + "/config";
    mqttClient.beginMessage(discoveryTopic, jsonString.length(), true, 1);
    mqttClient.print(jsonString);
    mqttClient.endMessage();
  }
}




