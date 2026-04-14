/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║  ESP8266 Doraemon OLED                                      ║
 * ║  0.96" SSD1306 128×64  ·  single-button control            ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 *  Features
 *  ─────────
 *  • 6 animated pixel-art face expressions
 *  • 25 funny Doraemon quotes
 *  • 10 gadget-of-the-day cards
 *  • 16-frame bounce-dance animation with floating music notes
 *
 *  Wiring  (NodeMCU / ESP-12 / D1-mini)
 *  ─────────────────────────────────────
 *  OLED SDA  →  D2  (GPIO4)
 *  OLED SCL  →  D1  (GPIO5)
 *  OLED VCC  →  3.3 V
 *  OLED GND  →  GND
 *  Button    →  D3  (GPIO0)  +  GND   (internal pull-up enabled)
 *
 *  Button actions
 *  ──────────────
 *  Short press  ( < 600 ms )  →  next item in current mode
 *  Long  press  ( ≥ 600 ms )  →  advance to next mode
 *
 *  Mode cycle:   FACE  →  QUOTE  →  GADGET  →  DANCE  →  FACE …
 *
 *  Libraries (install via Arduino Library Manager)
 *  ────────────────────────────────────────────────
 *  Adafruit SSD1306   ≥ 2.5
 *  Adafruit GFX       ≥ 1.11
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─────────────────────────────────────────────────────────────────────────────
//  Hardware
// ─────────────────────────────────────────────────────────────────────────────
#define SCR_W      128
#define SCR_H       64
#define BTN_PIN      0    // GPIO0 / D3  – active-LOW, internal pull-up

Adafruit_SSD1306 display(SCR_W, SCR_H, &Wire, -1);

// ─────────────────────────────────────────────────────────────────────────────
//  Timing
// ─────────────────────────────────────────────────────────────────────────────
static const uint16_t DEBOUNCE_MS  =  50;
static const uint16_t LONG_MS      = 600;
static const uint16_t DANCE_TICK   =  90;   // ms per dance frame
static const uint8_t  DANCE_FRAMES =  16;

// ─────────────────────────────────────────────────────────────────────────────
//  App state
// ─────────────────────────────────────────────────────────────────────────────
enum Mode : uint8_t { FACE = 0, QUOTE, GADGET, DANCE, MODE_CNT };

static Mode     g_mode      = FACE;
static uint8_t  g_faceIdx   = 0;    // 0–5
static uint8_t  g_quoteIdx  = 0;    // 0–24
static uint8_t  g_gadgetIdx = 0;    // 0–9
static uint8_t  g_danceStep = 0;

static bool     g_prevBtn   = HIGH;
static uint32_t g_pressAt   = 0;
static bool     g_longFired = false;

static uint32_t g_danceAt   = 0;
static bool     g_needDraw  = true;

// ─────────────────────────────────────────────────────────────────────────────
//  Content tables
// ─────────────────────────────────────────────────────────────────────────────

// 6 expression names
static const char * const EXPR_NAME[6] = {
  "Normal", "Happy", "Wink", "Sleepy", "Surprised", "Angry"
};

// 25 quotes
static const char * const QUOTES[25] = {
  "I came from the\nfuture. Still\nno WiFi.",
  "My 4D pocket has\neverything except\na phone charger.",
  "Anywhere Door:\nstill cheaper\nthan Uber.",
  "Nobita failed\nagain. I am\nnot surprised.",
  "A dorayaki a day\nkeeps Gian\nfar away.",
  "4D pocket: 90%\ndorayaki, 10%\ngadgets.",
  "If Nobita had\nstudied, I would\nbe retired by now.",
  "Small Light:\nbecause city\nparking is hard.",
  "Bamboo-copter:\nthe original drone.\nNo FAA required.",
  "Memory Bread:\ntastes better\nthan any exam.",
  "Time machine:\nI use it to\navoid Mondays.",
  "No ears, yet I\nhear every single\ncry for help.",
  "Gian's singing:\nthe one gadget\nI cannot fix.",
  "Translation Jelly\nstill cannot decode\nNobita's homework.",
  "Big Light:\nbecause bigger\nis always better.",
  "Take-copter:\nbest commute.\nBattery: finite.",
  "Suneo showed off.\nI simply reached\ninto my pocket.",
  "Dokodemo Door:\nfirst teleport ever.\nNo TSA, no queue.",
  "I am a cat robot.\nDogs have owners.\nCats have staff.",
  "Pass-Through Hoop:\nI use it to\nskip Mondays.",
  "Weather Box:\nbetter forecast\nthan any app.",
  "Gourmet Tablecloth:\nfinally, food that\nisn't burnt.",
  "Echo Glasses:\nhear what others\nreally think.",
  "Future newspaper:\nspoiler - Nobita\nfailed again.",
  "Whatever happens,\nI believe in you.\nAlso, please study."
};

