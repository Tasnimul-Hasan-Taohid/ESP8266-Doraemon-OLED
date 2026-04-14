#include "renderer.h"

Renderer::Renderer()
  : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET) {}

bool Renderer::begin() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println("[RENDERER] Failed");
    return false;
  }
  _display.clearDisplay();
  _display.setTextColor(SSD1306_WHITE);
  _display.display();
  Serial.println("[RENDERER] OK");
  return true;
}

// ─── Boot Screen ───────────────────────────────────────────────
void Renderer::drawBoot() {
  _display.clearDisplay();
  _drawDoraemonFace(32, 32, 24, 0, 0);
  _display.setTextSize(1);
  _drawCentered("Doraemon!", 4);
  _drawCentered("v1.0", 54);
  _display.display();
}

// ─── MODE: Big animated face ────────────────────────────────────
// Cycles through 6 expressions as frame advances
void Renderer::drawFace(int frame) {
  _display.clearDisplay();

  int expr = (frame / 8) % 6;   // Change expression every 8 frames
  int r    = 26;
  int cx   = 64, cy = 30;

  _drawDoraemonFace(cx, cy, r, expr, frame);

  // Expression label bottom
  const char* labels[] = {
    "(^o^)  Normal",
    "(^▽^)  Happy!",
    "(O_O)  Shocked!",
    "(-_-)  Sleepy...",
    "(>_<)  Angry!",
    "(^o^)  Dorayaki!!",
  };
  _display.setTextSize(1);
  _drawCentered(labels[expr], 57);

  _display.display();
}

// ─── MODE: Quote + small face ───────────────────────────────────
void Renderer::drawQuote(int quoteIdx, int frame) {
  _display.clearDisplay();

  const char* quote = DORAEMON_QUOTES[quoteIdx % QUOTE_COUNT];

  // Small animated face (left column)
  int expr = (frame / 12) % 6;
  _drawDoraemonFace(18, 28, 14, expr, frame);

  // Divider
  _display.drawFastVLine(36, 0, 64, SSD1306_WHITE);

  // Quote text (right side)
  _drawWrappedSmall(quote, 40, 4, 87);

  // Tiny quote number bottom left
  _display.setTextSize(1);
  _display.setCursor(0, 56);
  _display.print(quoteIdx % QUOTE_COUNT + 1);
  _display.print("/");
  _display.print(QUOTE_COUNT);

  _display.display();
}

// ─── MODE: Gadget of the day ────────────────────────────────────
void Renderer::drawGadget(int gadgetIdx) {
  _display.clearDisplay();
  const Gadget& g = GADGETS[gadgetIdx % GADGET_COUNT];

  // Header
  _display.fillRect(0, 0, OLED_WIDTH, 11, SSD1306_WHITE);
  _display.setTextColor(SSD1306_BLACK);
  _display.setTextSize(1);
  _display.setCursor(2, 2);
  _display.print("Gadget of the Day");
  _display.setTextColor(SSD1306_WHITE);

  // Gadget name
  _display.setTextSize(1);
  _drawCentered(String(g.name), 15);
  _display.drawFastHLine(10, 24, 108, SSD1306_WHITE);

  // Effect
  _display.setCursor(2, 27);
  _display.print("Effect:");
  _drawWrappedSmall(g.effect, 2, 36, 124);

  // Warning
  _display.drawFastHLine(10, 46, 108, SSD1306_WHITE);
  _display.setCursor(2, 48);
  _display.print("Warning:");
  _drawWrappedSmall(g.warning, 2, 56, 124);

  _display.display();
}

// ─── MODE: Dance animation ──────────────────────────────────────
// Full body Doraemon doing a little bounce dance
void Renderer::drawDance(int frame) {
  _display.clearDisplay();

  int bounce = (frame % 4 < 2) ? 0 : 2;  // body bounces up/down
  int armSwing = (frame % 6 < 3) ? -1 : 1; // arms swing L/R

  int cx = 64;
  int cy = 34 + bounce;

  _drawDoraemonBody(cx, cy, frame);
  _drawDoraemonFace(cx, cy - 20, 13, 1 + (frame % 3), frame);

  // Dance notes
  int noteX = 94 + ((frame * 3) % 20);
  int noteY = 10 + (frame % 8);
  _display.setCursor(noteX % 110, noteY);
  _display.setTextSize(1);
  _display.print((frame % 2 == 0) ? "♪" : "♫");

  // "Doraemon dance!" text
  _drawCentered("Doraemon Dance!", 57);

  _display.display();
}

