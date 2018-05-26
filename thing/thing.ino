#include <KNoTThing.h>
#include <EmonLib.h>

#define CT_ID 1
#define CT_PIN 1
#define CT_NAME "Current transformer"

#define RELAY_ID 2
#define RELAY_PIN 1
#define RELAY_NAME "Relay"

KNoTThing thing;
EnergyMonitor emon1;

static int current_read(uint8_t *val) {
  return 0;
}

static int relay_read(uint8_t *val) {
 return 0;
}

static int relay_write(uint8_t *val) {
  return 0;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_PIN, OUTPUT);
  emon1.current(CT_PIN, 111.1); 
  
  thing.init("KNoTThing");
  thing.registerFloatData(CT_NAME, CT_ID, KNOT_TYPE_ID_CURRENT, KNOT_UNIT_CURRENT_A, current_read, );
  thing.registerBoolData(RELAY_NAME, RELAY_ID, KNOT_TYPE_ID_SWITCH, KNOT_UNIT_NOT_APPLICABLE, relay_read, relay_write);
  thing.registerDefaultConfig(CT_ID, KNOT_EVT_FLAG_CHANGE, 0, 0, 0, 0, 0);
  thing.registerDefaultConfig(RELAY_ID, KNOT_EVT_FLAG_TIM, 10, 0, 0, 0, 0);

  Serial.println(F("Perkw Thing"));
}

void loop() {
  thing.run();
}
