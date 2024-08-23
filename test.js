// This function listens for changes on any dropdown with the 'language-dropdown' class and updates the languages accordingly
function addEventListenersToLanguageDropdowns() {
    document.querySelectorAll('.language-dropdown').forEach(dropdown => {
        dropdown.addEventListener('change', function() {
            const groupId = this.getAttribute('data-group-id');
            const newLanguage = this.value;
            updateGroupLanguages(groupId, newLanguage);
        });
    });
}

function updateGroupLanguages(groupId, newLanguage) {
    // Select all devices within the specified group
    const devicesInGroup = document.querySelectorAll(`#group${groupId} .device`);

    devicesInGroup.forEach(device => {
        // Extract the unique ID for each device
        const uniqueId = device.getAttribute('data-unique-id') || device.id.replace('device-', '');

        // Send the language update command to each device by its unique ID
        sendLanguageCommandToDevice(newLanguage.toLowerCase(), uniqueId);

        // Update the UI to reflect the new language
        const languageDisplayElement = device.querySelector('.language-display'); // Adjust this selector based on your HTML structure
        if (languageDisplayElement) {
            languageDisplayElement.textContent = newLanguage;
        }
    });

    // Log the result
    console.log(`Language for Group ${groupId} updated to ${newLanguage}. Total devices updated: ${devicesInGroup.length}`);
}


function sendLanguageCommandToDevice(language, uniqueId) {

    // Send the message via WebSocket
    ws.send(language);
    console.log(`Sent language change command: ${language} to ${uniqueId}`);
}


function addRowSelectionListener() {
    const rows = document.querySelectorAll('#chairName .device');
    rows.forEach(row => {
        row.addEventListener('click', function() {
            // First, remove 'selected' class from all rows if needed
            rows.forEach(r => r.classList.remove('selected'));

            // Add 'selected' class to this row
            this.classList.add('selected');

            // Hide all play/stop buttons
            document.querySelectorAll('.play-button, .stop-button').forEach(button => {
                button.classList.add('hidden');
            });

            // Show play/stop buttons for the clicked row only
            this.querySelectorAll('.play-button, .stop-button').forEach(button => {
                button.classList.remove('hidden');
            });
        });
    });
}



function allowDrop(event) {
    event.preventDefault();
}

function handleDrop(event) {
    event.preventDefault();
    const data = event.dataTransfer.getData("text/plain");
    const chair = document.getElementById(data);
    if (chair) {
        const group = event.target.closest('.group');
        const groupId = group.id.replace('group', ''); // Extract the group number
        group.querySelector('.devices').appendChild(chair);
        const selectedLanguage = document.querySelector(`select[data-group-id="${groupId}"]`).value;
        
        // Call updateChairLanguage with chair's unique ID
        const uniqueId = chair.getAttribute('data-unique-id') || chair.id.replace('device-', ''); // Ensure you have a way to get uniqueId
        updateChairLanguage(chair, selectedLanguage, uniqueId); // Now passing uniqueId as well
    }
}


function updateChairLanguage(chairElement, language, uniqueId) {
    const languageSpan = chairElement.querySelector('.language');
    if (languageSpan) {
        languageSpan.textContent = language; // Update the display language
    }
    
    // Send the language change command via WebSocket
    sendLanguageCommandToDevice(language.toLowerCase(), uniqueId); // Reuse the function you already have
}

// I need to update it in the device

