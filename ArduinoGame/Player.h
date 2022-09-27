#pragma once
#include <Adafruit_ST7735.h>
#include "ImageData.h"

enum PlayerState { MovingUp, MovingRight, MovingDown, MovingLeft, Idle, Sleeping, Eating, FinishedEating };

class Player {
  public:
    Player(uint16_t bitmapColors[], int16_t posX, int16_t posY, int16_t resX, int16_t resY, uint8_t scale);
    void update();
    void render();
    void setState(PlayerState playerState);
    PlayerState getState() { return this->state; }
    uint16_t getIdleFrames() { return this->idleFrames; }
  private:
    int16_t posX, posY, posOffsetX, posOffsetY;
    uint8_t resX, resY, scale, currentFrame;
    PlayerState state;
    int8_t bitmapCount;
    const unsigned char** bitmaps;
    uint16_t* bitmapColors;
    uint16_t idleFrames = 0;
};
