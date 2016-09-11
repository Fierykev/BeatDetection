#include "Keyboard.h"

bool keys[256] = { false };

void keyDown(int key)
{
	keys[key] = 1;
}

void keyUp(int key)
{
	keys[key] = 0;
}