#include "Server.h"
#include "States.h"
#include <SensorsInfos.h>
#include <stdint.h>
#include <Arduino.h>
#include <math.h>

class PerkwThing {
public:
	void registerFetchSensor(uint32_t id, char *name, float (*reader)());
	void registerStateSensor(uint32_t id, char *name, bool (*reader)(), void (*writer)(bool));
	void run();
};