// 10 gadget names
static const char * const G_NAMES[10] = {
  "Anywhere Door",  "Bamboo-Copter",  "Small Light",
  "Memory Bread",   "Take-copter",    "4D Pocket",
  "Big Light",      "Transl. Jelly",  "Weather Box",
  "Echo Glasses"
};

// 10 gadget descriptions
static const char * const G_DESCS[10] = {
  "Step through to\nanywhere you wish.\nNo fare needed.",
  "Wear on head,\nspin and soar high.\nNo licence req.",
  "Shrinks anything.\nGreat for parking,\nbad for snacks.",
  "Press on a page,\neat the bread,\nknowledge stored!",
  "Propeller hat.\nAltitude: infinite.\nBattery: finite.",
  "Infinite storage,\nrandom retrieval.\nBetter than cloud.",
  "Enlarges objects.\nDo NOT aim it\nat Gian's ego.",
  "Eat it and speak\nany language.\nNo app needed.",
  "Control the sky.\nSun, rain or snow.\nYou decide today.",
  "See the thoughts\nof whoever you\nlook at. Careful."
};

// ─────────────────────────────────────────────────────────────────────────────
//  Dance animation tables
//  yOff  = vertical bounce offset (negative = jumping up)
//  expr  = which face expression to use each frame
// ─────────────────────────────────────────────────────────────────────────────
static const int8_t  DANCE_Y[16]    = {
   0, -2, -4, -6, -4, -2,  0,  2,
   0, -2, -5, -8, -5, -2,  0,  2
};
static const uint8_t DANCE_EXPR[16] = {
  1,  1,  1,  4,  1,  1,  1,  1,
  1,  2,  1,  4,  1,  2,  1,  1
};

// ─────────────────────────────────────────────────────────────────────────────
//  Low-level draw helpers
// ─────────────────────────────────────────────────────────────────────────────

// Quadratic Bézier curve  (x0,y0) → control (cpx,cpy) → (x1,y1)
static void drawBez(int x0, int y0, int cpx, int cpy, int x1, int y1) {
  float px = (float)x0, py = (float)y0;
  for (int i = 1; i <= 14; i++) {
    float t  = i / 14.0f;
    float it = 1.0f - t;
    float nx = it * it * x0 + 2.0f * it * t * cpx + t * t * x1;
    float ny = it * it * y0 + 2.0f * it * t * cpy + t * t * y1;
    display.drawLine((int)px, (int)py, (int)nx, (int)ny, WHITE);
    px = nx;
    py = ny;
  }
}

// Single musical note glyph anchored at (x, y)
static void drawNote(int x, int y) {
  display.fillCircle(x,     y + 5, 2, WHITE);   // note head
  display.drawLine(x + 2,   y + 5, x + 2, y, WHITE); // stem
  display.drawLine(x + 2,   y,     x + 7, y + 2, WHITE); // flag
}

