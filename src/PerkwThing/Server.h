#include "MessagesTypes.h"
#include "States.h"
#include <stdint.h>
#include <string.h>
#include <Arduino.h>

class Server {
public:
	void registerSensor(uint32_t id, char *name);
	int explore(int *state, bool *receiving, byte *incoming_data, int *tail);
	void update(uint32_t id, byte value[4]);
};