function createOrUpdateDeviceElement(device) {
    const unassignedDevicesContainer = document.getElementById('chairName'); // Ensure this is the correct container ID
    let deviceElement = document.getElementById(`device-${device.uniqueId}`);

    if (!deviceElement) {
        deviceElement = document.createElement('div');
        deviceElement.id = `device-${device.uniqueId}`;
        deviceElement.className = 'device bg-gray-100 p-2 rounded-lg flex justify-between items-center mb-2 cursor-pointer hover:bg-gray-200';
        deviceElement.setAttribute('draggable', 'true');
        
        // Create the inner HTML structure similar to the group placeholder
        const deviceHTML = `
                <span class="font-medium">${device.deviceName}</span>
                <span class="language ml-2">${device.language}</span>
                <span class="battery">${device.battery}%</span>
                <span class="connection-status ml-2">${getConnectionStatusHTML(device.isConnected)}</span>
                <div>
                    <button class="play-button hidden bg-green-500 hover:bg-green-700 text-white font-bold py-1 px-2 rounded text-xs mr-1">Play</button>
                    <button class="stop-button hidden bg-green-500 hover:bg-green-700 text-white font-bold py-1 px-2 rounded text-xs">Stop</button>
                </div>        
            `;

        deviceElement.innerHTML = deviceHTML;
        unassignedDevicesContainer.appendChild(deviceElement);

        // Add event listener for toggling visibility of play and stop buttons
        deviceElement.addEventListener('click', function() {
            const playButton = this.querySelector('.play-button');
            const stopButton = this.querySelector('.stop-button');
            playButton.classList.toggle('hidden');
            stopButton.classList.toggle('hidden');
        });

        // Assuming addEventListenersToDevice function initializes drag events
        addEventListenersToDevice(deviceElement);
    } else {
        // Only update dynamic parts (battery and connection status) for existing elements
        const batterySpan = deviceElement.querySelector('.battery');
    const connectionStatusSpan = deviceElement.querySelector('.connection-status');
    
    batterySpan.textContent = `${device.battery}%`;
    connectionStatusSpan.innerHTML = getConnectionStatusHTML(device.isConnected);
        updateDeviceElementContent(deviceElement, device);

    }
}























































// This function listens for changes on any dropdown with the 'language-dropdown' class and updates the languages accordingly
function addEventListenersToLanguageDropdowns() {
    document.querySelectorAll('.language-dropdown').forEach(dropdown => {
        dropdown.addEventListener('change', function() {
            const groupId = this.getAttribute('data-group-id');
            const newLanguage = this.value;
            updateGroupLanguages(groupId, newLanguage);
        });
    });
}

function updateGroupLanguages(groupId, newLanguage) {
    
    let devicesInGroup;
    
    // Adjust the selector if your groups are named differently or just use IDs
    devicesInGroup = document.querySelectorAll(`#group${groupId} .device .language`);

    devicesInGroup.forEach(languageSpan => {
        languageSpan.textContent = newLanguage;
    });

    // Log the result
    console.log(`Language for Group ${groupId} updated to ${newLanguage}. Total devices updated: ${devicesInGroup.length}`);

}





















function handleDrop(event) {
    event.preventDefault();
    const data = event.dataTransfer.getData("text/plain");
    const chair = document.getElementById(data);
    if (chair) {
        const group = event.target.closest('.group');
        const groupId = group.id.replace('group', ''); // Extract the group number
        group.querySelector('.devices').appendChild(chair);
        const selectedLanguage = document.querySelector(`select[data-group-id="${groupId}"]`).value;
        
        // Call updateChairLanguage with chair's unique ID
        const uniqueId = chair.getAttribute('data-unique-id') || chair.id.replace('device-', ''); // Ensure you have a way to get uniqueId
        updateChairLanguage(chair, selectedLanguage, uniqueId); // Now passing uniqueId as well
    }
}


function updateChairLanguage(chairElement, language, uniqueId) {
    const languageSpan = chairElement.querySelector('.language');
    if (languageSpan) {
        languageSpan.textContent = language; // Update the display language
    }
    
    // Send the language change command via WebSocket
    sendLanguageCommandToDevice(language.toLowerCase(), uniqueId); // Reuse the function you already have
}





















