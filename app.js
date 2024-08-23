const express = require('express');
const mysql = require('mysql');
const cors = require('cors');
const fs = require("fs");
// const https = require('https');
const expressWs = require('express-ws'); // Import express-ws
const WebSocket = require('ws');
const { exec } = require('child_process');
const path = require('path');



const app = express();
const port = 5500;

// // Read the certificate and key files
// const privateKey = fs.readFileSync('./certs/private.key', 'utf8');
// const certificate = fs.readFileSync('./certs/certificate.crt', 'utf8');

// const credentials = { key: privateKey, cert: certificate };
// const httpsServer = https.createServer(credentials, app);

// Use CORS middleware
app.use(cors());
app.use(express.static('public'));



// Add WebSocket support using express-ws
const wsInstance = expressWs(app);

// Handle WebSocket connections
app.ws('/', (ws, req) => {
  console.log('WebSocket connection established.');

  // Handle WebSocket messages here
  ws.on('message', (message) => {
    console.log(`Received message: ${message}`);

    
    // Broadcast the message to all connected clients
    wsInstance.getWss().clients.forEach((client) => {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        // Send the message to other clients (excluding the sender)
        client.send(message);
      }
    });
  });
});


app.use(express.static("src"));

// Route to handle the emergency stop
app.get('/emergency_stop', (req, res) => {
  let sentCount = 0;
  wsInstance.getWss().clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
          client.send(JSON.stringify({ command: 'stopallheadsets' }));
          sentCount++;
      }
  });
  console.log(`Sent 'stop all headsets' command to ${sentCount} clients.`);
  res.send(`Sent 'stop all headsets' command to ${sentCount} clients.`);
});



// Route to handle the emergency stop
app.get('/shutdown', (req, res) => {
  exec('shutdown now', (err, output) => {
    res.send('Shutdown activated!');

  })
});




// app.get("/", (req, res) => {
//   res.sendFile(path.join(__dirname, 'public/index.html'));
// });



// Start the server
// app.listen(port, () => {
//     console.log(`Server running at http://localhost:${port}/`);
// });

app.listen(5500, '0.0.0.0', () => {
    console.log('Server running on port 5500');
  });
  
//   httpsServer.listen(5500, '0.0.0.0', () => {
//     console.log('HTTPS Server running on port 5500');
//   });
  