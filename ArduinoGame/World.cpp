#pragma once

#include "World.h"
#include <Arduino.h>
#include "ImageData.h"
#include "Helpers.h"

World::World(uint8_t RAND1, uint8_t RAND2, int8_t minCoinAmount) {
  //#define DEBUG_WORLD_CONSTRUCT_PRE_GENERATE
  #ifdef DEBUG_WORLD_CONSTRUCT_PRE_GENERATE
  Game::tft->fillScreen(0x0000);
  Game::tft->setCursor(0, 0);
  Game::tft->print("Pixel pos.X(pre): ");
  Game::tft->println(this->pixelPositionX);
  Game::tft->print("Pixel pos.Y(pre): ");
  Game::tft->println(this->pixelPositionY);
  #endif
  
  this->generate(RAND1, RAND2, minCoinAmount);
  
  //#define DEBUG_WORLD_CONSTRUCT_POST_GENERATE
  #ifdef DEBUG_WORLD_CONSTRUCT_POST_GENERATE
  Game::tft->fillScreen(0x0000);
  Game::tft->setCursor(0, 0);
  Game::tft->print("Pixel pos.X(post): ");
  Game::tft->println(this->pixelPositionX);
  Game::tft->print("Pixel pos.Y(post): ");
  Game::tft->println(this->pixelPositionY);
  delay(1000);
  #endif
}

void World::setTile(int8_t x, int8_t y, Tile* tile) {
  if (tile == this->tileCoin) {
    this->tiles[x][y] = 3;
  }
  else if (tile == this->tileDecorA) {
    this->tiles[x][y] = 1;
  }
  else if (tile == this->tileDecorB) {
    this->tiles[x][y] = 2;
  }
  else {
    this->tiles[x][y] = 0;
  }
}

Tile* World::getTile(int8_t x, int8_t y) {
  switch(this->tiles[x][y]) {
    case 1:
      return this->tileDecorA;
    case 2:
      return this->tileDecorB;
    case 3:
      return this->tileCoin;
    default:
      return nullptr;
  }
}

void World::scrollX(int8_t x) {
  this->pixelPositionX += x;
}

void World::scrollY(int8_t y) {
  this->pixelPositionY += y;
}

void World::generate(uint8_t RAND1, uint8_t RAND2, int8_t minCoinAmount) {
  //#define DEBUG_WG_PRE_RAND_SEED
  #ifdef DEBUG_WG_PRE_RAND_SEED
  Game::tft->fillScreen(0x0000);
  Game::tft->setCursor(0, 0);
  Game::tft->println("Pre rand seed OK");
  delay(100);
  #endif
  
  randomSeed(analogRead(RAND1) * analogRead(RAND2));
  
  //#define DEBUG_WG_POST_RAND_SEED
  #ifdef DEBUG_WG_POST_RAND_SEED
  Game::tft->println("Post rand seed OK");
  delay(100);
  #endif
  
  //#define DEBUG_WG_LOOP
  #ifdef DEBUG_WG_LOOP
  Game::tft->fillScreen(0x0000);
  Game::tft->setCursor(0, 0);
  Game::tft->setTextWrap(true);
  Game::tft->setTextSize(0);
  #endif

  int16_t coinGuaranteedEvery = (int16_t)TILES_X * (int16_t)TILES_Y / (int16_t)minCoinAmount - 1;
  int16_t tilesWithoutCoin = 0;
  int16_t totalCoins = 0;
  
  for(int i = 0; i < TILES_X; ++i) {
    for (int j = 0; j < TILES_Y; ++j) {
      
      #ifdef DEBUG_WG_LOOP
      //Game::tft->print('|');
      //Game::tft->print(i);
      //Game::tft->print(';');
      //Game::tft->print(j);
      Game::tft->print(i+j);
      #endif
      
      if (i == TILES_X / 2 && j == TILES_Y / 2) {
        #ifdef DEBUG_WG_LOOP
        Game::tft->print("<-CTR");
        #endif
        
        this->setTile(i, j, nullptr);
        tilesWithoutCoin++;
        continue;
      }

      uint8_t randomNumber = random(1, 32);

      //#define DEBUG_RANDOM_NUMBERS
      #ifdef DEBUG_RANDOM_NUMBERS
      Game::tft->print(randomNumber);
      Game::tft->print('|');
      delay(10);
      #endif;

      if (tilesWithoutCoin >= coinGuaranteedEvery) {
        randomNumber = 1;
      }
      
      switch(randomNumber) {
        case 1:
          this->setTile(i, j, this->tileCoin);
          tilesWithoutCoin = 0;
          totalCoins++;
          break;
        case 10: case 11: case 12: case 13: case 14: case 15: case 16: //case 17: case 18: case 19:
          this->setTile(i, j, this->tileDecorA);
          tilesWithoutCoin++;
          break;
        case 20: case 21: case 22: case 23: case 24: case 25: case 26: //case 27: case 28: case 29:
          this->setTile(i, j, this->tileDecorB);
          tilesWithoutCoin++;
          break;
        default:
          this->setTile(i, j, nullptr);
          tilesWithoutCoin++;
          break;
      }

      //#define DEBUG_COIN_GUARANTEE
      #ifdef DEBUG_COIN_GUARANTEE
      Game::tft->fillScreen(0x0000);
      Game::tft->setCursor(0,0);
      Game::tft->print("Coin guar. every: ");
      Game::tft->println(coinGuaranteedEvery);
      Game::tft->print("Tiles w/o coin: ");
      Game::tft->println(tilesWithoutCoin);
      Game::tft->print("Total coins: ");
      Game::tft->println(totalCoins);
      delay(200);
      #endif
    }
  }

  #ifdef DEBUG_COIN_GUARANTEE
  Game::tft->fillScreen(0x0000);
  Game::tft->setCursor(2,2);
  Game::tft->print("Coin grand total: ");
  Game::tft->println(totalCoins);
  delay(1000);
  #endif
}

