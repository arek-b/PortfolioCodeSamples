#pragma once
#include "Player.h"
#include "Constants.h"
#include "Helpers.h"
#include "World.h"
#include "ImageData.h"

Player::Player(uint16_t bitmapColors[], int16_t posX, int16_t posY, int16_t resX, int16_t resY, uint8_t scale) {
  this->bitmapColors = bitmapColors;
  this->posX = posX;
  this->posY = posY;
  this->resX = resX;
  this->resY = resY;
  this->scale = scale;
  this->posOffsetX = -(scale * resX / 2);
  this->posOffsetY = -(scale * resY / 2);
}

void Player::update() {
  this->currentFrame = (this->currentFrame + 1) % (this->bitmapCount / 2);

  if (this->state == Idle || this->state == Sleeping) {
    this->idleFrames++;
  }
  else {
    this->idleFrames = 0;
  }
}

void Player::render() {
  int8_t frameOffset = ImageData::SizeofFrogColors * this->currentFrame;
  for (int8_t i = 0; i < ImageData::SizeofFrogColors; ++i) {
    drawBitmapScaled(this->posX + this->posOffsetX, this->posY + this->posOffsetY, bitmaps[frameOffset + i], this->resX, this->resY, this->bitmapColors[i], this->scale);
  }

  if (this->state == Eating && (this->currentFrame == (this->bitmapCount / 2) - 1)) {
    this->setState(FinishedEating);
  }
}

void Player::setState(PlayerState playerState) {

  if (this->state == playerState)
    return;

  bool validState = true;

  switch(playerState) {

    case Idle:
      bitmaps = ImageData::FrogIdle;
      this->bitmapCount = ImageData::SizeofFrogIdle;
      break;

    case Sleeping:
      bitmaps = ImageData::FrogSleep;
      this->bitmapCount = ImageData::SizeofFrogSleep;
      break;

    case Eating:
      bitmaps = ImageData::FrogEat;
      this->bitmapCount = ImageData::SizeofFrogEat;
      break;

    case FinishedEating: // transitional state, won't be displayed
      bitmaps = ImageData::FrogIdle;
      this->bitmapCount = ImageData::SizeofFrogIdle;
      break;

    case MovingUp:
      bitmaps = ImageData::FrogUp;
      this->bitmapCount = ImageData::SizeofFrogUp;
      break;

    case MovingRight:
      bitmaps = ImageData::FrogWalkRight;
      this->bitmapCount = ImageData::SizeofFrogWalkRight;
      break;

    case MovingDown:
      bitmaps = ImageData::FrogDown;
      this->bitmapCount = ImageData::SizeofFrogDown;
      break;

    case MovingLeft:
      bitmaps = ImageData::FrogWalkLeft;
      this->bitmapCount = ImageData::SizeofFrogWalkLeft;
      break;

    default:
      validState = false;
      break;
  }

  if (!validState)
    return;

  this->state = playerState;
  this->currentFrame = (this->bitmapCount / 2) - 1; // last frame, update() will increment it to 1st frame
}