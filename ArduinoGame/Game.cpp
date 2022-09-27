#pragma once
#include "Game.h"
#include "World.h"
#include "Constants.h"
#include "ImageData.h"
#include "Player.h"
#include "Intro.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "FreeMemory.h"

// as per https://stackoverflow.com/questions/24970540/undefined-reference-during-linkage-for-a-static-pointer-class-member-in-c/24970621
// it won't compile without this
Adafruit_ST7735* Game::tft = NULL; 

// function that resets Arduino
void(* resetFunc) (void) = 0;

Game::Game() {
  
}

Game::~Game() {
  
}

void Game::init(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_RST, uint8_t UP, uint8_t RIGHT, uint8_t DOWN, uint8_t LEFT,
                uint8_t RAND1, uint8_t RAND2, uint8_t LEDS[], uint8_t LEDS_LENGTH, uint16_t background) {
  
  pinMode(UP, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);

  pinMode(RAND1, INPUT);
  pinMode(RAND2, INPUT);

  for (int8_t i = 0; i < LEDS_LENGTH; ++i) {
    pinMode(LEDS[i], OUTPUT);
  }

  for (int8_t i = 0; i < LEDS_LENGTH; ++i) {
    digitalWrite(LEDS[i], LOW);
  }

  this->bgColor = background;
  
  tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  
  // Init 1.44" TFT ST7735R chip, green tab
  tft->initR(INITR_144GREENTAB);
  
  tft->fillScreen(BLACK);
  tft->setTextColor(WHITE);
  tft->setCursor(0, 0);

  this->frog = new Player(ImageData::FrogColors, 64, 64, 16, 16, 3);
  this->frog->setState(PlayerState::Idle);

  tft->setCursor(4, 4);
  tft->println(F("Generating world..."));
  this->world = new World(RAND1, RAND2, LEDS_LENGTH);
  tft->setCursor(4, 16);
  tft->println(F("Done!"));

  Intro::play(background);
}

void Game::handleEvents(uint8_t UP, uint8_t RIGHT, uint8_t DOWN, uint8_t LEFT) {
  up = digitalRead(UP) == PRESSED;
  right = digitalRead(RIGHT) == PRESSED;
  down = digitalRead(DOWN) == PRESSED;
  left = digitalRead(LEFT) == PRESSED;
}

void Game::update(int frameTime, uint8_t LEDS_LENGTH) {
  
  if (victory) {
    if (millis() - momentOfVictory > 3500) {
      if (up || down || left || right) {
        resetFunc();
      }
    }
    return;
  }
  
  if (this->frog->getState() != Eating) {
    const int8_t SPEED = 8;
    int8_t vectorX = 0;
    int8_t vectorY = 0;

    if (up) {
      this->world->scrollY(-SPEED);
      vectorY += 1;
    }
    if (right) {
      this->world->scrollX(SPEED);
      vectorX += 1;
    }
    if (down) {
      this->world->scrollY(SPEED);
      vectorY -=1;
    }
    if (left) {
      this->world->scrollX(-SPEED);
      vectorX -=1;
    }

    if (vectorX == 0 && vectorY == 0) {
      if (this->frog->getIdleFrames() < (ImageData::SizeofFrogIdle / 2 * 3)) {
        this->frog->setState(Idle);
      }
      else {
        this->frog->setState(Sleeping);
      }
    }
    else if (vectorX == 0 && vectorY == 1) {
      this->frog->setState(MovingUp);
    }
    else if (vectorX == 0 && vectorY == -1) {
      this->frog->setState(MovingDown);
    }
    else if (vectorX == 1) {
      this->frog->setState(MovingRight);
    }
    else if (vectorX == -1) {
      this->frog->setState(MovingLeft);
    }
  }

  this->world->updateTiles();
  if (this->world->collectCoin()) {
    this->coins++;
    this->frog->setState(Eating);
  }

  this->frog->update();

  if (this->coins >= LEDS_LENGTH && this->frog->getState() != Eating) {
    this->victory = true;
    this->momentOfVictory = millis();
  }
}

void Game::render(uint8_t LEDS[], uint8_t LEDS_LENGTH) {

  if (victory) {
    tft->setTextColor(tft->color565(abs(255 - (int)((millis()/2) % 510)), abs(255 - (int)((millis()/3) % 510)), abs(255 - (int)((millis()/5) % 510))));
    tft->setCursor(2,48);
    tft->setTextSize(3);
    tft->println(F("VICTORY"));
    if (millis() - momentOfVictory > 3000) {
      tft->setCursor(12, 80);
      tft->setTextSize(1);
      tft->println(F("Press any button"));
      tft->setCursor(12, 92);
      tft->println(F("to start again..."));
    }

    for (int8_t i = 0; i < LEDS_LENGTH; ++i) {
      digitalWrite(LEDS[i], ((millis() - momentOfVictory + (i * 100)) % 1000) > 500 ? LOW : HIGH);
    }
    return;
  }

  for (int8_t i = 0; i < LEDS_LENGTH; ++i) {
    digitalWrite(LEDS[i], this->coins >= (i + 1));
  }
  
  tft->fillScreen(bgColor);
  this->world->renderVisibleTiles();
  this->frog->render();

  //#define DISPLAY_COINS_COUNT
  #ifdef DISPLAY_COINS_COUNT
  tft->setCursor(2,2);
  tft->print(F("Coins: "));
  tft->println(this->coins);
  #endif

  //#define DISPLAY_FREE_RAM
  #ifdef DISPLAY_FREE_RAM
  tft->setCursor(2, 119);
  tft->print(F("Free RAM: "));
  tft->print(freeMemory());
  tft->println(F(" B"));
  #endif
}
