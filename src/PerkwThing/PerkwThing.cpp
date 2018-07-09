#include "PerkwThing.h"

#define EPS 1e-2

struct FetchSensors {
	uint32_t id;
	float (*reader)();
} fetch_sensor; // There'is only one fetch sensor for this project

struct StateSensors {
	uint32_t id;
	uint32_t (*reader)();
	void (*writer)(uint32_t);
} state_sensor; // There'is only one state sensor for this project

union IncomingData { // There's only one type of server request for this project
	byte raw[1];
	byte relay_state;
} incoming_data;

int state = EXPLORE, incoming_data_tail = 0;
bool receiving = false;
float last_electric_current = 0;

void PerkwThing::registerFetchSensor(uint32_t id, char *name, float (*reader)()) {
	Server server;

	fetch_sensor.id = id;
	fetch_sensor.reader = reader;
	server.registerSensor(id, name);
}
void PerkwThing::registerStateSensor(uint32_t id, char *name, uint32_t (*reader)(), void (*writer)(uint32_t)) {
	Server server;

	state_sensor.id = id;
	state_sensor.reader = reader;
	state_sensor.writer = writer;
	server.registerSensor(id, name);
}

void PerkwThing::run() {
	Server server;
	union ElectricCurrent {
		byte raw[4];
		float value;
	} electric_current;
	union RelayState {
		byte raw[4];
		uint32_t value;
	} relay_state;

	switch (state) {
		case EXPLORE: // Explore requests from server
			switch (server.explore(&receiving, incoming_data.raw, &incoming_data_tail)) {
				case 0: // No incoming data
					state = UPDATE;
					break;
				case 1: // All data received
					state = HANDLE;
					break;
				case 2: // Missing data
					state = EXPLORE;
					break;
			}
			break;
		case HANDLE: // Handle a request of the server
			if (incoming_data.relay_state) {
				state_sensor.writer(false); // True state leads to false normal open electric current (allowed)
			} else {
				state_sensor.writer(true); // False state leads to true normal open electric current (blocked)
			}
			state = EXPLORE;
			break;
		case UPDATE: // Update the sensors values in the cloud
			electric_current.value = fetch_sensor.reader();
			relay_state.value = state_sensor.reader();
			if (abs(electric_current.value - last_electric_current) >= EPS) { // Update only under significative changes
				server.update(CURRENT_SENSOR_ID, electric_current.raw);
				last_electric_current = electric_current.value;
			}
			server.update(RELAY_SENSOR_ID, relay_state.raw);
			state = EXPLORE;
			break; 
	}
}