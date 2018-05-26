#include <KNoTThing.h>

#define CT_ID 1
#define CT_PIN 1
#define CT_NAME "Current transformer"

#define RELAY_ID 2
#define RELAY_PIN 1
#define RELAY_NAME "Relay"

KNoTThing thing;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  thing.init("KNoTThing");
}

void loop() {
  thing.run();
}
