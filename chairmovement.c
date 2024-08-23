using System;
using System.Collections.Generic;
using System.Globalization;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Diagnostics;
using WebSocketSharp;
public class MotionHardwareController
{

    private WebSocket ws;

    private List<Timer> movementTimers = new List<Timer>();
    private CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();



    // Constants based on the Unity script
    private const byte PacketIdentifier = 0xAA;
    private const byte TargetDeviceId = 0x01;
    private const byte SetRumbleIntensityCommand = 0x08;
    private const byte MoveToAtSpeedDualCommand = 0x05;
    private const int MaxIntensity = 0xFF;
    private const uint YawMax = 9920; // These values might need to be configurable
    private const uint PitchMax = 10000;
    private const uint YawNeutral = 3500;
    private const uint PitchNeutral = 3840;
    private const byte SpeedMax = 255;
    // Network fields
    private readonly UdpClient udpClient;
    private readonly IPEndPoint endPoint;

    public MotionHardwareController(string ipAddress, int port, string wsUri)
    {
        // Initialize UDP client
        endPoint = new IPEndPoint(IPAddress.Parse(ipAddress), port);
        udpClient = new UdpClient();
        udpClient.Connect(endPoint);

        // Initialize WebSocket
        ws = new WebSocket(wsUri);
        ws.OnMessage += OnWebSocketMessage;
        ws.Connect();
    }



    private static float Clamp(float value, float min, float max)
    {
        return (value < min) ? min : (value > max) ? max : value;
    }
    // Methods for sending different packets, based on Unity code
    public void SendRumbleIntensity(float intensity)
    {
        uint mappedIntensity = (uint)(Clamp(intensity, 0.0f, 1.0f) * MaxIntensity);
        byte[] packet = {
            PacketIdentifier,
            TargetDeviceId,
            SetRumbleIntensityCommand,
            (byte)mappedIntensity
        };
        SendMessageToHardware(packet);
    }


    private void OnWebSocketMessage(object sender, MessageEventArgs e)
    {
        Console.WriteLine($"Received WebSocket message: {e.Data}");
        switch (e.Data)
        {
            case "play":
                SetUpTimedMovements(this);
                break;
            case "stop":
                StopAllMovements();
                break;
        }
    }


    public void MoveChair(float normalizedYawTarget, float normalizedPitchTarget, float normalizedYawSpeed, float normalizedPitchSpeed)
    {
        // Map and clamp the values based on Unity code
        uint clampedYawTarget = MapAndClamp(normalizedYawTarget, YawNeutral, YawMax);
        uint clampedPitchTarget = MapAndClamp(normalizedPitchTarget, PitchNeutral, PitchMax);
        // Adjust speed calculation here
        byte clampedYawSpeed = (byte)(Clamp(normalizedYawSpeed, 0.0f, 1.0f) * SpeedMax * 0.6f); // 60% of max speed
        byte clampedPitchSpeed = (byte)(Clamp(normalizedPitchSpeed, 0.0f, 1.0f) * SpeedMax * 0.6f); // 60% of max speed
        byte[] packet = {
            PacketIdentifier,
            TargetDeviceId,
            MoveToAtSpeedDualCommand,
            // Packet structure based on Unity code
            (byte)((clampedYawTarget >> 16) & 0xFF),
            (byte)((clampedYawTarget >> 8) & 0xFF),
            (byte)((clampedYawTarget) & 0xFF),
            (byte)((uint)clampedYawSpeed << 1),
            (byte)((clampedPitchTarget >> 16) & 0xFF),
            (byte)((clampedPitchTarget >> 8) & 0xFF),
            (byte)((clampedPitchTarget) & 0xFF),
            (byte)((uint)clampedPitchSpeed << 1)
        };
        // Add logic to split uint into bytes and append to the packet
        SendMessageToHardware(packet);
    }
    // Add try-catch in SendMessageToHardware
    private void SendMessageToHardware(byte[] packet)
    {
        try
        {
            udpClient.Send(packet, packet.Length);
            Console.WriteLine("Packet sent successfully.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error sending packet: {ex.Message}");
        }
    }
    private static uint MapAndClamp(float value, uint neutral, uint max)
    {
        float mappedValue = (value * (max - neutral) + neutral);
        return (uint)Clamp(mappedValue, 0, max);
    }


    public void StopAllMovements()
    {
        foreach (var timer in movementTimers) // Use 'movementTimers' here
        {
            timer.Change(Timeout.Infinite, Timeout.Infinite);
            timer.Dispose();
        }
        movementTimers.Clear(); // Use 'movementTimers' here
        Console.WriteLine("Timed movements stopped.");
        MoveChair(0, 0, 0, 0); // Sets both yaw and pitch to neutral with zero speed
    }

