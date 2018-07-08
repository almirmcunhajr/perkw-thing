#include <EmonLib.h>
#include <PerkwThing.h>
#include "SensorsInfos.h"

EnergyMonitor emon1;
PerkwThing thing; 

float currentRead() {
  float irms = (float) emon1.calcIrms(1480);

  return irms;
}

bool relayRead() {
  bool state = digitalRead(RELAY_PIN);

  return state;
}

void relayWrite(bool state) {
  digitalWrite(RELAY_PIN, state);
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  emon1.current(CURRENT_SENSOR_PIN, 111.1);
  thing.registerFetchSensor(CURRENT_SENSOR_ID, CURRENT_SENSOR_NAME, currentRead);
  thing.registerStateSensor(RELAY_SENSOR_ID, RELAY_NAME, relayRead, relayWrite);
}

void loop() {
  thing.run();
}
