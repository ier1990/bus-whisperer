
---

## 🧠 Pin Roles (ATmega2560)

| Pin Range   | Role                        |
|-------------|-----------------------------|
| D2–D17      | Active signal drive (160Ω)  |
| D18–D25     | Comm/pulse or expansion use |
| D22–D53     | Pull-up/pull-down config (10k) |
| A0–A15      | Analog monitoring (100Ω + Zener) |

---

---

## 🧠 How One Pin Works

Each of the 16 test pins on the Bus Whisperer is more than a simple wire — it's a smart, flexible interface that can:

- Drive logic levels (HIGH or LOW)
- Pull gently to 5V or GND
- Monitor voltages in real time
- Float to let the circuit speak back

### 🧰 Internally, each test pin connects to:

- 🔵 **1 analog input (A0–A15)** through a 100Ω resistor and Zener — for safe voltage sniffing
- 🟢 **1 active drive pin (D2–D17)** through 160Ω — for logic stimulation (pulses, HIGH/LOW)
- 🟡 **1 weak pull pin (D22–D53)** through 10kΩ — for passive pull-up/down

### ⚙️ Software can switch a pin into modes:

| Mode         | What It Does                                   |
|--------------|------------------------------------------------|
| Float        | Releases control (all pins in `INPUT`)         |
| Pull-Up      | Gently raises pin to 5V                        |
| Pull-Down    | Gently lowers pin to GND                       |
| Drive HIGH   | Strongly drives pin to 5V                      |
| Drive LOW    | Strongly drives pin to GND                     |
| Monitor      | Reads the actual voltage via `analogRead()`    |

This design lets you safely interact with unknown ICs, live boards, or logic terminals — one pin at a time, or all 16 in parallel.

---

### 🖼️ [Visual diagram coming soon...]


## 🔌 Hardware Suggestions

- 16-pin ZIF socket or IDE header
- Banana jacks or logic clip connectors
- Ribbon cable for DIP clip probing
- 16-channel relay board (optional)
- ATmega2560 board (Arduino Mega)

---

## 🚀 Coming Soon

- [ ] Automatic pin type detection
- [ ] Basic logic IC test profiles (e.g., 7400, 4011, 74121)
- [ ] ECU/drive pinout guesser with scan logging
- [ ] Web serial interface (browser-based)

---

## 🤝 Contribute

Found it useful? Fork it, break it, fix it — and whisper back:
**[github.com/ier1990/bus-whisperer](https://github.com/YOURNAME/bus-whisperer)**

---

## 📸 License

MIT License — feel free to reuse, modify, or remix with proper attribution.

---

*Built by hand, tested on the bench, and debugged in the basement.*
