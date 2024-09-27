#include <MD_MAX72xx.h>
#include <TM1637.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10
#define CLK_SEGMENT 6
#define DIO_SEGMENT 7
#define LeftButton 2
#define RightButton 3

int x = 3;
int y = 0;

int i = 0;
int check = 0;

int score = 0;
int playsNumber = 0;

int mode = 0;

MD_MAX72XX M = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
TM1637  tm1637(CLK_SEGMENT, DIO_SEGMENT);

byte game_map_hard[32] = {B10000001, B10000001, B10000001, B10000001, B11100001, B10000111, B10011001, B10000111,
                     B10000001, B11100111, B11100001, B11111001, B10011001, B10000001, B10000001, B10011111,
                     B10000001, B10000001, B10000111, B11100001, B10011001, B11100001, B10000001, B10000001,
                     B11100111, B10000001, B10011111, B10000001, B10000001, B11111001, B10000001, B10000111};

byte menu[8] = {B00000000, B11100101, B00100101, B11100111, B00100101, B11100101, B00000000, B00000000};

byte game_map[32];

void setup() {
  pinMode(LeftButton, INPUT_PULLUP);
  pinMode(RightButton, INPUT_PULLUP);
  tm1637.init();
  tm1637.setBrightness(0x0a);
  M.begin();
  M.control(MD_MAX72XX::INTENSITY, 3);
  M.clear();
  M.setPoint(y, x, 1);
  attachInterrupt(digitalPinToInterrupt(LeftButton), moveLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(RightButton), moveRight, FALLING);
  randomSeed(analogRead(0));
}

void loop() {
  drawMap();
}

byte generateRandom() {
  int randomNumber = random(400);
  if (randomNumber < 100) {
    return B11100001;
  } else if (randomNumber >= 100 && randomNumber < 200) {
    return B10011001;
  } else if (randomNumber >= 200 && randomNumber < 300){
    return B10000111;
  } else {
    return B10000001;
  }
}

void drawMap() {
  for (i = 0; i < 32; i++) {
    if (i == 0 && score == 0) {
      tm1637.clearScreen();
      tm1637.display(score, false);
      for (int s = 0; s < 8; s++) {
        M.setRow(s, menu[s]);
      }
      while (digitalRead(LeftButton) == HIGH && digitalRead(RightButton) == HIGH) {}
      if (mode == 0) {
        game_map[0] = B10000001;
        game_map[1] = B10000001;
        game_map[2] = B10000001;
        game_map[3] = B10000001;
        for (int s = 4; s < 32; s++) {
          byte randomByte = generateRandom();
          game_map[s] = randomByte;
        }
        for (int s = 4; s < 30; s++) {
          if (game_map[s] == B11100001) {
            if (game_map[s + 1] == B10011001) {
              if (game_map[s + 2] == B10000111) {
                game_map[s + 2] = B10000001;
              }
            }
          }
          if (game_map[s] == B10000111) {
            if (game_map[s + 1] == B10011001) {
              if (game_map[s + 2] == B11100001) {
                game_map[s + 2] = B10000001;
              }
            }
          }
        }
      } else {
        for (int s = 0; s < 32; s++) {
          game_map[s] = game_map_hard[s];
        }
      }
    }
    for (int j = 0; j < 8; j++) {
      int k = i + j;
      if (i + j > 31) {
        k = k - 32;
      }
      M.setRow(j, game_map[k]);
      if (j == y) {
        int bitValue = bitRead(game_map[k], x);
        if (bitValue) {
          check = 1;
          collision();
        } else {
          M.setPoint(y, x, 1);
        }
      }
    }
    delay(500);
    if (i != -1) {
      score = score + 1;
      tm1637.clearScreen();
      tm1637.display(score, false);
    } else {
      if (check == 0) {
        tm1637.clearScreen();
        tm1637.display(playsNumber, false);
        delay(1500);
      }
      check = 0;
    }
  }
}

void moveLeft() {
  if (score == 0) {
    mode = 1;
  } else {
    if (x != 1) {
      M.setPoint(y, x, 0);
      bool checkLeft = M.getPoint(y, x - 1);
      if (checkLeft) {
        collision();
      } else {
        x = x - 1;
        M.setPoint(y, x, 1);
      }
    }
  }
}

void moveRight() {
  if (score == 0) {
    mode = 0;
  } else {
    if (x != 6) {
      M.setPoint(y, x, 0);
      bool checkRight = M.getPoint(y, x + 1);
      if (checkRight) {
        collision();
      } else {
        x = x + 1;
        M.setPoint(y, x, 1);
      }
    }
  }
}

void collision() {
  int index = 0;
  for (; index < 8; index++) {
    M.setRow(index, B11111111);
  }
  x = 3;
  y = 0;
  i = -1;
  playsNumber = playsNumber + 1;
  score = 0;
  if (check == 1) {
    tm1637.clearScreen();
    tm1637.display(playsNumber, false);
    delay(1500);
  }
}