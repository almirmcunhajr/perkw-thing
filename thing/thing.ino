#include <EmonLib.h>
#include <PerkwMachine.h>
#include <SensorsInfos.h>

EnergyMonitor emon1;
PerkwMachine machine; 

float currentRead() {
  float irms = (float) emon1.calcIrms(1480);

  return irms;
}

uint32_t relayRead() {
  uint32_t state;

  if (digitalRead(RELAY_PIN) == HIGH) {
    state = 1;
  } else {
    state = 0;
  }

  return state;
}

void relayWrite(uint32_t state) {
  if (state == 1) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  emon1.current(CURRENT_SENSOR_PIN, 111.1);
  machine.registerFetchSensor(CURRENT_SENSOR_ID, CURRENT_SENSOR_NAME, currentRead);
  machine.registerActuator(RELAY_ID, RELAY_NAME, relayRead, relayWrite);
}

void loop() {
  machine.run();
}
