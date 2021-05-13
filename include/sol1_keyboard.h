#ifndef SOL1KEYBOARD_H
#define SOL1KEYBOARD_H

#include <stdbool.h>
#include "config.h"
struct sol1_keyboard
{
    bool keyboard[SOL1_TOTAL_KEYS];
    const char* keyboard_map;
};

void sol1_keyboard_set_map(struct sol1_keyboard* keyboard, const char* map);
int sol1_keyboard_map(struct sol1_keyboard* keyboard, char key);
void sol1_keyboard_down(struct sol1_keyboard* keyboard, int key);
void sol1_keyboard_up(struct sol1_keyboard* keyboard, int key);
bool sol1_keyboard_is_down(struct sol1_keyboard* keyboard, int key);

#endif