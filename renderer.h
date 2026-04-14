#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "quotes.h"

class Renderer {
public:
  Renderer();
  bool begin();

  // Modes
  void drawFace(int frame);           // Animated face — big centred
  void drawQuote(int quoteIdx, int frame);   // Quote + small face
  void drawGadget(int gadgetIdx);     // Gadget of the day
  void drawDance(int frame);          // Full body dance animation
  void drawBoot();                    // Boot splash

private:
  Adafruit_SSD1306 _display;

  // Face drawing
  void _drawDoraemonFace(int cx, int cy, int r, int expression, int frame);
  void _drawDoraemonBody(int cx, int cy, int frame);

  // Expressions: 0=normal 1=happy 2=shocked 3=sleepy 4=angry 5=dorayaki
  void _drawEyes(int cx, int cy, int r, int expr, int frame);
  void _drawMouth(int cx, int cy, int r, int expr);
  void _drawWhiskers(int cx, int cy, int r);
  void _drawNose(int cx, int cy, int r);
  void _drawBell(int cx, int cy);
  void _drawPocket(int cx, int cy);

  void _drawCentered(const String& text, int y, uint8_t size = 1);
  void _drawWrappedSmall(const char* text, int x, int y, int maxW);
  int  _countLines(const char* text);
};
