using System;
using UnityEngine;
using UnityEngine.Video;
using Websocket.Client;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Concurrent; // Add this line


public class WebSocketAndVideoControl : MonoBehaviour
{
    public Material waitingRoomMaterial; // Material for "360 image"
    public Material videoMaterial;       // Material for "360 video"
    public VideoPlayer videoPlayer;      // Reference to the VideoPlayer

    // video path of the language
    public string videoPathEnglish;
    public string videoPathDutch;
    public string videoPathFrench;
    public string videoPathGerman;
    public string videoPathHungarian;
    public string videoPathItalian;


    private WebsocketClient websocketClient;
    private bool isVideoPlaying = false;
    private bool playReceived = false;
    private bool pauseReceived = false;
    private bool restartReceived = false;
    private bool stopReceived = false;
    private bool playRequested = false; // Flag to indicate a play request

    private bool videoPrepared = false;


    private bool nederlands = false;
    private bool english = false;
    private bool french = false;
    private bool german = false;
    private bool hungarian = false;
    private bool italian = false;


    private string currentLanguage = "nederlands";

    private DeviceInfo currentDeviceInfo; // Declare at the class level
    private ConcurrentQueue<CommandMessage> commandQueue = new ConcurrentQueue<CommandMessage>();
    private string deviceUniqueId;


    public class DeviceInfo
    {
        public float battery;
        public string deviceName;
        public string uniqueId;
    }

    public class CommandMessage
    {
        public string uniqueId;
        public string command;
    }

    void Start()
    {
        currentDeviceInfo = new DeviceInfo();
        InitializeWebSocket();
        // Set the default video path to Nederlands at start
        UpdateVideoPathBasedOnLanguage(currentLanguage);
        videoPlayer.Prepare();
        videoPlayer.prepareCompleted += OnVideoPrepared; // Add this line
        videoPlayer.loopPointReached += OnVideoFinished;

        deviceUniqueId = SystemInfo.deviceUniqueIdentifier;

    }

    // /sdcard/Spacebuzz/Space_VR_3D_english.mp4
    // /sdcard/Spacebuzz/Space_VR_3D_nederlands.mp4
    // /sdcard/Spacebuzz/Space_VR_3D_french.mp4
    // /sdcard/Spacebuzz/Space_VR_3D_german.mp4
    // /sdcard/Spacebuzz/Space_VR_3D_hungarian.mp4
    // /sdcard/Spacebuzz/Space_VR_3D_italian.mp4
    // C:/Users/31653/Downloads/Space_VR_3D_nederlands.mp4

    void InitializeWebSocket()
    {
        // wifi var url = new Uri("ws://172.28.0.10:5500/");

        // hostpot daan phone
        var url = new Uri("ws://10.42.1.200:5500/");

        websocketClient = new WebsocketClient(url)
        {
            ReconnectTimeout = TimeSpan.FromSeconds(30),
        };

        websocketClient.ReconnectionHappened.Subscribe(info =>
            Debug.Log($"Reconnection happened, type: {info.Type}"));

        websocketClient.MessageReceived.Subscribe(msg =>
        {
            Debug.Log($"Message received: {msg}");
            HandleWebSocketMessage(msg.Text);
        });

        websocketClient.Start();
        StartCoroutine(SendPeriodicUpdates());
    }

    void HandleWebSocketMessage(string message)
    {
        Debug.Log("Received message: " + message);

        try
        {
            CommandMessage receivedMessage = JsonUtility.FromJson<CommandMessage>(message);

            if (receivedMessage.uniqueId == deviceUniqueId)
            {

                // The command is intended for this device.
                switch (receivedMessage.command.ToLower())
                {
                    case "play":
                        playReceived = true;
                        break;
                    case "pause":
                        pauseReceived = true;
                        break;
                    case "restart":
                        restartReceived = true;
                        break;
                    case "stop":
                        stopReceived = true;
                        break;
                    case "nederlands":
                        nederlands = true;
                        break;
                    case "english":
                        english = true;
                        break;
                    case "french":
                        french = true;
                        break;
                    case "german":
                        german = true;
                        break;
                    case "hungarian":
                        hungarian = true;
                        break;
                    case "italian":
                        italian = true;
                        break;
                    default:
                        Debug.LogWarning($"Unknown command: {receivedMessage.command}");
                        break;
                }
            }
            else
            {
                Debug.Log("Command not intended for this device.");
            }
        }
        catch (Exception ex)
        {
            Debug.LogError($"Error parsing JSON message: {ex.Message}");
        }
    }


