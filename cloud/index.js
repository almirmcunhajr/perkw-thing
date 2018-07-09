const SerialPort = require('serialport');
const messages_types = require('./messages-types');
const sensors_infos = require('./sensors-infos');
const EventEmitter = require('events');

class DataEmitter extends EventEmitter {}

const serialport = new SerialPort('/dev/ttyUSB1');
const dataEmiter = new DataEmitter();

const starting_signal = 0x3C ,endding_signal = 0x3E;

var buf = [], msg = [];
var receiving = false;
var msg_type = -1;

function handleMessage() {
	if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) {
		
	} else if (msg_type == messages_types.UPDATE_MESSAGE) {
		var id = new Uint32Array(msg.slice(0,4))[0];
		var value_buffer = new ArrayBuffer(4);
		var value_uint8 = new Uint8Array(value_buffer);

		value_uint8[0] = msg.slice(4,8)[0];
		value_uint8[1] = msg.slice(4,8)[1];
		value_uint8[2] = msg.slice(4,8)[2];
		value_uint8[3] = msg.slice(4,8)[3];

		var value_view = new DataView(value_buffer);
		var value;

		if (id == sensors_infos.CURRENT_SENSOR_ID) {
			value = value_view.getFloat32(0, true);
		} else if (id = sensors_infos.RELAY_SENSOR_ID) {
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
	}
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})