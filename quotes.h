#pragma once
#include <Arduino.h>

// ─── Quotes ─────────────────────────────────────────────────────
// Funny, motivational and iconic Doraemon-style quotes

static const char* const DORAEMON_QUOTES[] = {
  "I ate the gadget\nagain. My bad.",
  "Nobita! Do your\nhomework NOW!",
  "Out of gadgets.\nPlease restock.",
  "I came from\nthe future.\nStill no wifi.",
  "My pocket has\neverything except\na girlfriend.",
  "Nobita cried\n3 times today.\nNew record.",
  "Dorayaki count:\n47. Don't judge.",
  "I'm a cat-robot.\nI don't do Mondays.",
  "Shizuka is\ntoo good for\nall of you.",
  "Giant punched me.\nI deserved it.",
  "The future\nis bright!\n(for everyone but Nobita)",
  "Memory bread:\nfor those who\nforgot everything.",
  "Anywhere door:\nbecause walking\nis overrated.",
  "Take-copter:\nflight attendants\nhate him.",
  "Small light:\nbig light:\nbig problems.",
  "Nobita got\n0% again.\nI'm so tired.",
  "Sewashi sent me\nto fix Nobita.\nSend help.",
  "I have no ears.\nYet I hear\neverything.",
  "My tail is\na screwdriver.\nYou're welcome.",
  "Dorayaki > pizza.\nFight me.",
  "Time machine?\nUsed it to skip\nmonday. Worth it.",
  "I believe in you!\n(but also bring\nbackup gadgets)",
  "Suneo is showing\noff again.\nClassic Suneo.",
  "Even robots\nhave bad days.\nToday is mine.",
  "Friendship is\nthe best gadget\nI ever had.",
};

static const int QUOTE_COUNT = sizeof(DORAEMON_QUOTES) / sizeof(DORAEMON_QUOTES[0]);

// ─── Gadget of the Day ─────────────────────────────────────────

struct Gadget {
  const char* name;
  const char* effect;
  const char* warning;
};

static const Gadget GADGETS[] = {
  { "Anywhere Door",  "Go anywhere!",      "May end up in\nsomeone's closet." },
  { "Take-copter",    "Fly like a bird!",  "Helmet hair\nnot included." },
  { "Small Light",    "Shrink anything!",  "Don't sit on\nyour shrunken sofa." },
  { "Memory Bread",   "Learn by eating!",  "Side effect:\ncraving toast 24/7." },
  { "Dorayaki Gun",   "Fires dorayaki!",   "Extremely\ndangerous to diets." },
  { "Copy Mirror",    "Clone yourself!",   "Clone may be\nsmarter than you." },
  { "Time Cloth",     "Age or de-age!",    "Do NOT use on\nyour homework." },
  { "Air Cannon",     "Blast of air!",     "Suneo's hair\nwill not survive." },
  { "Bully Radar",    "Detects Gian!",     "Always\nbuzzing." },
  { "Translator Jelly","Speak any lang!",  "May cause you\nto speak gibberish." },
};

static const int GADGET_COUNT = sizeof(GADGETS) / sizeof(GADGETS[0]);
