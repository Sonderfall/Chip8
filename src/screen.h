#include <SFML/Graphics.hpp>
#include <array>
#include <cstdint>

typedef uint16_t word;
typedef uint8_t byte;

/**
 * @brief Represents the screen & graphic ship
 */
class Screen
{
    public:
        Screen(void);
        /**
         * @brief The CLS command
         */
        void Clear(void);
        /**
         * @brief Xor the sprite at coordinates X, Y
         *
         * @param sprite the sprite to be xored
         * @param x
         * @param y
         *
         * @return bool saying if a sprite has been erased
         */
        int  XorSprite(byte sprite, int x, int y);
        /**
         * @brief Refresh the screen. Has to be called once a cycle
         */
        void Display(void);

    private:
        sf::RenderWindow win_;
        std::array<std::array<bool, 64>, 32> screen_;
};

