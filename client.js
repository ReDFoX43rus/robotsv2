const net = require('net');
const path = require('path');
const readline = require('readline')
const rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout
})
fs = require('fs')

const client = net.createConnection(3000,"192.168.0.105", () => {
	console.log("Connected, use console mode")
	rl.on('line', (input) => {
		if(input == "update")
			sendFirmware()
		else if (input.indexOf("filereceive") > -1)
			sendFile(input)
		else client.write(input + "\n")
	})
});

function sendFirmware(){
	var content = fs.readFileSync("build/esp32_robots.bin")
	client.write("update " + content.length + "\n")
	client.write(content)
}

function sendFile(data){
	var file = data.split(' ')
	var content = fs.readFileSync(file[1])

	var query = "filereceive /fat/" + path.basename(file[1]) + " " + content.length + "\n"

	client.write(query)
	client.write(content)
}

client.on('data', (data) => {
	process.stdout.write(data.toString('ascii'));
});
client.on('end', () => {
	console.log('disconnected from server');
	process.exit();
});
