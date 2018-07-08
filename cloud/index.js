const SerialPort = require('serialport');
const messages_types = require('./messages-types');

const serialport = new SerialPort('/dev/ttyUSB0');
const starting_signal = 0x3C ,endding_signal = 0x3E;

var receiving = false, new_message = false;
var msg_type, msg_tail = 0;
var sensor_register_msg = new DataView(new ArrayBuffer(41)), update_msg = new DataView(new ArrayBuffer(65));

function fillMessage(offset, prune, data_view) {
	for (let i = offset; i < data_view.byteLength-prune; i++) {
		if (msg_type == messages_types.SENSOR_REGISTER_MESSAGE) {
			sensor_register_msg[msg_tail] = data_view[i];
		} else if (msg_type == messages_types.UPDATE_MESSAGE) {
			update_msg[msg_tail] = data_view[i];
		}
		msg_tail++;
	}
}

serialport.on('data', (data) => {
	var data_view = new DataView(data.buffer);
	var head = data_view[0], tail = data_view[data_view.byteLength-1];

	if (!new_message) { // Supposing that the data wasn't/won't be lost and the protocol is well known by the thing
		if (head == starting_signal && tail != endding_signal) { // Buffer: <0x3C ...>
			if (data_view.byteLength > 1) { // If The message type was received
				msg_type = data_view[1];
				receiving = true;
				fillMessage(2, 0, data_view); // Two flags (fist and second byte, i.e., offset = 2)
			} // Else should be received later 
		} else if (head != starting_signal && tail != endding_signal) { // Buffer: <...>
			if (receiving) { // If the message type is known
				fillMessage(0, 0, data_view); // There's no flag in the buffer
			} else { // Else the message type should be the first byte in the buffer
				msg_type = data_view[0];
				receiving = true;
				fillMessage(1, 0, data_view); // One flag (first byte, i.e., offset = 1)
			}
		} else if (head != starting_signal && tail == endding_signal) { // Buffer: <... 0x3E>
			if (receiving) {  // If the message type is known
				fillMessage(0, 1, data_view); // One flag (last byte, i.e., prune = 1)
				new_message = true; // New message available to handle
			} else { // Else the message type should be the first byte in the buffer
				msg_type = data_view[0];
				receiving = true;
				fillMessage(1, 1, data_view); // Two flags (first and last byte, i.e., offset = 1, prune = 1)
				new_message = true;
			}
			receiving = false; // End of message
			msg_tail = 0; // Ready for another message
		} else if (head == starting_signal && tail == endding_signal) { // Buffer: <0x3C ... 0x3E> (full message)
			fillMessage(2, 1, data_view); // Three flags (first, second and last byte, i.e., offset = 2, prune = 1)
			msg_type = data_view[1];
			new_message = true;
			msg_tail = 0;
		}
	}
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})