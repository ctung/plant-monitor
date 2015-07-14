/* sql_pusher.js
This listens to incoming messages on /plant/brightness and stores the value into the sql database
*/

var sys = require('sys');
var net = require('net');
var mqtt = require('mqtt');
var mysql = require('mysql');

var maxAnalogAmplitude = 4096;
var moistureThreshold = 0.2 * maxAnalogAmplitude;
var waterDuration = 5;

// create sql connection
var connection = mysql.createConnection({
    host     : 'localhost',
    user     : '<user>',
    password : '<password>',
    database : 'iop'
});

connection.connect();

// create an mqtt client object and connect to the mqtt broker
var client = mqtt.connect('mqtt://192.168.1.116');
// subscribe to all plant messages
client.subscribe('/plant/#');

// mqtt topic format is /plant/<plant_name>/<plant_attribute>
// valid plant attributes are light and moisture
var pattern = /\/plant\/(\w+)\/(\w+)/;
var match;
var validAttributes = ['light','moisture','temp','pump'];

client.on('message', function (topic, payload, packet) {
    console.log(topic+'='+payload);

    // check for recognized attribute in topic string
    if (match = topic.match(pattern)) {
	for (var j=0; j<validAttributes.length; j++) {
	    if (validAttributes[j].match(match[2])) {
		// create the plant, if it doesn't already exist in SQL
		connection.query('INSERT IGNORE INTO plant (name) VALUES ("'+match[1]+'")', function(err,result) {
		    if (err) throw err;
		});

		// insert the plant's attribute data into the SQL data table
		var query = 'INSERT INTO data (timestamp,pid,'+match[2]+') VALUES (now(), (SELECT pid FROM plant WHERE name="'+match[1]+'"),'+payload+') ON DUPLICATE KEY UPDATE '+match[2]+'=VALUES('+match[2]+')';
		console.log(query);
		connection.query(query, function(err,results) { 
		    if (err) throw err;
		});

		// tell the plant to add water if the moisture level is below the critical threshold
		if (validAttributes[j] === "moisture" && payload < moistureThreshold) {
		    client.publish('/plant/'+match[1]+'/addWater',waterDuration);
		}
	    }
	}
    }
});
