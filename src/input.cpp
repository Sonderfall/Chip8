#include "input.h"
#include <SFML/Graphics.hpp>
#include <vector>

Input::Input(void)
    : keys_(new sf::Keyboard())
{
}

void Input::Update(void)
{
    struct KeyVal { int code; sf::Keyboard::Key key; };
    std::vector<KeyVal> keys = {
        { 0x1, keys_->Num1 },
        { 0x2, keys_->Num2 },
        { 0x3, keys_->Num3 },
        { 0xC, keys_->Num4 },
        { 0x4, keys_->A },
        { 0x5, keys_->Z },
        { 0x6, keys_->E },
        { 0xD, keys_->R },
        { 0x7, keys_->Q },
        { 0x8, keys_->S },
        { 0x9, keys_->D },
        { 0xE, keys_->F },
        { 0xA, keys_->W },
        { 0x0, keys_->X },
        { 0xB, keys_->C },
        { 0xF, keys_->V },
    };

    for (auto k : keys)
        at(k.code) = keys_->IsKeyPressed(k.key);
}

byte Input::WaitKey(void)
{
    do
    {
        for (int i = 0 ; i < 0x10 ; ++i)
            if (at(i))
                return i;
        Update();
    }
    while (true);
    return 0; // disables the warning
}

