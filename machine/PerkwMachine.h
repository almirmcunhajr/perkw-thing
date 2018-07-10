#include "Comm.h"
#include "States.h"
#include <SensorsInfos.h>
#include <stdint.h>
#include <Arduino.h>
#include <math.h>

#define EPS 1e-2

class PerkwMachine {
private:
	struct FetchSensors {
		uint32_t id;
		float (*reader)();
	} current_sensor; // There'is only one fetch sensor for this project

	struct Actuators {
		uint32_t id;
		uint32_t (*reader)();
		void (*writer)(uint32_t);
	} relay; // There'is only one state sensor for this project relay

	int state = EXPLORE, incoming_data_tail = 0;
	bool receiving = false, received = false;
	float last_electric_current = 0;
	byte incoming_data[1]; // There's only one incoming byte for this project

	Comm comm;

public:
	void registerFetchSensor(uint32_t id, char *name, float (*reader)()) {
		current_sensor.id = id;
		current_sensor.reader = reader;
		comm.cregister(id, name);
	}
	void registerActuator(uint32_t id, char *name, uint32_t (*reader)(), void (*writer)(uint32_t)) {
		relay.id = id;
		relay.reader = reader;
		relay.writer = writer;
		comm.cregister(id, name);
	}

	void run() {
		union ElectricCurrent {
			byte raw[4];
			float value;
		} electric_current;
		union RelayState {
			byte raw[4];
			uint32_t value;
		} relay_state;

		switch (state) {
			case EXPLORE: // Explore requests from communicator
				switch (comm.explore(&receiving, &received, incoming_data, &incoming_data_tail)) {
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
			case HANDLE: // Handle a request of the communicator
				if (incoming_data[0]) {
					relay.writer(1);
				} else {
					relay.writer(0);
				}
				state = EXPLORE;
				break;
			case UPDATE: // Update the sensors values in the cloud
				electric_current.value = current_sensor.reader();
				relay_state.value = relay.reader();
				if (abs(electric_current.value - last_electric_current) >= EPS) { // Update only under significative changes
					comm.update(CURRENT_SENSOR_ID, electric_current.raw);
					last_electric_current = electric_current.value;
				}
				comm.update(RELAY_ID, relay_state.raw);
				state = EXPLORE;
				break;
		}
	}
};