void World::updateTiles() {
  this->tileCoin->update();
  this->tileDecorA->update();
  this->tileDecorB->update();
}

bool World::collectCoin() {
  int8_t tileIndexX = this->pixelPositionX / PIXELS_PER_TILE;
  int8_t tileIndexY = this->pixelPositionY / PIXELS_PER_TILE;
  int8_t pixelRemainderX = this->pixelPositionX - tileIndexX * PIXELS_PER_TILE;
  int8_t pixelRemainderY = this->pixelPositionY - tileIndexY * PIXELS_PER_TILE;

  //#define DEBUG_COLLECT_COIN

  for(int8_t i = tileIndexX - 1; i <= tileIndexX + 1; ++i) {
    for (int8_t j = tileIndexY - 1; j <= tileIndexY + 1; ++j) {
      #ifdef DEBUG_COLLECT_COIN
      if (i != 6 || j != 6) // let's say the coin is at (6,6)
        continue;
      #else
      if (this->getTile(i, j) != this->tileCoin)
        continue;
      #endif

      bool x = (tileIndexX == i && pixelRemainderX <= 16) || (tileIndexX + 1 == i && pixelRemainderX >= 16);
      bool y = (tileIndexY == j && pixelRemainderY <= 16) || (tileIndexY + 1 == j && pixelRemainderY >= 16);

      #ifdef DEBUG_COLLECT_COIN
      Game::tft->fillRect(0, 0, 32, 8, 0x0000);
      Game::tft->setTextColor(0xFFFF);
      Game::tft->setCursor(0,0);
      #endif
      if (x && y) {
        #ifdef DEBUG_COLLECT_COIN
        Game::tft->println("YES");
        #else
        this->setTile(i, j, nullptr);
        return true;
        #endif
      }
      #ifdef DEBUG_COLLECT_COIN
      else {
        Game::tft->println("NO");
      }
      delay(150);
      #endif
    }
  }
  
  return false;
}

