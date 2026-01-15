# Expected Runtime Output

This document shows the expected console output when running the Vision-Based RC Car Control System.

## Startup Sequence

```
========================================
Vision-Based RC Car Control System
========================================
Config file: config/config.json
Mode: Autonomous
UI: Enabled
========================================

Loading configuration...
Configuration loaded successfully

Initializing camera...
Camera initialized: 1920x1080 @ 30 FPS

Connecting to BLE device: f9:af:3c:e2:d2:f5
Connecting to: DR!FT Car [f9:af:3c:e2:d2:f5]
Successfully connected, listing services...
Service: 6e400001-b5a3-f393-e0a9-e50e24dcca9e
    Characteristic: 6e400002-b5a3-f393-e0a9-e50e24dcca9e
    Characteristic: 6e400003-b5a3-f393-e0a9-e50e24dcca9e
Successfully connected to BLE device

Control orchestrator initialized successfully

Select the object (car) to track in the window...
[OpenCV window "Select Object to Track" appears]
[User selects ROI by clicking and dragging]
Tracker initialized: CSRT

System started successfully

System running. Press Ctrl+C to stop.
Autonomous mode: ACTIVE
```

## Runtime Output (During Operation)

### Normal Operation

```
[Tracking window displays:]
  - Blue bounding box around tracked car
  - Green circle at midpoint
  - Yellow arrow showing movement vector
  - "TRACKING" text in green (top-left)

[Guidance window displays:]
  - Three red rays from car position:
    * Left ray: -60° angle
    * Center ray: 0° angle  
    * Right ray: +60° angle
  - Distance values displayed near ray endpoints
  - Speed: 10 L:0 R:0 (or current values)

[Console output every second:]
BLE Command: bf0a00082800000a00000000000000020000
BLE Command: bf0a00082800000a00000005000000020000
BLE Command: bf0a00082800000a0000000a000000020000
```

### When Car Approaches Boundary

```
[Guidance window shows:]
  - Ray distances decrease (e.g., 85, 120, 95 pixels)
  - Evasive action triggered when distance < 80px
  - Steering values change: L:128 R:0 (or L:0 R:128)

[Console shows steering adjustments:]
Speed: 10 L:128 R:0  [Steering left to avoid boundary]
```

### Tracking Loss Scenario

```
[Tracking window shows:]
  - "TRACKING LOST" text in red
  - Last known bounding box (if available)

[Console output:]
Warning: Tracking lost, sending stop command
BLE Command: bf0a00082800000000000000000000000000  [STOP]
```

## BLE Command Format Explanation

Commands are hexadecimal strings with format:
```
bf0a00082800  [Device identifier]
000a          [Speed: 10 in hex]
0000          [Drift: 0]
0000          [Steering: 0 = straight]
0200          [Lights: 0200 = on, 0000 = off]
00            [Checksum]
```

Example commands:
- **Forward, straight**: `bf0a00082800000a00000000000000020000`
- **Forward, right turn**: `bf0a00082800000a0000001e000000020000` (steering=30)
- **Stop**: `bf0a00082800000000000000000000000000`

## Performance Metrics (Expected)

When running on Raspberry Pi 4:
- **Frame Rate**: 15-30 FPS (depending on resolution)
- **Tracking Latency**: < 50ms per frame
- **Guidance Latency**: < 30ms per frame
- **BLE Command Rate**: ~200 Hz (5ms intervals)
- **End-to-End Latency**: < 100ms (camera → BLE send)
- **CPU Usage**: 60-80% (all cores)
- **Memory Usage**: < 1.5GB RAM

## Shutdown Sequence

```
^C
Received signal 2. Shutting down...
Stopping system...
BLE Command: bf0a00082800000000000000000000000000  [Emergency STOP]
System stopped
System shutdown complete.
```

## Error Scenarios

### Camera Not Found
```
Error: Could not open camera 0
Error: Failed to initialize camera
Error: Failed to initialize system
```

### BLE Connection Failed
```
Connecting to BLE device: f9:af:3c:e2:d2:f5
Failed to connect to BLE device
Warning: Failed to connect to BLE device. Continuing without BLE...
[System continues but no commands sent]
```

### Invalid ROI Selection
```
Select the object (car) to track in the window...
Error: Invalid ROI selected
Error: Failed to start system
```

### Tracking Loss Recovery
```
[Tracking lost for 5+ frames]
Warning: Tracking lost, attempting recovery...
[System reduces speed]
[If recovery fails after 10 seconds:]
Tracking lost for extended period, stopping system
```

## Debug Output (if enabled)

With debug logging:
```
[DEBUG] Frame captured: 1920x1080
[DEBUG] Tracking update: bbox=(100,200,50,30), midpoint=(125,215)
[DEBUG] Movement vector: dx=2, dy=1, magnitude=2.24, angle=26.57°
[DEBUG] Ray distances: left=150, center=200, right=180
[DEBUG] Control: speed=10, left=0, right=0
[DEBUG] BLE command sent: bf0a00082800000a00000000000000020000
```

## UI Windows

### Window 1: "Select Object to Track"
- First frame from camera
- User selects ROI
- Press SPACE or ENTER to confirm

### Window 2: "Tracking" (if UI enabled)
- Live camera feed
- Bounding box overlay
- Midpoint marker
- Movement vector arrow
- Status text

### Window 3: "Guidance" (if UI enabled)
- Live camera feed with boundary detection
- Three rays from car position
- Distance annotations
- Control values display

## Command Line Options

```bash
# Basic run
./VisionBasedRCCarControl

# Custom config
./VisionBasedRCCarControl -c /path/to/config.json

# Manual mode (no autonomous control)
./VisionBasedRCCarControl --manual

# Headless (no UI)
./VisionBasedRCCarControl --no-ui

# Help
./VisionBasedRCCarControl --help
```

## Expected Behavior Summary

1. **Startup**: Loads config, initializes camera, connects BLE
2. **ROI Selection**: User selects car to track
3. **Tracking**: Continuously tracks car and calculates movement
4. **Guidance**: Detects boundaries and generates control commands
5. **BLE**: Sends commands to car at high frequency
6. **Monitoring**: Displays tracking and guidance overlays
7. **Shutdown**: Sends stop command and exits cleanly