// Initialize WebSocket connection and set up event handlers
const ws = new WebSocket('ws://192.168.1.103:5500/');
const devices = new Map();
const timeout = 2000;

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    if (data.deviceName && data.uniqueId) {
        // Update the device info and the timestamp
        devices.set(data.uniqueId, {...data, lastUpdate: Date.now(), isConnected: true});
        // Call function to create or update device element
        createOrUpdateDeviceElement(devices.get(data.uniqueId));
    }
};

// Periodically check for disconnected devices
setInterval(function() {
    const currentTime = Date.now();
    devices.forEach((device, uniqueId) => {
        if (currentTime - device.lastUpdate > timeout && device.isConnected) {
            // Mark as disconnected and update the UI
            device.isConnected = false;
            createOrUpdateDeviceElement(device);
        }
    });
}, 1000);



function createOrUpdateDeviceElement(device) {
    const unassignedDevicesContainer = document.getElementById('chairName'); // Ensure this is the correct container ID
    let deviceElement = document.getElementById(`device-${device.uniqueId}`);

    if (!deviceElement) {
        deviceElement = document.createElement('div');
        deviceElement.id = `device-${device.uniqueId}`;
        deviceElement.className = 'device bg-gray-100 p-2 rounded-lg flex justify-between items-center mb-2 cursor-pointer hover:bg-gray-200';
        deviceElement.setAttribute('draggable', 'true');
        
        // Create the inner HTML structure similar to the group placeholder
        const deviceHTML = `
                <span class="font-medium">${device.deviceName}</span>
                <span class="language ml-2">${device.language || 'Nederlands'}</span> <!-- Assuming language can be dynamic; adjust as needed -->
                <span class="battery">${device.battery}%</span>
                <span class="connection-status ml-2">${getConnectionStatusHTML(device.isConnected)}</span>
                <div>
                    <button class="play-button hidden bg-green-500 hover:bg-green-700 text-white font-bold py-1 px-2 rounded text-xs mr-1">Play</button>
                    <button class="stop-button hidden bg-green-500 hover:bg-green-700 text-white font-bold py-1 px-2 rounded text-xs">Stop</button>
                </div>        
            `;


        deviceElement.innerHTML = deviceHTML;
        unassignedDevicesContainer.appendChild(deviceElement);

        // Add event listener for toggling visibility of play and stop buttons
        deviceElement.addEventListener('click', function() {
            const playButton = this.querySelector('.play-button');
            const stopButton = this.querySelector('.stop-button');
            playButton.classList.toggle('hidden');
            stopButton.classList.toggle('hidden');
        });

        // Assuming addEventListenersToDevice function initializes drag events
        addEventListenersToDevice(deviceElement);
    } else {
        // Only update dynamic parts (battery and connection status) for existing elements
        const batterySpan = deviceElement.querySelector('.battery');
    const connectionStatusSpan = deviceElement.querySelector('.connection-status');
    
    batterySpan.textContent = `${device.battery}%`;
    connectionStatusSpan.innerHTML = getConnectionStatusHTML(device.isConnected);
        updateDeviceElementContent(deviceElement, device);

    }
}

