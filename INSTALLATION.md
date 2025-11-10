# Installation Guide - MiniIoT Library

Complete installation guide for all platforms.

---

## 📦 Installation Methods

### ⭐ Method 1: Arduino Library Manager (Easiest)

**Arduino IDE will auto-install all dependencies!**

#### Steps:

1. Open **Arduino IDE**
2. Click **Sketch** → **Include Library** → **Manage Libraries...**
3. In search box, type: **MiniIoT**
4. Click **Install** button
5. Wait for dependencies to install:
   - ✅ PubSubClient
   - ✅ ArduinoJson
6. Done! ✨

**Verification:**
- Go to **Sketch** → **Include Library**
- You should see **MiniIoT** in the list

---

### 📥 Method 2: GitHub ZIP Download

**For users who download directly from GitHub.**

#### Step 1: Download

Go to: https://github.com/yourusername/MiniIoT/releases

Download: `MiniIoT.zip` (latest release)

#### Step 2: Add to Arduino IDE

1. Arduino IDE → **Sketch** → **Include Library** → **Add .ZIP Library...**
2. Browse to downloaded `MiniIoT.zip`
3. Click **Open**
4. Arduino IDE shows: "Library added to your libraries"

#### Step 3: Install Dependencies

**Arduino IDE 1.8.10+ will auto-prompt:**

```
┌──────────────────────────────────────┐
│ Library MiniIoT depends on:          │
│                                      │
│ • PubSubClient (>=2.8.0)            │
│ • ArduinoJson (>=6.19.0, <7.0.0)    │
│                                      │
│ [Install all] [Install manually]    │
└──────────────────────────────────────┘
```

**Click "Install all"** → Done!

#### Step 3b: Manual Dependency Install

If auto-install doesn't work:

**Install PubSubClient:**
1. **Sketch** → **Include Library** → **Manage Libraries...**
2. Search: **PubSubClient**
3. Select: **PubSubClient by Nick O'Leary**
4. Click **Install**

**Install ArduinoJson:**
1. **Sketch** → **Include Library** → **Manage Libraries...**
2. Search: **ArduinoJson**
3. Select: **ArduinoJson by Benoit Blanchon**
4. Choose version: **6.21.3** (or latest 6.x.x, NOT 7.x.x)
5. Click **Install**

---

### 🔧 Method 3: Git Clone (For Developers)

#### For Windows:

```powershell
cd "%USERPROFILE%\Documents\Arduino\libraries"
git clone https://github.com/yourusername/MiniIoT.git
```

#### For macOS/Linux:

```bash
cd ~/Documents/Arduino/libraries/
git clone https://github.com/yourusername/MiniIoT.git
```

#### Install Dependencies:

```bash
# Open Arduino IDE
# Sketch → Include Library → Manage Libraries
# Install: PubSubClient, ArduinoJson
```

**Restart Arduino IDE**

---

### 📱 Method 4: PlatformIO (Advanced)

Add to `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    MiniIoT
    # Dependencies auto-installed!
```

Or manual:

```ini
lib_deps = 
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^6.21.3
    MiniIoT
```

---

## ✅ Verify Installation

### Test 1: Include Test

Create new sketch:

```cpp
#include <MiniIoT.h>

void setup() {
  Serial.begin(115200);
  Serial.println("MiniIoT library loaded!");
}

void loop() {}
```

Click **Verify** (✓)

**Success:** "Done compiling"  
**Fail:** "MiniIoT.h: No such file"

---

### Test 2: Example Sketch

**File** → **Examples** → **MiniIoT** → **Basic**

Modify WiFi credentials:
```cpp
const char* ssid = "YourWiFi";
const char* password = "YourPassword";
```

**Upload to ESP32**

**Serial Monitor (115200 baud):**
```
✅ WiFi connected!
📡 IP Address: 192.168.1.105
✅ MQTT Connected!
```

---

## 🐛 Troubleshooting

### Issue: Library not found

**Symptoms:**
```
fatal error: MiniIoT.h: No such file or directory
```

**Solutions:**

1. **Restart Arduino IDE** (most common fix)
2. Check library location:
   - Windows: `C:\Users\YourName\Documents\Arduino\libraries\MiniIoT\`
   - macOS: `/Users/YourName/Documents/Arduino/libraries/MiniIoT/`
   - Linux: `/home/YourName/Arduino/libraries/MiniIoT/`
3. Verify folder contains:
   - `MiniIoT.h`
   - `MiniIoT.cpp`
   - `library.properties`

---

### Issue: PubSubClient not found

**Symptoms:**
```
fatal error: PubSubClient.h: No such file or directory
```

**Solution:**

**Arduino IDE:**
1. Sketch → Include Library → Manage Libraries
2. Search: "PubSubClient"
3. Install: PubSubClient by Nick O'Leary

**PlatformIO:**
```ini
lib_deps = knolleary/PubSubClient@^2.8
```

---

### Issue: ArduinoJson not found

**Symptoms:**
```
fatal error: ArduinoJson.h: No such file or directory
```

**Solution:**

**Arduino IDE:**
1. Sketch → Include Library → Manage Libraries
2. Search: "ArduinoJson"
3. **Important:** Install version **6.x.x** (NOT 7.x.x)
4. Recommended: 6.21.3

**PlatformIO:**
```ini
lib_deps = bblanchon/ArduinoJson@^6.21.3
```

---

### Issue: Wrong ArduinoJson version

**Symptoms:**
```
error: 'StaticJsonDocument' was not declared
```

**Solution:**

You installed ArduinoJson v7 (incompatible).

**Uninstall v7:**
1. Sketch → Include Library → Manage Libraries
2. Search: "ArduinoJson"
3. Click **Remove**

**Install v6:**
1. Search: "ArduinoJson"
2. Click **Select version** dropdown
3. Choose: **6.21.3**
4. Click **Install**

---

### Issue: ESP32 board not found

**Symptoms:**
```
Board not found in boards.txt
```

**Solution:**

Install ESP32 board support:

1. **File** → **Preferences**
2. **Additional Board Manager URLs:**
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools** → **Board** → **Boards Manager**
4. Search: "esp32"
5. Install: "esp32 by Espressif Systems"

---

## 🔄 Updating Library

### Arduino Library Manager:

1. Sketch → Include Library → Manage Libraries
2. Search: "MiniIoT"
3. If update available, click **Update**

### Git Clone:

```bash
cd ~/Documents/Arduino/libraries/MiniIoT
git pull origin main
```

**Restart Arduino IDE after update!**

---

## 🗑️ Uninstalling

### Arduino IDE:

1. Close Arduino IDE
2. Delete folder:
   - Windows: `C:\Users\YourName\Documents\Arduino\libraries\MiniIoT\`
   - macOS: `/Users/YourName/Documents/Arduino/libraries/MiniIoT/`
   - Linux: `/home/YourName/Arduino/libraries/MiniIoT/`
3. Restart Arduino IDE

### PlatformIO:

Remove from `platformio.ini`:
```ini
lib_deps = 
    # MiniIoT  ← Remove this line
```

---

## 📋 Checklist

Before asking for help, verify:

- [ ] Arduino IDE version ≥ 1.8.10
- [ ] ESP32 board support installed
- [ ] MiniIoT library installed
- [ ] PubSubClient ≥ 2.8.0 installed
- [ ] ArduinoJson **6.x.x** (not 7.x.x) installed
- [ ] Arduino IDE restarted after install
- [ ] Example sketch compiles without errors

---

**Need more help?**

📧 Email: support@miniiot.com  
🐛 Issues: https://github.com/yourusername/MiniIoT/issues