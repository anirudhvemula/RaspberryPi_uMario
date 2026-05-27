


https://github.com/user-attachments/assets/04cc0e3d-6b41-4174-89b8-237faa86bcab


#  From a Forgotten Raspberry Pi to a Handheld Mario Console 🎮🏰👸🏼🍄🐢💗

> A weekend project that taught us more than gaming.

**"Can we make a custom Mario game console for Sri Nihira?"**

That innocent question turned into a full-blown hardware, software, Linux, electronics, and debugging adventure — involving a four-year-old Raspberry Pi, a retro gamepad, a tiny LCD screen, Bluetooth audio, custom C++ code, multiple operating systems, and more troubleshooting than I care to admit.

The result? A fully functional **Super Mario Bros. console powered by a Raspberry Pi 4B**, playable on both a monitor and a 3.5-inch touchscreen display using a classic SNES-style controller.

And perhaps more importantly, a project that reminded me why engineering is so much fun.
<br><br>

<img width="4096" height="3072" alt="IMG20260525113002" src="https://github.com/user-attachments/assets/3fb9e977-0270-446b-999d-49c08aa56f01" />

## 🎯 The Goal

Build a dedicated Super Mario Bros gaming console using hardware already sitting in a drawer.

**Starting point:** A 2021 Raspberry Pi 4B (2GB RAM) running Raspbian Buster.

