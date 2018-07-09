const SerialPort = require('serialport');
const messages_types = require('./messages-types');
const EventEmitter = require('events');

class DataEmitter extends EventEmitter {}

const serialport = new SerialPort('/dev/ttyUSB0');
const dataEmiter = new DataEmitter();

const starting_signal = 0x3C ,endding_signal = 0x3E;

var buf = [], msg = [];
var receiving = false;
var msg_type = -1;

function handleMessage() {
	if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) {
		console.log("Registering sensor: " + new Uint32Array(msg.slice(0,4))[0]);
	} else if (msg_type == messages_types.UPDATE_MESSAGE) {
		console.log("Updating sensor value: " + new Uint32Array(msg.slice(0,4))[0] + " " + new Float32Array(msg.slice(4,8))[0]);
	}
}

serialport.on('data', (data) => {
	buf.push(...data);
	dataEmiter.emit('new');
})

dataEmiter.on('new', () => {
	var incoming_byte = buf[0];

	if (receiving) {
		if (incoming_byte != endding_signal) {
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
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})