#include <KNoTThing.h>
#include <EmonLib.h>
#include <math.h>

#define CT_ID 1
#define CT_PIN 1
#define CT_NAME "Current transformer"

#define RELAY_ID 2
#define RELAY_PIN 2
#define RELAY_NAME "Relay"

#define EPS 1e-2

KNoTThing thing;
EnergyMonitor emon1;
float last_irms = 0;

static int current_read(int32_t *val_int, uint32_t *val_dec, int32_t *multiplier) {
  float irms = (float) emon1.calcIrms(1480);

  if (abs(irms-last_irms) >= EPS) { // Update only on significant changes
    *val_int = irms;
    *val_dec = 0;
    *multiplier = 1;
    last_irms = irms;
  } else {
    *val_int = last_irms;
    *val_dec = 0;
    *multiplier = 1;
  }
  
  return 0;
}

static int relay_read(uint8_t *val) {
  *val = digitalRead(RELAY_PIN);

  return 0;
}

static int relay_write(uint8_t *val) {
  digitalWrite(RELAY_PIN, *val); 
  
  return 0;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_PIN, OUTPUT);
  emon1.current(CT_PIN, 111.1); 
  
  thing.init("KNoTThing");
  thing.registerFloatData(CT_NAME, CT_ID, KNOT_TYPE_ID_CURRENT, KNOT_UNIT_CURRENT_A, current_read, NULL);
  thing.registerBoolData(RELAY_NAME, RELAY_ID, KNOT_TYPE_ID_SWITCH, KNOT_UNIT_NOT_APPLICABLE, relay_read, relay_write);
  thing.registerDefaultConfig(CT_ID, KNOT_EVT_FLAG_CHANGE, 0, 0, 0, 0, 0);
  thing.registerDefaultConfig(RELAY_ID, KNOT_EVT_FLAG_TIME, 10, 0, 0, 0, 0);
}

void loop() {
  thing.run();
}