function getConnectionStatusHTML(isConnected) {
    return isConnected
        ? '<span class="inline-block rounded-full h-3 w-3 bg-green-500"></span> Connected'
        : '<span class="inline-block rounded-full h-3 w-3 bg-red-500"></span> Disconnected';
}































    // Establish the WebSocket connection
    const ws = new WebSocket('ws://172.20.10.2:5500/');
    //const ws = new WebSocket('ws://172.28.0.15:5500/');
    
    
    const devices = new Map();
    const timeout = 2000;
    
    
    ws.onmessage = function(event) {
        const data = JSON.parse(event.data);
        if (data.deviceName && data.uniqueId) {
            // Update the device info and the timestamp
            devices.set(data.uniqueId, {...data, lastUpdate: Date.now(), isConnected: true});
            updateDeviceCount();
            updateDeviceTable();
        }
    };
    
    // Function to check for disconnected devices
    function checkForDisconnectedDevices() {
        const currentTime = Date.now();
        devices.forEach((device, uniqueId) => {
            if (currentTime - device.lastUpdate > timeout) {
                // Set the device status to disconnected
                device.isConnected = false;
                devices.set(uniqueId, device);
                updateDeviceTable();
            }
        });
    }
    
    // Periodically check for disconnected devices
    setInterval(checkForDisconnectedDevices, 1000); // Check every second
    
    // Update the device count on the button
    function updateDeviceCount() {
        const deviceCountSpan = document.getElementById('deviceCount');
        deviceCountSpan.textContent = devices.size;
    }
    
    // Function to update the device table
    function updateDeviceTable() {
        const tableBody = document.getElementById('deviceTableBody');
    
        devices.forEach((device, uniqueId) => {
            let row = document.getElementById(`device-row-${uniqueId}`);
            if (!row) {
                // If row doesn't exist, add new device row
                addDeviceToTable(device, uniqueId);
            } else {
                // Update only the necessary cells in the existing row
                updateDeviceRow(row, device);
            }
        });
    }
    
    // Function to update an individual device row
    function updateDeviceRow(row, device) {
        // Update specific cells (like battery, connection status) as needed
        row.querySelector('.battery-cell').textContent = `${device.battery}%`;
        row.querySelector('.connection-status-cell').innerHTML = getConnectionStatusHTML(device.isConnected);
        // Note: Add class names to your existing HTML for easy access (e.g., 'battery-cell', 'connection-status-cell')
    }
    
    // Function to send the selected language to the Unity app
    function updateVideoPath(selectedLanguage, uniqueId) {
        const message = JSON.stringify({
            action: "changeLanguage",
            language: selectedLanguage,
            uniqueId: uniqueId
        });
        ws.send(message);
    }
    
    // Function to generate the connection status HTML
    function getConnectionStatusHTML(isConnected) {
        return isConnected
            ? '<span class="inline-block rounded-full h-3 w-3 bg-green-500"></span> Connected'
            : '<span class="inline-block rounded-full h-3 w-3 bg-red-500"></span> Disconnected';
    }
    
     // Function to add a device to the table
     function addDeviceToTable(device, uniqueId) {
        const tableBody = document.getElementById('deviceTableBody');
        const numRows = tableBody.rows.length; // Get the current number of rows
    
        const row = document.createElement('tr');
        row.id = `device-row-${uniqueId}`; // Assign a unique ID to the row
        row.className = (numRows % 2 === 0) ? 'bg-gray-100' : 'bg-white';
    
        row.innerHTML = `
            <td class="px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-900">${device.deviceName}</td>
            <td class="battery-cell text-sm font-light px-6 py-4 whitespace-nowrap text-gray-900">${device.battery}%</td>
            <td class="connection-status-cell text-sm font-light px-6 py-4 whitespace-nowrap text-gray-900">${getConnectionStatusHTML(device.isConnected)}</td>
            <td class="text-sm font-light px-6 py-4 whitespace-nowrap text-gray-900">
                <select id="languageDropdown${uniqueId}">
                    <option value="nederlands">Nederlands</option>
                    <option value="english">English</option>mee
                    <option value="french">French</option>
                    <option value="german">German</option>
                    <option value="hungarian">Hungarian</option>
                    <option value="italian">Italian</option>
                </select>
            </td>
        `;
    
        tableBody.appendChild(row);
    
        // Get the language dropdown for this device
        const languageDropdown = document.getElementById(`languageDropdown${uniqueId}`);
    
        // Add an event listener to the language dropdown
        languageDropdown.addEventListener('change', function(event) {
            const selectedLanguage = event.target.value;
            const message = `${selectedLanguage}`; // Include uniqueId in the message
            ws.send(message);
            console.log("Sent message:" + message); // Log for debugging
        });
    }
    