// ─────────────────────────────────────────────────────────────────────────────
//  FACE RENDERER
//
//  Layout (cy = 32 + yOff, head radius = 23):
//    Head circle     : cy ± 23   → y = 9 .. 55  (normal)
//    Eyes            : ey = cy - 4,  lx = cx-10, rx = cx+10,  r = 5
//    Nose circle     : (cx, cy+4),   r = 3
//    Mouth           : mby = cy+11,  Bézier or shapes
//    Whiskers        : wy  = cy+4,   3 lines per side
//    Collar / bell   : drawn only when cy+30 < SCR_H
//
//  Expressions
//    0 = Normal      3 = Sleepy
//    1 = Happy       4 = Surprised
//    2 = Wink        5 = Angry
// ─────────────────────────────────────────────────────────────────────────────
static void drawFace(uint8_t expr, int8_t yOff = 0) {
  const int cx = 64;
  const int cy = 32 + (int)yOff;

  // ── Head outline (double circle = bold) ─────────────────────────────
  display.drawCircle(cx, cy, 23, WHITE);
  display.drawCircle(cx, cy, 22, WHITE);

  // ── Face-patch circle (slightly lower, creates inner-face boundary) ─
  display.drawCircle(cx, cy + 3, 15, WHITE);

  // ── Eyes ─────────────────────────────────────────────────────────────
  const int lx = cx - 10, rx = cx + 10, ey = cy - 4;

  switch (expr) {
    case 0: // Normal – round open eyes with pupils
      display.fillCircle(lx, ey, 5, WHITE);
      display.fillCircle(rx, ey, 5, WHITE);
      display.fillCircle(lx + 1, ey + 1, 2, BLACK);
      display.fillCircle(rx + 1, ey + 1, 2, BLACK);
      // small highlight dot
      display.drawPixel(lx - 1, ey - 2, WHITE);
      display.drawPixel(rx - 1, ey - 2, WHITE);
      break;

    case 1: // Happy – upward Bézier arc eyes (squinting joy)
      drawBez(lx - 5, ey + 3, lx,     ey - 5, lx + 5, ey + 3);
      drawBez(rx - 5, ey + 3, rx,     ey - 5, rx + 5, ey + 3);
      display.drawLine(lx - 5, ey + 4, lx + 5, ey + 4, WHITE);
      display.drawLine(rx - 5, ey + 4, rx + 5, ey + 4, WHITE);
      break;

    case 2: // Wink – left eye open, right eye closed
      display.fillCircle(lx, ey, 5, WHITE);
      display.fillCircle(lx + 1, ey + 1, 2, BLACK);
      display.drawPixel(lx - 1, ey - 2, WHITE);
      display.drawLine(rx - 5, ey,     rx + 5, ey,     WHITE);
      display.drawLine(rx - 5, ey + 1, rx + 5, ey + 1, WHITE);
      break;

    case 3: // Sleepy – half-closed (black rectangle masks top half)
      display.fillCircle(lx, ey, 5, WHITE);
      display.fillCircle(rx, ey, 5, WHITE);
      display.fillRect(lx - 5, ey - 6, 11, 7, BLACK);
      display.fillRect(rx - 5, ey - 6, 11, 7, BLACK);
      display.fillCircle(lx, ey + 1, 2, BLACK);
      display.fillCircle(rx, ey + 1, 2, BLACK);
      break;

    case 4: // Surprised – very wide eyes
      display.drawCircle(lx, ey, 7, WHITE);
      display.drawCircle(rx, ey, 7, WHITE);
      display.fillCircle(lx, ey, 5, WHITE);
      display.fillCircle(rx, ey, 5, WHITE);
      display.fillCircle(lx + 1, ey + 1, 2, BLACK);
      display.fillCircle(rx + 1, ey + 1, 2, BLACK);
      display.drawPixel(lx - 2, ey - 3, WHITE);
      display.drawPixel(rx - 2, ey - 3, WHITE);
      break;

    case 5: // Angry – normal eyes with diagonal eyebrows
      display.fillCircle(lx, ey, 5, WHITE);
      display.fillCircle(rx, ey, 5, WHITE);
      display.fillCircle(lx + 1, ey + 1, 2, BLACK);
      display.fillCircle(rx + 1, ey + 1, 2, BLACK);
      // inner-end-down brows
      display.drawLine(lx - 5, ey - 7, lx + 4, ey - 10, WHITE);
      display.drawLine(lx - 5, ey - 8, lx + 4, ey - 11, WHITE);
      display.drawLine(rx - 4, ey - 10, rx + 5, ey - 7,  WHITE);
      display.drawLine(rx - 4, ey - 11, rx + 5, ey - 8,  WHITE);
      break;
  }

  // ── Nose ─────────────────────────────────────────────────────────────
  const int ny = cy + 4;
  display.drawCircle(cx, ny, 3, WHITE);
  display.drawPixel(cx, ny, WHITE);

  // ── Mouth ─────────────────────────────────────────────────────────────
  const int mby = cy + 11;
  switch (expr) {
    case 0: // gentle smile
      drawBez(cx - 9, mby, cx, mby + 6, cx + 9, mby);
      break;
    case 1: // big grin with teeth
      drawBez(cx - 11, mby - 1, cx, mby + 9, cx + 11, mby - 1);
      display.fillRect(cx - 8, mby, 17, 5, WHITE);
      for (int t = cx - 8; t <= cx + 8; t += 4)
        display.drawLine(t, mby, t, mby + 5, BLACK);
      break;
    case 2: // smirk (right-leaning)
      drawBez(cx - 2, mby + 2, cx + 4, mby + 7, cx + 10, mby);
      break;
    case 3: // flat mouth + drool drop
      display.drawLine(cx - 5, mby + 2, cx + 5, mby + 2, WHITE);
      display.drawLine(cx,     mby + 3, cx,     mby + 8, WHITE);
      display.fillCircle(cx,   mby + 9, 2, WHITE);
      break;
    case 4: // open O mouth
      display.drawCircle(cx, mby + 5, 5, WHITE);
      display.fillCircle(cx, mby + 5, 3, WHITE);
      break;
    case 5: // frown
      drawBez(cx - 9, mby + 5, cx, mby - 1, cx + 9, mby + 5);
      break;
  }

  // ── Whiskers  (3 per side) ────────────────────────────────────────────
  const int wy = cy + 4;
  // left
  display.drawLine(cx - 30, wy - 3, cx - 8, wy,     WHITE);
  display.drawLine(cx - 30, wy + 1, cx - 8, wy + 1, WHITE);
  display.drawLine(cx - 30, wy + 5, cx - 8, wy + 3, WHITE);
  // right
  display.drawLine(cx +  8, wy,     cx + 30, wy - 3, WHITE);
  display.drawLine(cx +  8, wy + 1, cx + 30, wy + 1, WHITE);
  display.drawLine(cx +  8, wy + 3, cx + 30, wy + 5, WHITE);

  // ── Collar band + Bell  (draw whenever the band itself is on-screen) ────
  const int colY = cy + 24;
  if (colY - 3 < SCR_H) {                                     // band top visible
    display.drawRect(cx - 16, colY - 3, 33, 6, WHITE);        // collar band
    display.fillCircle(cx,    colY + 5, 3,  WHITE);            // bell body
    display.drawLine(cx - 2,  colY + 7, cx + 2, colY + 7, BLACK); // bell slit
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Screen renderers
// ─────────────────────────────────────────────────────────────────────────────

static void showFace() {
  display.clearDisplay();
  drawFace(g_faceIdx, 0);

  // Expression label – top-left, above the head (head top ≈ y=9)
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2, 1);
  display.print(EXPR_NAME[g_faceIdx]);

  // Index indicator – top-right
  display.setCursor(SCR_W - 13, 1);
  display.print(g_faceIdx + 1);
  display.print("/6");

  display.display();
}

static void showQuote() {
  display.clearDisplay();

  // ── Tiny face icon ────────────────────────────────────────────────────
  display.drawCircle(7, 7, 6, WHITE);
  display.drawPixel(5,  5, WHITE);   // left eye
  display.drawPixel(9,  5, WHITE);   // right eye
  display.drawPixel(7,  8, WHITE);   // nose
  display.drawLine(5, 10, 9, 10, WHITE); // smile

  // ── Header ────────────────────────────────────────────────────────────
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(17, 1);
  display.print("Quote ");
  display.print(g_quoteIdx + 1);
  display.print("/25");

  // ── Separator ─────────────────────────────────────────────────────────
  display.drawLine(0, 14, SCR_W - 1, 14, WHITE);

  // ── Quote text  (\n handled by GFX cursor) ────────────────────────────
  display.setCursor(2, 17);
  display.print(QUOTES[g_quoteIdx]);

  display.display();
}

static void showGadget() {
  display.clearDisplay();

  // ── Header ────────────────────────────────────────────────────────────
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2, 1);
  display.print("GADGET ");
  display.print(g_gadgetIdx + 1);
  display.print("/10");

  display.drawLine(0, 11, SCR_W - 1, 11, WHITE);

  // ── Gadget name (centred) ─────────────────────────────────────────────
  const char *name = G_NAMES[g_gadgetIdx];
  int nameLen = (int)strlen(name) * 6;
  display.setCursor((SCR_W - nameLen) / 2, 14);
  display.print(name);

  display.drawLine(0, 24, SCR_W - 1, 24, WHITE);

  // ── Description ───────────────────────────────────────────────────────
  display.setCursor(2, 27);
  display.print(G_DESCS[g_gadgetIdx]);

  display.display();
}

