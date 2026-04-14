# 🔵 Doraemon OLED — ESP8266 Desk Companion

> Your favourite blue cat-robot, now living on a 0.96" OLED display. Animated pixel art faces with 6 expressions, 25 funny quotes, 10 gadget cards, and a full dance animation — all on an ESP8266 with one button.

*"I came from the future. Still no WiFi."* — Doraemon, probably.

---

## What It Does

| Mode | Description |
|---|---|
| 😺 Face Mode | Big animated Doraemon face cycling through 6 expressions |
| 💬 Quote Mode | 25 funny Doraemon quotes auto-cycling every 5 seconds |
| 🎒 Gadget Mode | "Gadget of the Day" cards with name, effect, and warning |
| 🕺 Dance Mode | Full-body Doraemon bounce dance animation |

### The 6 Expressions

| Expression | Face |
|---|---|
| Normal | `(^o^)` Round eyes, soft smile |
| Happy | `(^▽^)` Curved eyes, big grin |
| Shocked | `(O_O)` Huge pupils, open mouth |
| Sleepy | `(-_-)` Half-lid eyes, flat mouth |
| Angry | `(>_<)` Slanted brows, grimace |
| Dorayaki | `(^o^)♥` Heart eyes, licking lips |

### Sample Quotes

- *"I ate the gadget again. My bad."*
- *"Nobita got 0% again. I'm so tired."*
- *"My pocket has everything except a girlfriend."*
- *"Time machine? Used it to skip Monday. Worth it."*
- *"I came from the future. Still no WiFi."*
- *"Dorayaki count: 47. Don't judge."*

### Sample Gadgets

| Gadget | Warning |
|---|---|
| Anywhere Door | May end up in someone's closet |
| Memory Bread | Side effect: craving toast 24/7 |
| Dorayaki Gun | Extremely dangerous to diets |
| Bully Radar | Always buzzing |
| Copy Mirror | Clone may be smarter than you |

---

## Hardware

### Components

| Part | Details |
|---|---|
| Board | ESP8266 — NodeMCU or Wemos D1 Mini |
| Display | 0.96" OLED, SSD1306, 128×64 px, I2C |
| Button | Momentary push button (normally open) |
| Power | Micro-USB 5V |

### Wiring

| ESP8266 Pin | Connected To | Notes |
|---|---|---|
| D1 (GPIO5) | OLED SCL | I2C Clock |
| D2 (GPIO4) | OLED SDA | I2C Data |
| 3V3 | OLED VCC | 3.3V only — not 5V |
| GND | OLED GND + Button | Ground |
| D3 (GPIO0) | Button → GND | Active LOW, internal pull-up |

> **No external button needed to test!** GPIO0 is the built-in FLASH button on all NodeMCU and D1 Mini boards.

---

## Project Structure

```
ESP8266-Doraemon-OLED/
├── main.ino          ← App loop, mode management, auto-cycling
├── config.h          ← Pin defs, timing constants, mode enum
├── button.h / .cpp   ← Debounced press + hold detection
├── quotes.h          ← 25 quotes + 10 gadget definitions
├── renderer.h / .cpp ← All pixel art drawing (faces, dance, gadgets)
└── README.md
```

---

## Library Installation

**Sketch → Include Library → Manage Libraries**, install:

| Library | Author |
|---|---|
| Adafruit GFX | Adafruit |
| Adafruit SSD1306 | Adafruit |

No other external libraries needed.

---

## Flash & Run

1. Open `main.ino` in Arduino IDE
2. **Tools → Board** → `NodeMCU 1.0` or `LOLIN(Wemos) D1 Mini`
3. **Tools → Port** → select your COM port
4. Click **Upload**
5. Doraemon appears in ~2 seconds

---

## Controls

| Action | Result |
|---|---|
| Short press | Cycle to next mode (Face → Quote → Gadget → Dance) |
| Hold 1s | Jump straight to Dance mode |
| No input | Modes auto-advance every 8 seconds |

### Auto-Behaviour

- **Quote mode**: New quote every 5 seconds automatically
- **All modes**: Switch to next mode every 8 seconds of idle
- **Dance mode**: Infinite loop until button pressed

---

## Customisation

### Add Your Own Quotes

Open `quotes.h` and add to `DORAEMON_QUOTES[]`:

```cpp
"Your funny quote\nhere!",
```

Keep each line under ~18 characters. Use `\n` for line breaks.

### Add Gadgets

In `quotes.h`, add to `GADGETS[]`:

```cpp
{ "Gadget Name", "What it does!", "Funny warning here." },
```

### Change Timing

Edit `config.h`:

```cpp
// How long each quote stays on screen (default 5s)
#define QUOTE_DISPLAY_MS    5000

// Animation frame speed — lower = faster (default 120ms)
#define ANIM_FRAME_MS        120

// Auto-mode switch idle time (default 8s)
#define IDLE_SWITCH_MS      8000
```

---

## Pixel Art Details

Doraemon is drawn entirely with Adafruit GFX primitives — no bitmaps, no PROGMEM images. Everything is generated in code:

- **Head**: `fillCircle` with black outline
- **Eyes**: 6 different expressions drawn with circles, lines, and fill rects
- **Whiskers**: 3 lines each side
- **Mouth**: Pixel-plotted parabola curves for smile/grin
- **Collar + Bell**: `fillCircle` with white cutout
- **Pocket**: Clipped circle with `drawFastHLine`
- **Body** (dance mode): Rounded rect + ellipses for arms/feet
- **Bounce**: Y offset oscillates every 2 frames

This means it works on any SSD1306 OLED with no extra storage.

---

## Troubleshooting

**Display blank**
- Check SDA → D2 (GPIO4), SCL → D1 (GPIO5)
- Use 3.3V only for OLED power — 5V will burn it
- Confirm I2C address `0x3C` in `config.h`

**Face looks wrong / garbled**
- Ensure Adafruit SSD1306 and Adafruit GFX are both installed
- Try `Tools → Board → ESP8266` with CPU frequency 80MHz

**Button not responding**
- GPIO0 is active LOW — connects to GND when pressed
- Open Serial Monitor at 115200 to see `[MODE]` transitions

---

## License

MIT — take it, remix it, make Doraemon do other things.

---

## Credits

Doraemon is a character created by Fujiko F. Fujio.  
This is a fan project for educational/hobby use only.  
Built with ESP8266 Arduino core + Adafruit SSD1306.

*"Friendship is the best gadget I ever had."* 🔵
