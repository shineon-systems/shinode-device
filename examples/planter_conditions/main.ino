#include <vector>
#include <Shinode.h>
#include <Adafruit_AHTX0.h>

int SOIL_MOISTURE = A0;
Sensor moisture_sensor = {
  String("soil_moisture"),
  String("%"),
  [=]() { pinMode(SOIL_MOISTURE, OUTPUT); },
  [=]() {
    float value = analogRead(SOIL_MOISTURE);
    int max = 290;
    int min = 720;
    float calibrated = ((value - min) / (max - min)) * 100;
    return String(calibrated);
  }
};

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_temp;
Sensor temperature_sensor = {
  String("temperature"),
  String("degrees C"),
  [&]() { 
    if (!aht.begin()) {
      Serial.println("Failed to find AHT10/AHT20 chip");
      while (1) {
        delay(10);
      }
    }
  },
  [&]() {
    sensors_event_t temp;
    aht_temp = aht.getTemperatureSensor();
    aht_temp->getEvent(&temp);
    return String(temp.temperature);
  }
};

Adafruit_Sensor *aht_humidity;
Sensor humidity_sensor = {
  String("humidity"),
  String("%H"),
  [=]() { 
    if (!aht.begin()) {
      Serial.println("Failed to find AHT10/AHT20 chip");
      while (1) {
        delay(10);
      }
    }
  },
  [=]() {
    sensors_event_t humidity;
    aht_humidity = aht.getHumiditySensor();
    aht_humidity->getEvent(&humidity);
    return String(humidity.relative_humidity);
  }
};

Shinode device(
  "123",
  "super_secret_token",
  APSSID,
  APPSK,
  host,
  rootCACert,
  std::vector<Sensor>{ moisture_sensor, temperature_sensor, humidity_sensor },
  std::vector<Controller>{}
);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();
  
  device.setup();
}

void loop() {
  device.sync();
  delay(1000);
}