static void showDance() {
  display.clearDisplay();

  const uint8_t s  = g_danceStep & (DANCE_FRAMES - 1);
  const int8_t  yo = DANCE_Y[s];
  const uint8_t ex = DANCE_EXPR[s];

  // Face with bounce offset (head centre y = 32+yo; collar guard prevents overflow)
  drawFace(ex, yo);

  // ── Floating music notes (drift left-right with phase) ───────────────
  const int phase = (s * 5) % 22;
  drawNote(4  + phase,  3);
  drawNote(SCR_W - 14 - phase,  5);

  // ── "DANCE!" label centred at the very bottom ─────────────────────────
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(42, SCR_H - 8);
  display.print("DANCE!");

  display.display();
}

// Brief full-screen banner shown when switching modes
static void showModeBanner(const char *label) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  const int len = (int)strlen(label) * 12;
  display.setCursor((SCR_W - len) / 2, (SCR_H - 16) / 2);
  display.print(label);
  display.display();
  delay(700);
}

// Render whichever mode is currently active
static void refreshScreen() {
  switch (g_mode) {
    case FACE:   showFace();   break;
    case QUOTE:  showQuote();  break;
    case GADGET: showGadget(); break;
    case DANCE:  showDance();  break;
    default:     break;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mode / item navigation
// ─────────────────────────────────────────────────────────────────────────────
static const char * const MODE_LABELS[MODE_CNT] = {
  "FACE", "QUOTE", "GADGET", "DANCE"
};

static void nextMode() {
  g_mode      = (Mode)((uint8_t)(g_mode + 1) % (uint8_t)MODE_CNT);
  g_danceStep = 0;
  g_danceAt   = millis();
  showModeBanner(MODE_LABELS[g_mode]);
  g_needDraw  = true;
}

static void nextItem() {
  switch (g_mode) {
    case FACE:
      g_faceIdx  = (g_faceIdx  + 1) % 6;
      showFace();
      break;
    case QUOTE:
      g_quoteIdx = (g_quoteIdx + 1) % 25;
      showQuote();
      break;
    case GADGET:
      g_gadgetIdx = (g_gadgetIdx + 1) % 10;
      showGadget();
      break;
    case DANCE:
      // In DANCE mode a short press resets the animation
      g_danceStep = 0;
      g_danceAt   = millis();
      break;
    default:
      break;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Splash screen
// ─────────────────────────────────────────────────────────────────────────────
static void showSplash() {
  // Happy face on boot
  display.clearDisplay();
  drawFace(1, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(18, SCR_H - 8);
  display.print("Doraemon OLED!");
  display.display();
  delay(1800);

  // Quick blink animation (happy → sleepy × 2)
  for (uint8_t i = 0; i < 2; i++) {
    display.clearDisplay();
    drawFace(3, 0);   // sleepy
    display.display();
    delay(250);
    display.clearDisplay();
    drawFace(1, 0);   // happy
    display.display();
    delay(250);
  }
  delay(400);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Button handler  (called every loop tick)
// ─────────────────────────────────────────────────────────────────────────────
static void handleButton() {
  const bool   cur = (bool)digitalRead(BTN_PIN);   // LOW = pressed
  const uint32_t now = millis();

  // ── Falling edge – button pressed ───────────────────────────────────
  if (g_prevBtn == HIGH && cur == LOW) {
    g_pressAt   = now;
    g_longFired = false;
  }

  // ── Still held – check for long press ────────────────────────────────
  if (cur == LOW && !g_longFired && (now - g_pressAt) >= LONG_MS) {
    g_longFired = true;
    nextMode();
  }

  // ── Rising edge – button released ────────────────────────────────────
  if (g_prevBtn == LOW && cur == HIGH) {
    if (!g_longFired && (now - g_pressAt) >= DEBOUNCE_MS) {
      nextItem();
    }
  }

  g_prevBtn = cur;
}

// ─────────────────────────────────────────────────────────────────────────────
//  setup
// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // Display not found – blink built-in LED indefinitely
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) {
      digitalWrite(LED_BUILTIN, LOW);  delay(200);
      digitalWrite(LED_BUILTIN, HIGH); delay(200);
    }
  }

  display.setRotation(0);
  display.dim(false);
  display.clearDisplay();
  display.display();

  showSplash();
  showFace();

  g_danceAt  = millis();
  g_needDraw = false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  loop
// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  handleButton();

  // Auto-advance dance frames
  if (g_mode == DANCE) {
    const uint32_t now = millis();
    if (now - g_danceAt >= DANCE_TICK) {
      g_danceAt   = now;
      g_danceStep = (g_danceStep + 1) & (DANCE_FRAMES - 1);
      showDance();
    }
  }

  // Deferred redraw after a mode-banner was shown
  if (g_needDraw) {
    g_needDraw = false;
    refreshScreen();
  }

  delay(10);
}