// ─── Private: Full Doraemon face ───────────────────────────────
void Renderer::_drawDoraemonFace(int cx, int cy, int r, int expression, int frame) {
  // Outer head circle
  _display.fillCircle(cx, cy, r, SSD1306_WHITE);
  // Inner face area (slightly inset top of head)
  // Red nose (circle)
  _drawNose(cx, cy, r);
  // Eyes
  _drawEyes(cx, cy, r, expression, frame);
  // Mouth
  _drawMouth(cx, cy, r, expression);
  // Whiskers (3 each side)
  _drawWhiskers(cx, cy, r);
  // Collar + bell
  _drawBell(cx, cy + r);
}

// ─── Private: Doraemon body for dance mode ─────────────────────
void Renderer::_drawDoraemonBody(int cx, int cy, int frame) {
  int bounce = (frame % 4 < 2) ? 0 : 1;

  // Body (big rounded rect)
  _display.fillRoundRect(cx - 18, cy - 4 + bounce, 36, 28, 8, SSD1306_WHITE);

  // Pocket (semi-circle outline on body)
  _drawPocket(cx, cy + 14 + bounce);

  // Left arm
  int laX = cx - 18;
  int laY = cy + 2 + bounce;
  int armOff = (frame % 6 < 3) ? -3 : 3;
  _display.fillCircle(laX - 6, laY + 6 + armOff, 5, SSD1306_WHITE);

  // Right arm
  int raX = cx + 18;
  _display.fillCircle(raX + 6, laY + 6 - armOff, 5, SSD1306_WHITE);

  // Legs
  int legBounce = (frame % 4 < 2) ? 0 : 2;
  _display.fillRoundRect(cx - 14, cy + 22 + bounce, 10, 12 - legBounce, 4, SSD1306_WHITE);
  _display.fillRoundRect(cx + 4,  cy + 22 + bounce, 10, 12 + legBounce, 4, SSD1306_WHITE);

  // Feet
  _display.fillEllipse(cx - 9,  cy + 34 + bounce, 8, 4, SSD1306_WHITE);
  _display.fillEllipse(cx + 9,  cy + 34 + bounce, 8, 4, SSD1306_WHITE);
}

