const express = require('express');
const SerialPort = require('serialport');
const messages_types = require('./messages-types');
const sensors_infos = require('./sensors-infos');
const EventEmitter = require('events');

class DataEmitter extends EventEmitter {}

const serialport = new SerialPort('/dev/ttyUSB0');
const dataEmiter = new DataEmitter();
const interface = express();

const starting_signal = 0x3C, ending_signal = 0x3E;

var buf = [], msg = [];
var receiving = false;
var msg_type = -1;

function handleMessage() {
	var id = new Uint32Array(msg.slice(0,4))[0];

	if (msg_type == messages_types.REGISTER_MESSAGE) {
		var name = new String();
		var name_msg = msg.slice(4,14);

		for (let i = 0; i < name_msg[i] != 0; i++) {
			name += String.fromCharCode(name_msg[i]);
		}		
	} else if (msg_type == messages_types.UPDATE_MESSAGE) {
		var value_buffer = new ArrayBuffer(4);
		var value_uint8 = new Uint8Array(value_buffer);
		var value_msg = msg.slice(4,8)

		for (let i = 0; i < 4; i++) {
			value_uint8[i] = value_msg[i];
		}

		var value_view = new DataView(value_buffer);
		var value;

		if (id == sensors_infos.CURRENT_SENSOR_ID) {
			value = value_view.getFloat32(0, true);
		} else if (id = sensors_infos.RELAY_ID) {
			value = value_view.getUint32(0);
		}
	}
}

serialport.on('data', (data) => {
	buf.push(...data);
	dataEmiter.emit('new');
})

dataEmiter.on('new', () => {
	var incoming_byte;

	while (buf.length > 0) {
		incoming_byte = buf[0];
		if (receiving) {
			if (incoming_byte != ending_signal) {
				if (msg_type == -1) {
					msg_type = incoming_byte;
				} else {
					msg.push(incoming_byte);
				}
			} else {
				handleMessage();
				receiving = false;
				msg_type = -1;
				msg = [];
			}
		} else if (incoming_byte == starting_signal) {
			receiving = true;
		}
		buf = buf.slice(1);
	}
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})

interface.get('/set-relay-state', (req, res) => {
	var state = req.query.state;

	if (state == 0 || state == 1) {
		let msg = new Buffer.from([starting_signal, state, ending_signal]);
		serialport.write(msg);
		res.send('Done!');
	} else 
		res.send('Invalid param');
})

interface.listen(8000, () => {
	console.log('Perkw interface listening on port 8000');
});