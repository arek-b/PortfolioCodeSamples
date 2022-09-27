#pragma once

#include <stdint.h>
#include "Tile.h"
#include "ImageData.h"

class World {
  public:
    static const int8_t TILES_X = 28;
    static const int8_t TILES_Y = 28;
    static const int8_t PIXELS_PER_TILE = 32;
    World(uint8_t RAND1, uint8_t RAND2, int8_t minCoinAmount);
    void renderVisibleTiles();
    void updateTiles();
    void scrollX(int8_t x);
    void scrollY(int8_t y);
    bool collectCoin();
  private:
    int16_t pixelPositionX = (TILES_X / 2) * PIXELS_PER_TILE; // default to dead center
    int16_t pixelPositionY = (TILES_Y / 2) * PIXELS_PER_TILE; // default to dead center
    int8_t tiles[TILES_X][TILES_Y];
    Tile* tileCoin = new Tile(ImageData::Coin, ImageData::CoinColors, ImageData::CoinBitmapCount, ImageData::CoinFrameCount, 3, ImageData::CoinSizeX, ImageData::CoinSizeY, true, "Coin");
    Tile* tileDecorA = new Tile(ImageData::epd_bitmap_decorAArray, ImageData::epd_bitmap_decorA_colors, 1, 1, 3, 8, 8, false, "GrassA");
    Tile* tileDecorB = new Tile(ImageData::epd_bitmap_decorBArray, ImageData::epd_bitmap_decorB_colors, 1, 1, 3, 8, 8, false, "GrassB");
    int16_t calculateScreenPosition(int8_t scale, uint8_t dimensionLength, int8_t pixelRemainder, int8_t index);
    void generate(uint8_t RAND1, uint8_t RAND2, int8_t minCoinAmount);
    Tile* getTile(int8_t x, int8_t y);
    void setTile(int8_t x, int8_t y, Tile* tile);
};
