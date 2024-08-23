const express = require('express');
const dgram = require('dgram');
const app = express();
const port = 5500;

app.get('/send-udp-message', (req, res) => {
    const message = Buffer.from('hello');
    const client = dgram.createSocket('udp4');
    client.send(message, 0, message.length, 12345, '192.168.1.181', function(err) {
        client.close();
        if (err) {
            console.error(err);
            return res.status(500).send('Failed to send UDP message');
        }
        res.send('UDP message sent successfully');
    });
});

app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
