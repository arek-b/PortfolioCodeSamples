#pragma once
#include "Constants.h"
#include "Player.h"
#include "World.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

class Game {
  public:
    static Adafruit_ST7735* tft;
    Game();
    ~Game();
    void init(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_RST, uint8_t UP, uint8_t RIGHT, uint8_t DOWN, uint8_t LEFT,
              uint8_t RAND1, uint8_t RAND2, uint8_t LEDS[], uint8_t LEDS_LENGTH, uint16_t background);
    void handleEvents(uint8_t UP, uint8_t RIGHT, uint8_t DOWN, uint8_t LEFT);
    void update(int frameTime, uint8_t LEDS_LENGTH);
    void render(uint8_t LEDS[], uint8_t LEDS_LENGTH);
    bool getVictory() { return victory; }
  private:
    uint16_t bgColor = BLACK;
    bool up = false;
    bool right = false;
    bool down = false;
    bool left = false;
    Player* frog;
    World* world;
    int8_t coins = 0;
    bool victory = false;
    uint32_t momentOfVictory;
};
