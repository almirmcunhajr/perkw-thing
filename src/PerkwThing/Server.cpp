#include "Server.h"

union SensorRegisterMessage {
	byte raw[41];
	struct Data {
		byte type;
		uint32_t id;
		char name[36];
	} data;
} sensor_register_msg;

union UpdateMessage {
	byte raw[65];
	struct Data {
		byte type;
		uint32_t id;
		byte value[4];
	} data;
} update_msg;

const byte startting_signal = 0x3C;
const byte endding_signal = 0x3E;

void Server::registerSensor(uint32_t id, char *name) {
	sensor_register_msg.data.type = SENSOR_REGISTER_MESSAGE;
	sensor_register_msg.data.id = id;
	strcpy(sensor_register_msg.data.name, name);
	Serial.write(startting_signal);
	Serial.write(sensor_register_msg.raw, 41);
	Serial.write(endding_signal);
}

int Server::explore(bool *receiving, byte *incoming_data, int *tail) {
	byte rb;

	if (Serial.available() == 0) // No incoming data
		return 0;

	while (Serial.available() > 0) {
		rb = Serial.read();
		if (*receiving) {
			if (rb != endding_signal) {
				incoming_data[*tail] = rb;
				*tail++;
			} else {
				*receiving = false;
				*tail = 0;
				return 1; // All data received
			}
		} else if (rb == startting_signal) {
			*receiving = true;
		}
	}

	return 2; // Missing data
}

void Server::update(uint32_t id, byte value[4]) {
	update_msg.data.type = UPDATE_MESSAGE;
	update_msg.data.id = id;
	memcpy(update_msg.data.value, value, 4);
	Serial.write(startting_signal);
	Serial.write(update_msg.raw, 65);
	Serial.write(endding_signal);
}