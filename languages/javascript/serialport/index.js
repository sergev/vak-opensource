#!/usr/bin/env node

var SerialPort = require('serialport');
var port = new SerialPort('/dev/tty.wchusbserialfd130', {
    baudRate: 9600
});

// Open errors will be emitted as an error event
port.on('error', function(err) {
    console.log('Error: ', err.message);
})

port.on('data', function (data) {
    // Convert the string into a JSON object:
    var serialData = JSON.parse(data);
    console.log('Data:', serialData);
});
