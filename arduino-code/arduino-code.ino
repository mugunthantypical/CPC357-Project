#include "VOneMqttClient.h"
#include <ESP32Servo.h>
#include "DHT.h"

int MinMoistureValue = 4095;
int MaxMoistureValue = 2060;
int MinMoisture = 0;
int MaxMoisture = 100;
int Moisture = 0;
int liquidDetected = 0;

// Control flags
bool automationEnabled = true;              // Default: Automation mode is ON
unsigned long lastManualCommandTime = 0;    // Timestamp of the last manual command
const unsigned long manualTimeout = 5000;  // 60 seconds timeout for manual control

// Add these constants for better control
const int MOISTURE_LOW_THRESHOLD = 30;    // When to start watering
const int MOISTURE_HIGH_THRESHOLD = 60;   // When to stop watering
const int SERVO_OPEN_ANGLE = 90;          // Servo angle for open position
const int SERVO_CLOSED_ANGLE = 0;         // Servo angle for closed position
const unsigned long MIN_WATERING_TIME = 5000;  // Minimum watering duration (5 seconds)
const unsigned long MAX_WATERING_TIME = 30000; // Maximum watering duration (30 seconds)

// Add these variables for tracking
unsigned long wateringStartTime = 0;
bool isWatering = false;

//define device id
const char* DHT11Sensor = "87eb0b4b-9d6b-4ab0-a1d1-db266137148d";     //Replace this with YOUR deviceID for the DHT11 sensor
const char* ServoMotor = "11c4bc5a-9d01-4272-b405-7e98451198cc";      //Replace this with YOUR deviceID for the servo
const char* MoistureSensor = "db34ad93-78d4-47ab-b576-1f1cae91f337";  //Replace this with YOUR deviceID for the moisture sensor

//Used Pins
const int dht11Pin = 42;        //Right side Maker Port
const int servoPin = 21;        //Pin 21
const int moisturePin = A2;     //Middle Maker Port
const int liquidSensorPin = 4;  // GPIO pin connected to the sensor's signal wire
const int buzzer = 12;

//input sensor
#define DHTTYPE DHT11
DHT dht(dht11Pin, DHTTYPE);

//Output
Servo Myservo;

//Create an instance of VOneMqttClient
VOneMqttClient voneClient;

//last message time
unsigned long lastMsgTime = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand) {
  //actuatorCommand format {"servo":90}
  Serial.print("Main received callback : ");
  Serial.print(actuatorDeviceId);
  Serial.print(" : ");
  Serial.println(actuatorCommand);

  String errorMsg = "";

  JSONVar commandObjct = JSON.parse(actuatorCommand);
  JSONVar keys = commandObjct.keys();

  if (String(actuatorDeviceId) == ServoMotor) {
    //{"Servo":90}
    String key = "";
    JSONVar commandValue = "";
    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      commandValue = commandObjct[keys[i]];
    }

    if (key == "Servo") {
      // Manual servo control
      int angle = (int)commandValue;
      automationEnabled = false;         // Temporarily disable automation
      lastManualCommandTime = millis();  // Record the time of the manual command
      Myservo.write(angle);              // Apply the manual servo angle
      Serial.print("Manual control: Servo set to ");
      Serial.println(angle);
    }

    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), true);  // Publish actuator status
  }

  else {
    Serial.print("No actuator found : ");
    Serial.println(actuatorDeviceId);
    errorMsg = "No actuator found";
    voneClient.publishActuatorStatusEvent(actuatorDeviceId, actuatorCommand, errorMsg.c_str(), false);  //publish actuator status
  }
}

void setup() {

  setup_wifi();
  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);

  //sensor
  dht.begin();
  pinMode(dht11Pin, INPUT);
  pinMode(liquidSensorPin, INPUT);
  pinMode(buzzer, OUTPUT);

  //actuator
  Myservo.attach(servoPin);
  Myservo.write(0);
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    String errorMsg = "Sensor Fail";
    voneClient.publishDeviceStatusEvent(DHT11Sensor, true);
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
  }
  voneClient.loop();

  unsigned long cur = millis();

  // Resume automation after timeout
  if (!automationEnabled && (cur - lastManualCommandTime > manualTimeout)) {
    automationEnabled = true;
    Serial.println("Resuming automation mode after manual timeout.");
    // Publish automation status change
    JSONVar statusPayload;
    statusPayload["AutomationStatus"] = "Enabled";
    voneClient.publishTelemetryData(ServoMotor, statusPayload);
  }

  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;

    //Publish humid & temp data
    float h = dht.readHumidity();
    int t = dht.readTemperature();

    if (!isnan(h) && !isnan(t) && h >= 20 && h <= 90 && t >= 0 && t <= 50) {
      JSONVar payloadObject;
      payloadObject["Humidity"] = h;
      payloadObject["Temperature"] = t;
      voneClient.publishTelemetryData(DHT11Sensor, payloadObject);
    }

    //Publish moisture data
    int sensorValue = analogRead(moisturePin);
    Moisture = map(sensorValue, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
    JSONVar moisturePayload;
    moisturePayload["Soil moisture"] = Moisture;
    voneClient.publishTelemetryData(MoistureSensor, moisturePayload);
  }

  // Improved automated servo control
  if (automationEnabled) {
    // Reset watering state if we were watering but ran out of water
    if (isWatering && digitalRead(liquidSensorPin) == 0) {
      isWatering = false;
      Myservo.write(SERVO_CLOSED_ANGLE);
      Serial.println("Stopping watering - water tank empty");
    }

    if (!isWatering && Moisture < MOISTURE_LOW_THRESHOLD) {
      // Check if there's water before starting
      if (digitalRead(liquidSensorPin) == 1) {  // Water detected
        isWatering = true;
        wateringStartTime = millis();
        Myservo.write(SERVO_OPEN_ANGLE);
        Serial.println("Starting watering cycle");
      } else {
        Serial.println("Cannot start watering - no water detected");
      }
    }
    else if (isWatering) {
      // Stop watering if either condition is met:
      // 1. Moisture level is high enough
      // 2. Maximum watering time exceeded
      // 3. Minimum watering time has passed AND moisture is above low threshold
      if (Moisture > MOISTURE_HIGH_THRESHOLD || 
          (cur - wateringStartTime > MAX_WATERING_TIME) ||
          ((cur - wateringStartTime > MIN_WATERING_TIME) && Moisture > MOISTURE_LOW_THRESHOLD)) {
        isWatering = false;
        Myservo.write(SERVO_CLOSED_ANGLE);
        Serial.println("Stopping watering cycle");
      }
    }
  }

  liquidDetected = analogRead(liquidSensorPin);
  if (liquidDetected == 0) {  // No water detected
    tone(buzzer, 1000);
    delay(1000);
  } else {
    noTone(buzzer);
  }
}