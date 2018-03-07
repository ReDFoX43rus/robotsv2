const net = require('net');
fs = require('fs')

var content = fs.readFileSync("build/esp32_robots.bin")

const client = net.createConnection(80,"192.168.0.6", () => {
	//client.write("update " + content.length + "\n");
	//client.write(content);
	client.write("help\n")
	client.write("tcp 1\n")
	client.end();
});
client.on('data', (data) => {
	process.stdout.write(data.toString('ascii'));
	client.end();
});
client.on('end', () => {
	console.log('disconnected from server');
});
