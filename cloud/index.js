const SerialPort = require('serialport');
const messages_types = require('./messages-types');

const serialport = new SerialPort('/dev/ttyUSB0');

const starting_signal = 0x3C ,endding_signal = 0x3E;

var receiving = false;
var msg_type, msg_tail = 0;
var sensor_register_msg = new DataView(new ArrayBuffer(41)), update_msg = new DataView(new ArrayBuffer(65));

var sensor_register_msgs = [], update_msgs = [];

function fillMessage(offset, prune, data) {
	for (let i = offset; i < data.length-prune; i++) {
		if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) {
			sensor_register_msg[msg_tail] = data[i];
		} else if (msg_type == messages_types.UPDATE_MESSAGE) {
			update_msg[msg_tail] = data[i];
		}
		msg_tail++;
	}
}

serialport.on('data', (data) => {
	var head = data[0], tail = data[data.length-1];

	serialport.pause();

	if (head == starting_signal && tail != endding_signal) { // Buffer: <0x3C ...>
		if (data.length > 1) { // If The message type was received
			msg_type = data[1];
			receiving = true;
			fillMessage(2, 0, data); // Two flags in the buffer (starting signal and message type)
		} // Else should be received later 
	} else if (head != starting_signal && tail != endding_signal) { // Buffer: <...>
		if (receiving) { // If the message type is known
			fillMessage(0, 0, data);
		} else { // Else the message type should be the first byte in the buffer
			msg_type = data[0];
			receiving = true;
			fillMessage(1, 0, data);
		}
	} else if (head != starting_signal && tail == endding_signal) { // Buffer: <... 0x3E>
		if (receiving) {
			fillMessage(0, 1, data);
		} else {
			msg_type = data[0];
			receiving = true;
			fillMessage(1, 1, data);
		}
		receiving = false; // End of message
		if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) { // Save message
			sensor_register_msgs.push(sensor_register_msg);
		} else if (msg_type == messages_types.UPDATE_MESSAGE) {
			update_msgs.push(update_msg);
		}
		msg_tail = 0; // Ready for another message
	} else if (head == starting_signal && tail == endding_signal) { // Buffer: <0x3C ... 0x3E> (full message)
		fillMessage(2, 1, data);
		msg_type = data[1];
		if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) {
			sensor_register_msgs.push(sensor_register_msg);
		} else if (msg_type == messages_types.UPDATE_MESSAGE) {
			update_msgs.push(update_msg);
		}
		msg_tail = 0;
	}

	serialport.resume();
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})