### 🧰 Hardware Components
| Component | Details |
| --- | --- |
| Raspberry Pi 4B | 2GB RAM, 2021 model |
| Gamepad | Retro SNES/NES-style USB Gamepad |
| Display | 3.5" GPIO Touchscreen LCD (480×320) |
| Case | Acrylic Raspberry Pi + LCD enclosure |
| Audio | Bluetooth speaker |
| Storage | MicroSD card |
| Game | Open-source Super Mario Bros clone ([uMario](https://github.com/anirudhvemula/RaspberryPi_uMario)) |
<br>
<!-- Replace with your hardware photo -->
<img width="600" height="493" alt="3 5&#39;&#39; LCD Display" src="https://github.com/user-attachments/assets/6d7d2fd7-1c2c-410d-892b-ea5ea76ad335" /><br>
3.5" GPIO Touchscreen LCD (480×320) Display<br><br>
<img width="600" height="519" alt="Acrylic-case" src="https://github.com/user-attachments/assets/eba6be37-e882-4ecb-82ef-fc6862f18c97" /><br>
Acrylic Case for Raspberry Pi 4B and 3.5" LCD Display<br><br>
<img width="600" height="538" alt="gamepad" src="https://github.com/user-attachments/assets/874e7773-5ee0-4b20-a5c7-eaf553fb08cd" /><br>
Raspberry Pi Compatible Gamepad Controller<br><br>
<img width="600" height="418" alt="Bluetooth Speaker" src="https://github.com/user-attachments/assets/1584e86c-890a-45a4-91ff-a470eda9858f" /><br>
Bluetooth Speaker
<br>
<br>

The challenge wasn't simply getting the game to run. The challenge was making the entire system behave like a **real game console**:<br>
❌ No terminal windows<br>
❌ No keyboard requirements<br>
❌ No development environment visible<br>
✅ Just power on and play<br>

---

## 🛠️ Build Steps

### Step 1 — Making Mario Feel Like a Real Console

The original game launched inside a desktop window and required terminal execution — not acceptable for a game console. The first modification updated the source code so the game launched:
✅ Full screen
✅ Borderless
✅ Without visible terminal windows

The goal: turn **Linux into an invisible layer** and let Mario become the entire experience. <br><br>

<!-- Replace with your fullscreen screenshot -->
<img width="800" height="480" alt="Screen1" src="https://github.com/user-attachments/assets/7b740b20-2f0c-4bef-a796-a995868fc427" /> <br>
Mario game in Window Mode<br><br>
<img width="800" height="480" alt="Screen2" src="https://github.com/user-attachments/assets/eebbc3ab-1152-4cde-9545-efd7e30b9bc6" /> <br>

<br><br>

### Step 2 — The Operating System Surprise

The Raspberry Pi was still running **Debian Buster (10)**. Everything appeared normal until connected to a modern monitor — display issues surfaced immediately.

**Solution:** Reflash the microSD card with **Debian Bullseye (11) – December 2023 release**, reconfigure development tools, and rebuild the project.

> *Sometimes the hardest bugs are not in your code. They're in software written years before you touched the system.*

---

### Step 3 — Teaching Linux About the Gamepad

The retro controller looked fantastic. Unfortunately, Linux had no idea what to do with it. The first task was verifying the OS could see the device:

```bash
sudo apt install joystick
lsusb
jstest /dev/input/js0
```

**Results:** <br>
✅ Directional pad detected<br>
✅ Buttons detected<br>
✅ Start/Select detected<br>

With the hardware confirmed, the project was compiled and run:

```bash
sudo apt update
sudo apt upgrade -y
sudo apt install -y \
    build-essential \
    cmake \
    git \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev

git clone https://github.com/anirudhvemula/NihiraLuvsMario.git
cd NihiraLuvsMario
mkdir build
cd build
cmake ..
make -j$(nproc)
```

Run the executable:

```bash
./uMario
```

---

### Step 4 — The Most Frustrating Bug of the Entire Project

Integrating gamepad controls should have been straightforward. It wasn't.

One particularly stubborn issue caused: pressing **RIGHT** would sometimes make Mario **JUMP** — a simple walk could unexpectedly become an aerial maneuver.

Debug logging was added to investigate:

```cpp
// Write debug info to a log file for analysis
if(mainEvent->type == SDL_JOYAXISMOTION)
{
    std::ofstream log("/tmp/joydebug.log", std::ios::app);

    log << "AXIS "
        << (int)mainEvent->jaxis.axis
        << " = "
        << mainEvent->jaxis.value
        << std::endl;
}
```

Debugging process:
1. SDL event analysis
2. Joystick axis inspection
3. Input mapping revisions
4. Multiple source-code edits
5. Recompilation after every change
6. Extensive gameplay testing

The culprit: **controller event handling and axis interpretation**. After several iterations:<br>
✅ Walking worked<br>
✅ Jumping worked<br>
✅ Pause menu navigation worked<br>
✅ Start button functionality worked<br>
✅ Controller became a true keyboard replacement<br>

---

### Step 5 — Adding Sound

A Mario console without audio is only half a Mario console. A small Bluetooth speaker was paired with the Raspberry Pi. After configuring the audio output:<br>
🎵 Music played<br>
🎵 Sound effects worked<br>
🎵 Wireless audio connected successfully<br>

The first time the classic Mario soundtrack came through the speaker felt like a major milestone.

---

### Step 6 — The LCD Screen Disaster

A 3.5-inch Raspberry Pi touchscreen was purchased, advertised as compatible. The result after installation?

**A completely white screen. No graphics. No desktop. Nothing.**

<img width="600" height="450" alt="IMG20260525004057" src="https://github.com/user-attachments/assets/3e3d5a5b-0e31-4a1b-9af2-c782aa2556df" />
<br><br>

Hours of investigation followed — different drivers, different repositories, different installation methods, reboots, reinstalls, more reboots.

**Root cause:** The display model supplied by the vendor was not the model described in the documentation.

> The **LCDWiki MPI3501** is a 3.5" SPI TFT touchscreen (480×320) based on the **ILI9486 controller** with an **XPT2046 touch controller**. <br><br>
> The flashed image (`MPI3508`) was for a *different* display model entirely.

<br>
<img width="600" height="1067" alt="IMG-20260525-WA0002" src="https://github.com/user-attachments/assets/51186d36-3555-40b5-a0c0-ba736dd1a43f" />
<br>
<br>
Once the correct driver was identified:

```bash
sudo rm -rf LCD-show
git clone https://github.com/goodtft/LCD-show.git
chmod -R 755 LCD-show
cd LCD-show/
sudo ./LCD35-show
```

✅ Display initialized<br>
✅ Touchscreen responded<br>
✅ Graphics rendered properly<br>

> *Sometimes debugging is less about engineering and more about detective work and persistence.*

---

### Step 7 — One Tiny Setting That Changes Everything

Before connecting the display permanently, one critical feature needed to be enabled: **SPI (Serial Peripheral Interface)**.

```
Without SPI enabled: ❌ Display communication fails
With SPI enabled:    ✅ Display communication succeeds
```

One checkbox. Several hours saved.
<br><br>
<img width="640" height="360" alt="rc_cmd_spi_1" src="https://github.com/user-attachments/assets/f0825189-228e-4fcc-8042-8bf96d429d79" />
<br>

---

### Step 8 — Success... Sort Of

<img width="600" height="450" alt="IMG20260524225308" src="https://github.com/user-attachments/assets/1440b2c1-8d78-461b-ad60-9229b27e5e73" /><br>
Stuck at boot. rc-local.service <br><br>

The LCD finally came alive — but only in terminal mode. Linux booted, text appeared, the screen functioned, yet the graphical desktop refused to launch automatically.

The fix required changing Raspberry Pi startup behavior:
- Enable automatic desktop login
- Restore GUI startup mode
- Adjust boot configuration

After rebooting: 🎉 **Full Raspberry Pi desktop on the 3.5-inch screen!!!**

### Success. 😎👌🔥🏆
<br>
<img width="600" height="450" alt="IMG20260525030521" src="https://github.com/user-attachments/assets/65cd1345-91ac-4df2-b576-1b14dcfce26c" />


---

### Step 9 — Restoring HDMI Output

The LCD was now operational, but HDMI stopped cooperating — the Raspberry Pi believed the tiny display was the only display in existence.

More configuration adjustments followed:
- Reset display resolution settings
- Restore HDMI detection
- Verify dual-display functionality

**Final result:**<br>
✅ HDMI monitor works<br>
✅ 3.5-inch LCD works<br>
✅ Mario launches successfully<br>
✅ Gamepad controls gameplay<br>
✅ Bluetooth speaker handles audio<br>

**Mission accomplished.** 🏆
<br>
<br>
<img width="600" height="800" alt="IMG20260525123437" src="https://github.com/user-attachments/assets/4f5f189f-2f3b-466a-9863-e2ee4999bce6" />


---

## 💡 What We Learned

This project wasn't really about Mario. It was about the intersection of:

- **Linux** — OS-level configuration and troubleshooting
- **Embedded Systems** — GPIO, SPI, display interfaces
- **C++** — Source code modification and compilation
- **SDL Game Development** — Input handling and rendering
- **Device Drivers** — Identifying and installing correct hardware drivers
- **Bluetooth Audio** — Wireless speaker pairing and configuration
- **Hardware Troubleshooting** — Persistence through unexpected failures

The same engineering principles that apply to serious projects apply here too:

1. Define the objective.
2. Break down the problem.
3. Debug relentlessly.
4. Never trust vendor documentation blindly.
5. Keep iterating until it works.

---

## ❤️ The Best Part

The most rewarding moment wasn't compiling the code. It wasn't fixing the LCD. It wasn't hearing the soundtrack.

It was watching **Sri Nihira** sit down with the controller and immediately start playing a game running on hardware we had assembled and debugged ourselves.

That moment made every failed driver installation worthwhile.

> *Engineering isn't only about solving problems. Sometimes it's about creating experiences.*

🎮 🍄 👨‍💻

---

## 📚 References

- **Display Driver** — LCD Wiki: [3.5inch HDMI Display-B](https://www.lcdwiki.com/3.5inch_HDMI_Display-B)
- **Source Code** — GitHub: [anirudhvemula/RaspberryPi_uMario](https://github.com/anirudhvemula/RaspberryPi_uMario)
- **LCD Driver Repo** — GitHub: [goodtft/LCD-show](https://github.com/goodtft/LCD-show)

---

## 🏷️ Tags

`#RaspberryPi` `#Linux` `#EmbeddedSystems` `#CPlusPlus` `#OpenSource` `#RetroGaming` `#SuperMarioBros` `#STEM` `#Engineering` `#Maker` `#DIYProjects` `#GameDevelopment` `#IoT` `#HardwareHacking` `#Debugging` `#LearningByBuilding`
