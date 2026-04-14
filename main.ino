#include "config.h"
#include "button.h"
#include "renderer.h"
#include "quotes.h"

// ─── Objects ───────────────────────────────────────────────────
Button   btn;
Renderer renderer;

// ─── State ─────────────────────────────────────────────────────
AppMode       mode          = MODE_FACE;
int           animFrame     = 0;
int           quoteIdx      = 0;
int           gadgetIdx     = 0;
unsigned long lastFrameTime = 0;
unsigned long lastModeSwitch = 0;
unsigned long lastQuoteTime = 0;

// ─── Helpers ───────────────────────────────────────────────────
void setMode(AppMode m) {
  mode         = m;
  animFrame    = 0;
  lastModeSwitch = millis();
  const char* names[] = {"FACE","QUOTE","GADGET","DANCE"};
  Serial.print("[MODE] -> ");
  Serial.println(names[(int)m]);
}

void nextMode() {
  setMode((AppMode)(((int)mode + 1) % 4));
}

// ─── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(DEBUG_BAUD);
  delay(100);
  Serial.println("\n\n== Doraemon OLED v1.0 ==");

  btn.begin();

  if (!renderer.begin()) {
    Serial.println("[SETUP] Display FATAL");
    while (1) yield();
  }

  renderer.drawBoot();
  delay(2500);

  // Start with a random quote
  quoteIdx   = (int)(millis() % QUOTE_COUNT);
  gadgetIdx  = (int)(millis() % GADGET_COUNT);

  setMode(MODE_FACE);
  lastQuoteTime = millis();
  Serial.println("[SETUP] Ready — press to cycle modes, hold for gadget!");
}

// ─── Loop ──────────────────────────────────────────────────────
void loop() {
  btn.update();
  unsigned long now = millis();

  // ── Button: press = next mode ──────────────────────────────
  if (btn.wasPressed()) {
    nextMode();
    if (mode == MODE_QUOTE) {
      quoteIdx = (quoteIdx + 1) % QUOTE_COUNT;
      lastQuoteTime = now;
    }
    if (mode == MODE_GADGET) {
      gadgetIdx = (gadgetIdx + 1) % GADGET_COUNT;
    }
  }

  // ── Button: hold = jump to dance mode ──────────────────────
  if (btn.wasHeld()) {
    setMode(MODE_DANCE);
  }

  // ── Auto-advance quote ─────────────────────────────────────
  if (mode == MODE_QUOTE && now - lastQuoteTime >= QUOTE_DISPLAY_MS) {
    quoteIdx = (quoteIdx + 1) % QUOTE_COUNT;
    lastQuoteTime = now;
    animFrame = 0;
    Serial.print("[QUOTE] -> ");
    Serial.println(quoteIdx);
  }

  // ── Auto-switch mode after idle ────────────────────────────
  if (now - lastModeSwitch >= IDLE_SWITCH_MS) {
    nextMode();
    if (mode == MODE_QUOTE) {
      quoteIdx = (quoteIdx + 1) % QUOTE_COUNT;
      lastQuoteTime = now;
    }
    if (mode == MODE_GADGET) {
      gadgetIdx = (int)(now % GADGET_COUNT);
    }
  }

  // ── Animation tick ─────────────────────────────────────────
  if (now - lastFrameTime >= ANIM_FRAME_MS) {
    lastFrameTime = now;
    animFrame++;

    switch (mode) {
      case MODE_FACE:   renderer.drawFace(animFrame);                       break;
      case MODE_QUOTE:  renderer.drawQuote(quoteIdx, animFrame);            break;
      case MODE_GADGET: renderer.drawGadget(gadgetIdx);                     break;
      case MODE_DANCE:  renderer.drawDance(animFrame);                      break;
    }
  }

  yield();
}
