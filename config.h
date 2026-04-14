#pragma once

// ─── OLED ───────────────────────────────────────────────────────
#define OLED_SDA          4        // D2
#define OLED_SCL          5        // D1
#define OLED_WIDTH        128
#define OLED_HEIGHT       64
#define OLED_RESET        -1
#define OLED_I2C_ADDR     0x3C

// ─── Button ─────────────────────────────────────────────────────
#define BUTTON_PIN        0        // D3 / GPIO0 — active LOW
#define DEBOUNCE_MS       50
#define HOLD_MS           1000

// ─── Display Timing ─────────────────────────────────────────────
#define QUOTE_DISPLAY_MS    5000   // Show each quote for 5 seconds
#define ANIM_FRAME_MS        120   // Animation frame speed (ms)
#define IDLE_SWITCH_MS      8000   // Auto-switch mode after idle

// ─── Modes ──────────────────────────────────────────────────────
enum AppMode {
  MODE_FACE,       // Animated Doraemon face cycling expressions
  MODE_QUOTE,      // Doraemon quote with small face
  MODE_GADGET,     // "Gadget of the day" screen
  MODE_DANCE,      // Doraemon dance animation
};

// ─── Debug ──────────────────────────────────────────────────────
#define DEBUG_BAUD  115200
