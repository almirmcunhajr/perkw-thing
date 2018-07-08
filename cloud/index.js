const SerialPort = require('serialport');

const serialport = new SerialPort('/dev/ttyUSB0');

serialport.on('data', (data) => {
})

serialport.on('error', (err) => {
  console.log('Serial port error: ', err.message);
})