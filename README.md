# ESP8266 Doraemon OLED

> *"I came from the future. Still no WiFi."*

Your favourite blue cat-robot on a 0.96″ OLED display.  
Animated pixel-art faces with **6 expressions**, **25 funny quotes**, **10 gadget-of-the-day cards**, and a full **bounce dance** — all on an ESP8266 with a single button.

---

## Features

| Mode | Content | Button (short press) |
|------|---------|----------------------|
| **FACE** | 6 animated pixel-art expressions: Normal · Happy · Wink · Sleepy · Surprised · Angry | Cycle to next expression |
| **QUOTE** | 25 funny Doraemon quotes | Cycle to next quote |
| **GADGET** | 10 gadget-of-the-day cards (Anywhere Door, Bamboo-Copter, Memory Bread, …) | Cycle to next gadget |
| **DANCE** | 16-frame bounce animation with floating music notes | Reset animation |

**Long press (≥ 600 ms)** → advance to the next mode  
**Mode cycle:** `FACE → QUOTE → GADGET → DANCE → FACE …`

---

## Hardware

| Component | Spec |
|-----------|------|
| Microcontroller | NodeMCU v2 / ESP-12 / D1-mini (any ESP8266) |
| Display | 0.96″ SSD1306 OLED, 128 × 64, I²C |
| Button | Momentary push-button |

### Wiring

```
ESP8266 Pin   →   Connects to
────────────────────────────────
D2 (GPIO4)    →   OLED SDA
D1 (GPIO5)    →   OLED SCL
3.3 V         →   OLED VCC
GND           →   OLED GND
D3 (GPIO0)    →   Button pin 1
GND           →   Button pin 2   (internal pull-up used)
```

> The OLED I²C address defaults to `0x3C`.  
> If your module uses `0x3D`, change the address in `display.begin(SSD1306_SWITCHCAPVCC, 0x3C)`.

---

## Software Setup

### Arduino IDE

1. **Board support** – add the ESP8266 board URL in *Preferences*:  
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`  
   Then install *esp8266* via *Tools → Board → Boards Manager*.

2. **Libraries** – install via *Sketch → Include Library → Manage Libraries*:
   - `Adafruit SSD1306` ≥ 2.5
   - `Adafruit GFX Library` ≥ 1.11

3. **Open sketch** – open `Doraemon_OLED/Doraemon_OLED.ino`.

4. **Board settings**:
   - Board: *NodeMCU 1.0 (ESP-12E Module)* (or your variant)
   - Upload Speed: 115200
   - Flash Size: 4MB

5. **Upload** and enjoy 🍩

---

## File Structure

```
Doraemon_OLED/
└── Doraemon_OLED.ino   ← single-file Arduino sketch (~590 lines)
```

---

## Expressions

```
0 Normal   – round eyes, gentle smile
1 Happy    – arc eyes squinting with joy, big toothed grin
2 Wink     – left eye open, right eye closed, smirk
3 Sleepy   – half-closed eyes, flat mouth, drool drop
4 Surprised– wide eyes, open O mouth
5 Angry    – normal eyes with diagonal brows, frown
```

---

## Gadgets of the Day

1. Anywhere Door  
2. Bamboo-Copter  
3. Small Light  
4. Memory Bread  
5. Take-copter  
6. 4D Pocket  
7. Big Light  
8. Translation Jelly  
9. Weather Box  
10. Echo Glasses  

---

## License

MIT — feel free to remix, improve, and share.