    public void CenterChair()
    {
        Console.WriteLine("Center all chair movements...");
        MoveChair(0, 0, 0.3f, 0.1f); // Sets both yaw and pitch to neutral with zero speed
    }
    public void TiltBackwards()
    {
        Console.WriteLine("Starting to tilt chair backwards...");
        MoveChair(0, -1, 0f, 0.1f); // Move backward
    }
    public void TurnLeft()
    {
        Console.WriteLine("Starting to turn chair left...");
        MoveChair(-1, 0, 0.3f, 0f);
    }
    // Main method to run the application
    public static void Main(string[] args)
    {
        MotionHardwareController controller = new MotionHardwareController("192.168.20.20", 7999, "ws://10.42.1.200:5500");
        Console.WriteLine("Chair control started. Press 'T' to start timed movements, 'Q' to quit.");
        bool timedMovementsSetUp = false;
        // Continuously listen for keyboard input
        while (true)
        {
            if (Console.KeyAvailable)
            {
                var key = Console.ReadKey(intercept: true).Key;
                switch (key)
                {
                    case ConsoleKey.C:
                        controller.CenterChair();
                        break;
                    case ConsoleKey.W:
                        controller.TiltBackwards();
                        break;
                    case ConsoleKey.D:
                        controller.TurnLeft();
                        break;
                    case ConsoleKey.T:
                        if (!timedMovementsSetUp)
                        {
                            SetUpTimedMovements(controller);
                            timedMovementsSetUp = true;
                            Console.WriteLine("Timed movements have been set up.");
                        }
                        break;
                    case ConsoleKey.Q:
                        controller.StopAllMovements();
                        Console.WriteLine("Exiting application.");
                        return;
                }
            }
        }
    }
    private static void SetUpTimedMovements(MotionHardwareController controller)
    {
        try
        {
            Console.WriteLine("Setting up timed movements...");

            controller.movementTimers.Clear();

            var cancellationToken = controller.cancellationTokenSource.Token;


            // Define the times for each action
            var rotationLeftTime1 = TimeSpan.FromSeconds(0);

            var centerTime1 = TimeSpan.FromSeconds(25).Add(TimeSpan.FromMilliseconds(25));
            var tiltBackwardTime = TimeSpan.FromSeconds(36).Add(TimeSpan.FromMilliseconds(8));
            var centerTime2 = TimeSpan.FromMinutes(1).Add(TimeSpan.FromSeconds(23)).Add(TimeSpan.FromMilliseconds(10));
            var leftTime = TimeSpan.FromMinutes(2).Add(TimeSpan.FromSeconds(17)).Add(TimeSpan.FromMilliseconds(15));
            var centerTime3 = TimeSpan.FromMinutes(12).Add(TimeSpan.FromSeconds(34)).Add(TimeSpan.FromMilliseconds(5));
            var tiltBackwardTime2 = TimeSpan.FromMinutes(13).Add(TimeSpan.FromSeconds(29)).Add(TimeSpan.FromMilliseconds(10));
            var centerTime4 = TimeSpan.FromMinutes(14).Add(TimeSpan.FromSeconds(24)).Add(TimeSpan.FromMilliseconds(25));
            var leftTime2 = TimeSpan.FromMinutes(14).Add(TimeSpan.FromSeconds(40)).Add(TimeSpan.FromMilliseconds(13));

            // Schedule each action
            controller.movementTimers.Add(SetTimer(() => controller.TurnLeft(), rotationLeftTime1, cancellationToken));
            //controller.movementTimers.Add(SetTimer(() => controller.CenterChair(), centerTime1, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, 0, CalculateSpeedForDuration(3.2f, YawMax, YawNeutral), 0), centerTime1, cancellationToken));

            //controller.movementTimers.Add(SetTimer(() => controller.TiltBackwards(), tiltBackwardTime, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, -1, 0, CalculateSpeedForDuration(12.08f, PitchMax, PitchNeutral)), tiltBackwardTime, cancellationToken));

            //controller.movementTimers.Add(SetTimer(() => controller.CenterChair(), centerTime2, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, 0, CalculateSpeedForDuration(20.72f, YawMax, YawNeutral), 0), centerTime2, cancellationToken));

            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(-1, 0, CalculateSpeedForDuration(3.4f, YawMax, YawNeutral), 0), leftTime, cancellationToken));

            //controller.movementTimers.Add(SetTimer(() => controller.CenterChair(), centerTime3, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, 0, CalculateSpeedForDuration(3.4f, YawMax, YawNeutral), 0), centerTime3, cancellationToken));

            //controller.movementTimers.Add(SetTimer(() => controller.TiltBackwards(), tiltBackwardTime2, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, -1, CalculateSpeedForDuration(51.4f, PitchMax, PitchNeutral), 0), tiltBackwardTime2, cancellationToken));

            //controller.movementTimers.Add(SetTimer(() => controller.CenterChair(), centerTime4, cancellationToken));
            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(0, 0, CalculateSpeedForDuration(6.32f, YawMax, YawNeutral), 0), centerTime4, cancellationToken));

            controller.movementTimers.Add(SetTimer(() => controller.MoveChair(-1, 0, CalculateSpeedForDuration(3.4f, YawMax, YawNeutral), 0), leftTime2, cancellationToken));


            Console.WriteLine("Timed movements have been set up.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error in SetUpTimedMovements: {ex.Message}");
        }
    }

    private static Timer SetTimer(Action action, TimeSpan delay, CancellationToken cancellationToken)
    {
        var timer = new Timer(_ =>
        {
            if (!cancellationToken.IsCancellationRequested)
            {
                action();
            }
        }, null, delay, Timeout.InfiniteTimeSpan);

        return timer;
    }

    private static float CalculateSpeedForDuration(float seconds, uint max, uint neutral)
    {
        // Calculate the distance to cover
        var distance = max - neutral;
        // Calculate speed based on time and distance
        var speed = distance / (max * seconds); // Normalize speed to a value between 0.0f and 1.0f
        return speed;
    }

}