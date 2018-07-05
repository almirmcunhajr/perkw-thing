#include <EmonLib.h>
#include <math.h>

#define CT_PIN 1
#define RELAY_PIN 2

#define EPS 1e-2

EnergyMonitor emon1;
float last_irms = -1;

static int current_read() {
  float irms = (float) emon1.calcIrms(1480);

  //if (abs(irms-last_irms) >= EPS) { // Update only on significant changes
    Serial.println(irms);
    //last_irms = irms;
  //}
    
  return 0;
}

static int relay_read() {
  bool state = digitalRead(RELAY_PIN);

  if (state) Serial.println("ON");
  else Serial.println("OFF");

  return 0;
}

static int relay_write(bool state) {
  digitalWrite(RELAY_PIN, state); 
  
  return 0;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_PIN, OUTPUT);
  emon1.current(CT_PIN, 111.1);
  relay_write(true); 
}

void loop() {

  current_read();
  relay_read();

}
