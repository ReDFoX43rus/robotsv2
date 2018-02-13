const net = require('net');
fs = require('fs')

var content = fs.readFileSync("build/esp32_robots.bin")

const client = net.createConnection(3000,"192.168.1.1", () => {
	client.write(content)
	client.end();
});
client.on('data', (data) => {
	console.log(data.toString('ascii'));
	client.end();
});
client.on('end', () => {
	console.log('disconnected from server');
});