// ─── Private: Eyes ─────────────────────────────────────────────
void Renderer::_drawEyes(int cx, int cy, int r, int expr, int frame) {
  int eLY = cy - r/3;
  int eLX = cx - r/3;
  int eRX = cx + r/3;
  int er  = max(2, r/5);

  switch (expr) {
    case 0: // Normal — round eyes
      _display.fillCircle(eLX, eLY, er,     SSD1306_BLACK);
      _display.fillCircle(eRX, eLY, er,     SSD1306_BLACK);
      // Eye whites
      _display.fillCircle(eLX, eLY, er + 1, SSD1306_WHITE);
      _display.fillCircle(eRX, eLY, er + 1, SSD1306_WHITE);
      _display.fillCircle(eLX, eLY, er,     SSD1306_BLACK);
      _display.fillCircle(eRX, eLY, er,     SSD1306_BLACK);
      // Pupils
      _display.fillCircle(eLX + 1, eLY, er - 1, SSD1306_WHITE);
      _display.fillCircle(eRX + 1, eLY, er - 1, SSD1306_WHITE);
      break;

    case 1: // Happy — curved/closed eyes (^▽^)
      _display.drawFastHLine(eLX - er, eLY, er * 2, SSD1306_BLACK);
      _display.drawFastHLine(eRX - er, eLY, er * 2, SSD1306_BLACK);
      for (int i = -er; i <= er; i++)
        _display.drawPixel(eLX + i, eLY - abs(i) / 2, SSD1306_BLACK);
      for (int i = -er; i <= er; i++)
        _display.drawPixel(eRX + i, eLY - abs(i) / 2, SSD1306_BLACK);
      break;

    case 2: { // Shocked — huge round eyes (O_O)
      int er2 = er + 2;
      _display.fillCircle(eLX, eLY, er2,     SSD1306_WHITE);
      _display.fillCircle(eRX, eLY, er2,     SSD1306_WHITE);
      _display.drawCircle(eLX, eLY, er2,     SSD1306_BLACK);
      _display.drawCircle(eRX, eLY, er2,     SSD1306_BLACK);
      _display.fillCircle(eLX, eLY, er - 1,  SSD1306_BLACK);
      _display.fillCircle(eRX, eLY, er - 1,  SSD1306_BLACK);
      break;
    }

    case 3: // Sleepy — half-closed (blink animation)
      _display.fillCircle(eLX, eLY, er + 1, SSD1306_WHITE);
      _display.fillCircle(eRX, eLY, er + 1, SSD1306_WHITE);
      _display.drawCircle(eLX, eLY, er + 1, SSD1306_BLACK);
      _display.drawCircle(eRX, eLY, er + 1, SSD1306_BLACK);
      // Half-cover with black rect (droopy eyelids)
      _display.fillRect(eLX - er - 1, eLY - er - 1, er * 2 + 2, er + 1, SSD1306_BLACK);
      _display.fillRect(eRX - er - 1, eLY - er - 1, er * 2 + 2, er + 1, SSD1306_BLACK);
      break;

    case 4: // Angry — slanted eyebrows + squint
      _display.fillCircle(eLX, eLY, er,     SSD1306_BLACK);
      _display.fillCircle(eRX, eLY, er,     SSD1306_BLACK);
      _display.fillCircle(eLX, eLY, er + 1, SSD1306_WHITE);
      _display.fillCircle(eRX, eLY, er + 1, SSD1306_WHITE);
      _display.fillCircle(eLX, eLY, er,     SSD1306_BLACK);
      _display.fillCircle(eRX, eLY, er,     SSD1306_BLACK);
      // Angry eyebrows (diagonal lines)
      _display.drawLine(eLX - er, eLY - er - 2, eLX + er, eLY - er + 1, SSD1306_BLACK);
      _display.drawLine(eRX - er, eLY - er + 1, eRX + er, eLY - er - 2, SSD1306_BLACK);
      break;

    case 5: // Dorayaki love — heart eyes
      // Left heart
      _display.fillCircle(eLX - 1, eLY - 1, er - 1, SSD1306_BLACK);
      _display.fillCircle(eLX + 1, eLY - 1, er - 1, SSD1306_BLACK);
      for (int i = 0; i <= er; i++)
        _display.drawPixel(eLX + i - er/2, eLY + i/2, SSD1306_BLACK);
      // Right heart
      _display.fillCircle(eRX - 1, eLY - 1, er - 1, SSD1306_BLACK);
      _display.fillCircle(eRX + 1, eLY - 1, er - 1, SSD1306_BLACK);
      for (int i = 0; i <= er; i++)
        _display.drawPixel(eRX + i - er/2, eLY + i/2, SSD1306_BLACK);
      break;
  }
}

// ─── Private: Mouth ────────────────────────────────────────────
void Renderer::_drawMouth(int cx, int cy, int r, int expr) {
  int mY = cy + r / 3;
  int mW = r * 2 / 3;

  switch (expr) {
    case 0: // Normal smile
      for (int i = -mW; i <= mW; i++)
        _display.drawPixel(cx + i, mY + (i * i) / (mW * 2), SSD1306_BLACK);
      break;

    case 1: // Big happy grin
      for (int i = -mW; i <= mW; i++) {
        _display.drawPixel(cx + i, mY + (i * i) / (mW),     SSD1306_BLACK);
        _display.drawPixel(cx + i, mY + (i * i) / (mW) + 1, SSD1306_BLACK);
      }
      // Teeth
      _display.fillRect(cx - mW/2, mY + 1, mW, 3, SSD1306_WHITE);
      _display.drawFastHLine(cx, mY + 1, 1, SSD1306_BLACK);
      break;

    case 2: // Shocked open mouth (O)
      _display.drawCircle(cx, mY + 2, mW / 2, SSD1306_BLACK);
      break;

    case 3: // Sleepy flat line with ZZZ
      _display.drawFastHLine(cx - mW/2, mY, mW, SSD1306_BLACK);
      _display.setCursor(cx + mW/2 + 2, mY - 4);
      _display.setTextSize(1);
      _display.setTextColor(SSD1306_BLACK);
      _display.print("z");
      _display.setTextColor(SSD1306_WHITE);
      break;

    case 4: // Angry grimace
      _display.drawFastHLine(cx - mW/2, mY, mW, SSD1306_BLACK);
      _display.drawFastHLine(cx - mW/2, mY + 1, mW, SSD1306_BLACK);
      // Teeth showing
      for (int i = 0; i < 4; i++)
        _display.drawFastVLine(cx - mW/2 + 2 + i*4, mY - 2, 3, SSD1306_BLACK);
      break;

    case 5: // Dorayaki — licking lips
      for (int i = -mW; i <= mW; i++)
        _display.drawPixel(cx + i, mY + (i * i) / (mW * 2), SSD1306_BLACK);
      // Tongue licking
      _display.fillEllipse(cx + mW/2, mY + 3, 3, 2, SSD1306_BLACK);
      break;
  }
}

