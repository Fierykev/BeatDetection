#ifndef KEYBOARD_H
#define KEYBOARD_H

const char LANE_KEY[] = { 'a', 's', 'd', 'f' };

extern bool keys[256]; // check if a lane is pressed

void keyDown(int key);

void keyUp(int key);

#endif