void World::renderVisibleTiles() {
  int8_t tileIndexX = this->pixelPositionX / PIXELS_PER_TILE;
  int8_t tileIndexY = this->pixelPositionY / PIXELS_PER_TILE;
  int8_t pixelRemainderX = this->pixelPositionX - tileIndexX * PIXELS_PER_TILE;
  int8_t pixelRemainderY = this->pixelPositionY - tileIndexY * PIXELS_PER_TILE;
  const int8_t DRAW_DISTANCE = 2;
  Game::tft->setCursor(0, 0);
  
  //#define DEBUG_WORLD_SIZE
  #ifdef DEBUG_WORLD_SIZE
  Game::tft->print("World tiles X: ");
  Game::tft->println(TILES_X);
  Game::tft->print("World tiles Y: ");
  Game::tft->println(TILES_Y);
  #endif
  
  //#define DEBUG_TILE_RENDER
  #ifdef DEBUG_TILE_RENDER
  Game::tft->print("Pixel pos. X: ");
  Game::tft->println(this->pixelPositionX);
  Game::tft->print("Pixel pos. Y: ");
  Game::tft->println(this->pixelPositionY);
  Game::tft->print("Tile index X: ");
  Game::tft->println(tileIndexX);
  Game::tft->print("Tile index Y: ");
  Game::tft->println(tileIndexY);
  #endif

  //#define DEBUG_PIXEL_REMAINDER
  #ifdef DEBUG_PIXEL_REMAINDER
  Game::tft->print("Pixel rem. X: ");
  Game::tft->println(pixelRemainderX);
  Game::tft->print("Pixel rem. Y: ");
  Game::tft->println(pixelRemainderY);
  #endif
  
  for (int i = -DRAW_DISTANCE; i <= DRAW_DISTANCE; ++i) {
    for (int j = -DRAW_DISTANCE; j <= DRAW_DISTANCE; ++j) {
      int x = i + tileIndexX;
      int y = j + tileIndexY;

      if (x < 0 || x >= TILES_X || y < 0 || y >= TILES_Y) {
        continue;
      }

      //#define DEBUG_SHOW_COORDINATES
      #ifdef DEBUG_SHOW_COORDINATES
      Game::tft->setTextSize(1);
      Game::tft->setTextWrap(false);
      Game::tft->setTextColor(MAGENTA);
      const int8_t FONT_WIDTH = 5;
      const int8_t FONT_HEIGHT = 7;
      int16_t _screenX = this->calculateScreenPosition(1, FONT_WIDTH*(x<10?1:2)+FONT_WIDTH*(x<10?1:2)+FONT_WIDTH, pixelRemainderX, i);
      int16_t _screenY = this->calculateScreenPosition(1, FONT_HEIGHT, pixelRemainderY, j);
      Game::tft->setCursor(_screenX, _screenY);
      Game::tft->print(x);
      Game::tft->print(',');
      Game::tft->print(y);
      Game::tft->setTextColor(WHITE);
      Game::tft->setTextWrap(true);
      continue;
      #endif
      
      if (getTile(x, y) == nullptr) {
        continue;
      }
      
      //#define DEBUG_TILE_1
      #ifdef DEBUG_TILE_1
      // remember that some tiles are skipped because they are null!
      Game::tft->print("[");
      Game::tft->print(x);
      Game::tft->print("][");
      Game::tft->print(y);
      Game::tft->print("] scale: ");
      Game::tft->println(getTile(x, y)->scale);
      #endif
      
      //#define DEBUG_TILE_1_5
      #ifdef DEBUG_TILE_1_5
      Game::tft->print("width: ");
      Game::tft->print(getTile(x, y)->width);
      Game::tft->print("; height: ");
      Game::tft->println(getTile(x, y)->height);
      #endif

      //#define DEBUG_TILE_SCROLL_CALC_X;
      #ifdef DEBUG_TILE_SCROLL_CALC_X
      Game::tft->print("PixRem:");
      Game::tft->print(pixelRemainderX);
      Game::tft->print("|i:");
      Game::tft->println(i);
      #endif

      //#define DEBUG_TILE_SCROLL_CALC_Y;
      #ifdef DEBUG_TILE_SCROLL_CALC_Y
      Game::tft->print("PixRem:");
      Game::tft->print(pixelRemainderY);
      Game::tft->print("|j:");
      Game::tft->println(j);
      #endif
      
      int16_t screenX = this->calculateScreenPosition(getTile(x, y)->scale, getTile(x, y)->width, pixelRemainderX, i);
      int16_t screenY = this->calculateScreenPosition(getTile(x, y)->scale, getTile(x, y)->height, pixelRemainderY, j);
      
      //#define DEBUG_TILE_2

      if (screenX >= 128 || screenY >= 128 || screenX <= (-(int)PIXELS_PER_TILE) || screenY <= (-(int)PIXELS_PER_TILE)) {
        #ifdef DEBUG_TILE_2
        Game::tft->print("Will skip ");
        Game::tft->println(getTile(x, y)->shortName);
        Game::tft->print("At ");
        Game::tft->print(screenX);
        Game::tft->print("x");
        Game::tft->println(screenY);
        delay(250);
        #endif
        continue;
      }
      
      #ifdef DEBUG_TILE_2
      Game::tft->print("Will draw ");
      Game::tft->println(getTile(x, y)->shortName);
      Game::tft->print("At ");
      Game::tft->print(screenX);
      Game::tft->print("x");
      Game::tft->println(screenY);
      delay(250);
      #endif

      getTile(x, y)->render(screenX, screenY, x+y);

      #ifdef DEBUG_TILE_2
      delay(500);
      #endif
    }
  }
}

int16_t World::calculateScreenPosition(int8_t scale, uint8_t dimensionLength, int8_t pixelRemainder, int8_t index) {
  return 64 - scale * dimensionLength / 2 - pixelRemainder + index * PIXELS_PER_TILE;
}
