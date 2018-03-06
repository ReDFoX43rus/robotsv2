const net = require('net');
fs = require('fs')

var content = fs.readFileSync("scripts/debug.sh")

const client = net.createConnection(80,"192.168.0.6", () => {
	client.write("filereceive /fat/debug.sh " + content.length + "\n");
	client.write(content);
	client.end();
});
client.on('data', (data) => {
	console.log(data.toString('ascii'));
	client.end();
});
client.on('end', () => {
	console.log('disconnected from server');
});
