#ifndef CHIP8KEYBOARD_H
#define CHIP8KEYBOARD_H
#include "config.h"
#include <stdbool.h>

struct chip8_keyboard{
    bool keyboard[CHIP8_TOTAL_KEYS];


};
int chip8_keyboard_map(const char* map,char key);
void chip8_keyboard_dwon(struct chip8_keyboard* keyborad, char key);
void chip8_keyboard_up(struct chip8_keyboard* keyborad,int key);
bool chip8_keyboard_is_dwon(struct chip8_keyboard* keyborad,int key);
#endif