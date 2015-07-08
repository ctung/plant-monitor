/* sql_pusher.js
This listens to incoming messages on /plant/brightness and stores the value into the sql database
*/

var sys = require('sys');
var net = require('net');
var mqtt = require('mqtt');
var mysql = require('mysql');

// create sql connection
var connection = mysql.createConnection({
    host     : 'localhost',
    user     : '<user>',
    password : '<password>',
    database : 'plant'
});

connection.connect();

// create an mqtt client object and connect to the mqtt broker
var client = mqtt.connect('mqtt://192.168.1.116');

var channel = '/plant\/brightness';

client.subscribe(channel);

client.on('message', function (topic, payload, packet) {
    console.log(topic+'='+payload);
    if (topic.match(channel)) {
        connection.query('INSERT INTO data (timestamp,brightness) VALUES (now(),'+payload+')', function(err,result) {
            if (err) throw err;
        });
    }
});
