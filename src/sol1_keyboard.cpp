#include "sol1_keyboard.h"
#include <assert.h>

static void sol1_keyboard_ensure_in_bounds(int key){
    assert(key >= 0 && key < SOL1_TOTAL_KEYS);
}

void sol1_keyboard_set_map(struct sol1_keyboard* keyboard, const char* map)
{
    keyboard->keyboard_map = map;
    int i;
    for(i = 0; i < SOL1_TOTAL_KEYS; i++)
        keyboard->keyboard[i] = false;
}

int sol1_keyboard_map(struct sol1_keyboard* keyboard, char key)
{
    
    int i;
    for(i = 0; i < SOL1_TOTAL_KEYS; i++)
    {
        if(keyboard->keyboard_map[i] == key)
        {
            return i;
        }
    }

    return -1;
}

void sol1_keyboard_down(struct sol1_keyboard* keyboard, int key)
{
    keyboard->keyboard[key] = true;
}

void sol1_keyboard_up(struct sol1_keyboard* keyboard, int key)
{
    keyboard->keyboard[key] = false; 
}

bool sol1_keyboard_is_down(struct sol1_keyboard* keyboard, int key)
{
    return keyboard->keyboard[key];
}