    void ProcessCommand(CommandMessage commandMessage)
    {
        // Ensure the command is intended for this device
        if (commandMessage.uniqueId != SystemInfo.deviceUniqueIdentifier) return;

        switch (commandMessage.command.ToLower())
        {
            case "play":
                playReceived = true;
                break;
            case "pause":
                pauseReceived = true;
                break;
            case "restart":
                restartReceived = true;
                break;
            case "stop":
                stopReceived = true;
                break;
            case "nederlands":
                nederlands = true;
                break;
            case "english":
                english = true;
                break;
            case "french":
                french = true;
                break;
            case "german":
                german = true;
                break;
            case "hungarian":
                hungarian = true;
                break;
            case "italian":
                italian = true;
                break;
        }
    }


    void UpdateVideoPathBasedOnLanguage(string language)
    {
        string newUrl = videoPathDutch; // Default to Dutch

        switch (language)
        {
            case "nederlands":
                newUrl = videoPathDutch;
                break;
            case "english":
                newUrl = videoPathEnglish;
                break;
            case "french":
                newUrl = videoPathFrench;
                break;
            case "german":
                newUrl = videoPathGerman;
                break;
            case "hungarian":
                newUrl = videoPathHungarian;
                break;
            case "italian":
                newUrl = videoPathItalian;
                break;
        }

        Debug.Log($"Video path set to {language}: {newUrl}");

        if (isVideoPlaying)
        {
            videoPlayer.Stop(); // Stop the current video if it's playing
            isVideoPlaying = false;
        }

        videoPlayer.url = newUrl;
        videoPlayer.Prepare();
    }




    IEnumerator SendPeriodicUpdates()
    {
        while (true)
        {
            yield return new WaitForSeconds(1);

            if (currentDeviceInfo != null && websocketClient.IsRunning)
            {
                currentDeviceInfo.battery = Mathf.RoundToInt(SystemInfo.batteryLevel * 100);
                currentDeviceInfo.deviceName = SystemInfo.deviceName;
                currentDeviceInfo.uniqueId = SystemInfo.deviceUniqueIdentifier;

                string deviceInfoJson = JsonUtility.ToJson(currentDeviceInfo);
                Debug.Log("Sending Device Info: " + deviceInfoJson);

                websocketClient.Send(deviceInfoJson);
            }
        }
    }

    private void OnVideoPrepared(VideoPlayer source)
    {
        // Unsubscribe to avoid potential repeated calls
        videoPlayer.prepareCompleted -= OnVideoPrepared;

        if (playRequested)
        {
            // Play the video after it's prepared if a play was requested
            videoPlayer.Play();
            isVideoPlaying = true;
            playRequested = false; // Reset the flag after playing
            Debug.Log("Video prepared and is now playing.");
        }

        // Even if not playing immediately, the video is now ready for future play commands
    }

    private void OnVideoFinished(VideoPlayer source)
    {
        Debug.Log("Video finished playing.");
        StopVideo(); // Automatically go back to the waiting room when video finishes
    }

    private void PlayVideo()
    {
        Debug.Log("PlayVideo called");

        // Ensure the video player is directed to the current video material
        RenderSettings.skybox = videoMaterial;

        if (!isVideoPlaying)
        {
            if (videoPlayer.isPrepared)
            {
                // Play the video if it is already prepared
                videoPlayer.Play();
                isVideoPlaying = true;
                playRequested = false; // Reset the flag as the play action is fulfilled
                Debug.Log("Video is playing.");
            }
            else
            {
                // Prepare the video if it's not prepared; preparation is asynchronous
                playRequested = true; // Set the flag to indicate play was requested
                videoPlayer.Prepare();
                // Listen for the prepareCompleted event to know when the video is ready to play
                videoPlayer.prepareCompleted += OnVideoPrepared;
                Debug.Log("Preparing video...");
            }
        }
    }