// ─── Private: Nose ─────────────────────────────────────────────
void Renderer::_drawNose(int cx, int cy, int r) {
  int nR = max(2, r / 7);
  _display.fillCircle(cx, cy - r / 6, nR, SSD1306_BLACK);
}

// ─── Private: Whiskers ─────────────────────────────────────────
void Renderer::_drawWhiskers(int cx, int cy, int r) {
  int wY = cy + r / 6;
  int wLen = r - 4;
  // Left whiskers
  _display.drawLine(cx - 4, wY - 3, cx - 4 - wLen, wY - 5, SSD1306_BLACK);
  _display.drawLine(cx - 4, wY,     cx - 4 - wLen, wY,     SSD1306_BLACK);
  _display.drawLine(cx - 4, wY + 3, cx - 4 - wLen, wY + 5, SSD1306_BLACK);
  // Right whiskers
  _display.drawLine(cx + 4, wY - 3, cx + 4 + wLen, wY - 5, SSD1306_BLACK);
  _display.drawLine(cx + 4, wY,     cx + 4 + wLen, wY,     SSD1306_BLACK);
  _display.drawLine(cx + 4, wY + 3, cx + 4 + wLen, wY + 5, SSD1306_BLACK);
}

// ─── Private: Bell ─────────────────────────────────────────────
void Renderer::_drawBell(int cx, int bellY) {
  // Collar line
  _display.drawFastHLine(cx - 18, bellY, 36, SSD1306_BLACK);
  _display.drawFastHLine(cx - 18, bellY + 1, 36, SSD1306_BLACK);
  // Bell circle
  _display.fillCircle(cx, bellY + 5, 4, SSD1306_BLACK);
  _display.fillCircle(cx, bellY + 5, 2, SSD1306_WHITE);
  // Bell dot
  _display.drawPixel(cx, bellY + 7, SSD1306_BLACK);
}

// ─── Private: Pocket ───────────────────────────────────────────
void Renderer::_drawPocket(int cx, int cy) {
  _display.drawCircle(cx, cy, 8, SSD1306_BLACK);
  _display.fillRect(cx - 8, cy, 17, 9, SSD1306_WHITE); // hide bottom half
  _display.drawFastHLine(cx - 8, cy, 17, SSD1306_BLACK); // pocket opening
}

// ─── Private: Centered text ────────────────────────────────────
void Renderer::_drawCentered(const String& text, int y, uint8_t size) {
  _display.setTextSize(size);
  _display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1; uint16_t w, h;
  _display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = max(0, (int)((OLED_WIDTH - (int)w) / 2));
  _display.setCursor(x, y);
  _display.print(text);
}

// ─── Private: Wrapped small text ───────────────────────────────
void Renderer::_drawWrappedSmall(const char* text, int x, int y, int maxW) {
  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  int curX = x, curY = y;
  String line = "";
  const char* p = text;

  while (*p) {
    if (*p == '\n') {
      _display.setCursor(curX, curY);
      _display.print(line);
      line = "";
      curX = x;
      curY += 9;
      p++;
      continue;
    }
    line += *p;
    if ((int)line.length() * 6 >= maxW) {
      // find last space
      int sp = line.lastIndexOf(' ');
      if (sp > 0) {
        _display.setCursor(curX, curY);
        _display.print(line.substring(0, sp));
        line = line.substring(sp + 1);
      } else {
        _display.setCursor(curX, curY);
        _display.print(line);
        line = "";
      }
      curX = x;
      curY += 9;
    }
    p++;
  }
  if (line.length() > 0) {
    _display.setCursor(curX, curY);
    _display.print(line);
  }
}
