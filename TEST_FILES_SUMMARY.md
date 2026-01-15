# Test Files Summary - Mac BLE Car Control

## 📁 Files Created for Mac Testing

### 1. Python Script (Recommended) ✅
**File**: `test_ble_python.py`
- **Purpose**: Full BLE connection and command sending
- **Status**: Ready to use
- **Requirements**: `pip3 install simplepyble`

**Usage**:
```bash
python3 test_ble_python.py
```

**Features**:
- ✅ Scans for BLE devices
- ✅ Connects to RC car
- ✅ Sends start/stop commands
- ✅ Interactive mode for testing
- ✅ Automatic stop on exit

### 2. C++ Test Program ✅
**File**: `test_ble_mac.cpp`
- **Purpose**: Command generation and testing (no actual BLE sending yet)
- **Status**: Compiles successfully
- **Requirements**: C++17 compiler (clang++/g++)

**Usage**:
```bash
make -f Makefile.test
./test_ble_mac
```

**Features**:
- ✅ Generates correct command format
- ✅ Shows command breakdown
- ✅ Interactive command testing
- ⚠️ BLE sending needs library integration

### 3. Makefile ✅
**File**: `Makefile.test`
- **Purpose**: Easy compilation of C++ test program
- **Usage**: `make -f Makefile.test`

### 4. Documentation ✅
- **`QUICK_START_MAC.md`**: Fast setup guide
- **`MAC_SETUP_INSTRUCTIONS.md`**: Detailed instructions and troubleshooting

## 🚀 Quick Start (Choose One)

### Option 1: Python (Easiest - Recommended)
```bash
# 1. Install
pip3 install simplepyble

# 2. Run
python3 test_ble_python.py

# 3. Use commands
> s    # Start
> t    # Stop
> q    # Quit
```

### Option 2: C++ (Command Testing Only)
```bash
# 1. Compile
make -f Makefile.test

# 2. Run
./test_ble_mac

# 3. Choose mode
# Mode 1: Test command generation
# Mode 2: Interactive mode
```

## 📋 What Each File Does

### `test_ble_python.py`
- **Scans** for BLE devices
- **Connects** to your RC car by MAC address
- **Discovers** services and characteristics
- **Sends** actual BLE commands
- **Interactive** command mode
- **Auto-stop** on exit

### `test_ble_mac.cpp`
- **Generates** command strings (hex format)
- **Displays** command breakdown
- **Tests** command format correctness
- **Interactive** mode for testing
- ⚠️ **Does NOT send** BLE commands (needs library)

## 🎯 Testing Workflow

### Step 1: Prepare
1. Turn on your RC car
2. Make sure it's in pairing mode
3. Enable Bluetooth on Mac
4. Note your car's MAC address

### Step 2: Test Connection
```bash
python3 test_ble_python.py
```

Expected output:
```
Scanning for BLE devices...
Found device: DR!FT Car [f9:af:3c:e2:d2:f5]
Connecting...
Connected successfully!
```

### Step 3: Test Commands
The script will automatically:
1. Send STOP command
2. Send START command (car moves)
3. Wait 2 seconds
4. Send STOP command again
5. Enter interactive mode

### Step 4: Interactive Control
```
> s    # Car starts moving forward
> l    # Car turns left
> r    # Car turns right
> t    # Car stops
> q    # Quit (sends stop automatically)
```

## 🔧 Command Format

All commands follow this format:
```
bf0a00082800  [Device ID - fixed]
000a          [Speed: 0-255]
0000          [Drift: usually 0]
0000          [Steering: 0=straight, 1-255=turn]
0200          [Lights: 0200=ON, 0000=OFF]
00            [Checksum]
```

**Example Commands**:
- **Stop**: `bf0a00082800000000000000000000000000`
- **Start (speed 20)**: `bf0a00082800001400000000000000020000`
- **Right turn**: `bf0a00082800001400000014000000020000`

## ✅ Verification Checklist

After running the test:

- [ ] Script finds your car's MAC address
- [ ] Connection successful
- [ ] Characteristic UUID found
- [ ] STOP command works (car stops)
- [ ] START command works (car moves)
- [ ] Interactive mode responds
- [ ] Commands sent successfully (✓ shown)
- [ ] Car stops when quitting

## 🐛 Common Issues

### Issue: "No BLE adapters found"
**Fix**: Enable Bluetooth in System Settings

### Issue: "Device not found"
**Fix**: 
- Check MAC address
- Make sure car is ON
- Car in pairing mode
- Move car closer

### Issue: "simplepyble not found"
**Fix**: `pip3 install simplepyble`

### Issue: "Permission denied"
**Fix**: System Settings → Privacy → Bluetooth → Allow Terminal

## 📊 Expected Results

### Successful Connection
```
✓ Found device
✓ Connected successfully
✓ Found target characteristic
✓ Commands sent successfully
```

### Car Response
- **START command**: Car moves forward slowly
- **STOP command**: Car stops immediately
- **TURN commands**: Car turns in specified direction
- **SPEED changes**: Car speed adjusts

## 🎓 Next Steps

Once BLE testing works:

1. ✅ Verify all commands work correctly
2. ✅ Note any car-specific behavior
3. ✅ Test with different speeds
4. ✅ Integrate into main C++ project
5. ✅ Connect camera and start full system

## 📚 Files Reference

| File | Purpose | Status |
|------|---------|--------|
| `test_ble_python.py` | Full BLE test (Python) | ✅ Ready |
| `test_ble_mac.cpp` | Command generation (C++) | ✅ Compiles |
| `Makefile.test` | Build C++ test | ✅ Ready |
| `QUICK_START_MAC.md` | Quick guide | ✅ Complete |
| `MAC_SETUP_INSTRUCTIONS.md` | Full guide | ✅ Complete |

## 💡 Tips

1. **Start with Python**: Easier to debug and test
2. **Test in safe area**: Car will move when you send START
3. **Always stop**: Script auto-stops, but be ready
4. **Check MAC address**: Wrong address = no connection
5. **Bluetooth range**: Keep car within ~10 meters

## 🎉 Success!

If you see:
- ✅ Connection successful
- ✅ Commands sent successfully
- ✅ Car responds to commands

Then your BLE connection is working! You can now proceed to integrate this into the full vision-based control system.