    public void PauseVideo()
    {
        // If the video is currently playing, pause it.
    if (isVideoPlaying)
    {
        videoPlayer.Pause();
        isVideoPlaying = false;
        Debug.Log("Video paused.");
    }
    else
    {
        // If the video is not playing, check if it's prepared before playing.
        if (videoPlayer.isPrepared)
        {
            videoPlayer.Play();
            isVideoPlaying = true;
            Debug.Log("Video playing.");
        }
        else
        {
            // If the video is not prepared, prepare it, then play.
            videoPlayer.Prepare();
            videoPlayer.prepareCompleted += (source) => 
            {
                videoPlayer.Play();
                isVideoPlaying = true;
                Debug.Log("Video prepared and now playing.");
            };
        }
    }
    }

    //C:/Users/31653/Downloads/video.mp4

    public void RestartVideo()
    {
        videoPlayer.Stop();
        videoPlayer.Prepare();
        videoPlayer.Play();
        isVideoPlaying = true;
    }

    private void StopVideo()
    {
        Debug.Log("StopVideo called");

        if (isVideoPlaying)
        {
            // Stop the video
            videoPlayer.Stop();
            isVideoPlaying = false;
            Debug.Log("Video stopped.");

            // Optionally, you might want to prepare the video again for the next play
            videoPlayer.Prepare();

            // Switch back to the waiting room material
            RenderSettings.skybox = waitingRoomMaterial;
            Debug.Log("Switched to waiting room.");
        }
    }


    void OnDestroy()
    {
        websocketClient?.Dispose();
    }


    void Update()
    {
        while (commandQueue.TryDequeue(out CommandMessage command))
        {
            // Process the command
            ProcessCommand(command);
        }

        // Existing WebSocket message handling
        if (playReceived) { PlayVideo(); playReceived = false; }
        if (pauseReceived) { PauseVideo(); pauseReceived = false; }
        if (restartReceived) { RestartVideo(); restartReceived = false; }
        if (stopReceived) { StopVideo(); stopReceived = false; }

        // Handle language changes based on WebSocket messages
        if (english) { UpdateVideoPathBasedOnLanguage("english"); english = false; }
        if (nederlands) { UpdateVideoPathBasedOnLanguage("nederlands"); nederlands = false; }
        if (french) { UpdateVideoPathBasedOnLanguage("french"); french = false; }
        if (german) { UpdateVideoPathBasedOnLanguage("german"); german = false; }
        if (hungarian) { UpdateVideoPathBasedOnLanguage("hungarian"); hungarian = false; }
        if (italian) { UpdateVideoPathBasedOnLanguage("italian"); italian = false; }

        // Existing Keyboard controls for play, pause, restart, and stop
        if (Input.GetKeyDown(KeyCode.P)) { PlayVideo(); Debug.Log("Play Video (Keyboard)"); }
        if (Input.GetKeyDown(KeyCode.O)) { PauseVideo(); Debug.Log("Pause Video (Keyboard)"); }
        if (Input.GetKeyDown(KeyCode.R)) { RestartVideo(); Debug.Log("Restart Video (Keyboard)"); }
        if (Input.GetKeyDown(KeyCode.S)) { StopVideo(); Debug.Log("Stop Video (Keyboard)"); }

        // Additional keyboard controls for changing language
        if (Input.GetKeyDown(KeyCode.E)) { UpdateVideoPathBasedOnLanguage("english"); Debug.Log("Switched to English"); }
        if (Input.GetKeyDown(KeyCode.D)) { UpdateVideoPathBasedOnLanguage("nederlands"); Debug.Log("Switched to Dutch"); }
        // Assign new keys for the additional languages
        if (Input.GetKeyDown(KeyCode.F)) { UpdateVideoPathBasedOnLanguage("french"); Debug.Log("Switched to French"); }
        if (Input.GetKeyDown(KeyCode.G)) { UpdateVideoPathBasedOnLanguage("german"); Debug.Log("Switched to German"); }
        if (Input.GetKeyDown(KeyCode.H)) { UpdateVideoPathBasedOnLanguage("hungarian"); Debug.Log("Switched to Hungarian"); }
        if (Input.GetKeyDown(KeyCode.I)) { UpdateVideoPathBasedOnLanguage("italian"); Debug.Log("Switched to Italian"); }
    }




}