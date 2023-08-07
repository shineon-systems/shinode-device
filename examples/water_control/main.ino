#include <vector>
#include <Shinode.h>
#include <Adafruit_AHTX0.h>

int SOLENOID = 2;
Controller water_controller = {
  String("water_valve"),
  String("seconds open"),
  [=]() { 
    pinMode(SOLENOID, OUTPUT); 
    digitalWrite(SOLENOID, HIGH);
  },
  [=](Result result) {
    Serial.println("Controlling water " + result.value);
    digitalWrite(SOLENOID, LOW);
    delay(result.value.toInt() * 1000);
    digitalWrite(SOLENOID, HIGH);
    return String(0);
  }
};

Shinode device(
  "456",
  "super_secret_token",
  APSSID,
  APPSK,
  host,
  rootCACert,
  std::vector<Sensor>{},
  std::vector<Controller>{ water_controller }
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
