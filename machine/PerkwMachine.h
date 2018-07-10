#include "Server.h"
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
	} fetch_sensor; // There'is only one fetch sensor for this project

	struct StateSensors {
		uint32_t id;
		uint32_t (*reader)();
		void (*writer)(uint32_t);
	} state_sensor; // There'is only one state sensor for this project

	int state = EXPLORE, incoming_data_tail = 0;
	bool receiving = false;
	float last_electric_current = 0;
	byte incoming_data[1]; // There's only one incoming byte for this project

	Server server;

public:
	void registerFetchSensor(uint32_t id, char *name, float (*reader)()) {
		fetch_sensor.id = id;
		fetch_sensor.reader = reader;
		server.registerSensor(id, name);
	}
	void registerStateSensor(uint32_t id, char *name, uint32_t (*reader)(), void (*writer)(uint32_t)) {
		state_sensor.id = id;
		state_sensor.reader = reader;
		state_sensor.writer = writer;
		server.registerSensor(id, name);
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
			case EXPLORE: // Explore requests from server
				switch (server.explore(&receiving, incoming_data, &incoming_data_tail)) {
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
				if (incoming_data[0]) {
					state_sensor.writer(1);
				} else {
					state_sensor.writer(0);
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
};