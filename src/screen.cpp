#include "screen.h"
#include <SFML/Graphics.hpp>

#include <iostream>

using namespace sf;

Screen::Screen(void) :
    win_{VideoMode(64*4, 32*4, 32), "Chip - 8 emulator"}
{
    win_.SetFramerateLimit(60);
    for (auto line : screen_)
        for (bool& pixl : line)
            pixl = false;
}

void Screen::Clear(void)
{
    for (auto line : screen_)
        for (bool& pixl : line)
            pixl = false;
}

int Screen::XorSprite(byte sprite, int x, int y)
{
    int erased = false;
    x += 7;

    for (int i = 0; i < 8 ; ++i)
    {
        screen_[y][x-i] ^= sprite % 2;
        sprite >>= 1;
        erased |= !screen_[y][x-i];
    }
    return erased;
}

void Screen::Display(void)
{
    win_.Clear(Color(0, 0, 0, 255));
    for (int i = 0 ; i < screen_.size() ; ++i)
        for (int j = 0 ; j < screen_[0].size() ; ++j)
        {
            if (screen_[i][j])
            {
                FloatRect src(j * 4, i * 4, 4, 4);
                auto r = Shape::Rectangle(src, Color::White);
                win_.Draw(r);
            }
        }
    win_.Display();
}

