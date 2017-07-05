#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <cstdint>

typedef uint8_t byte;

/**
 * @brief This class represents the keyboard.
 */
class Input : public std::array<byte, 0x10>
{
    public:
        /**
         * @brief Input constructor
         */
        Input(void);
        /**
         * @brief Gets the pressed keys, and actualises the array
         */
        void Update(void);
        /**
         * @brief Blocking call for waiting a key
         *
         * @return the value of the pressed key
         */
        byte WaitKey(void);

    private:
        std::unique_ptr<sf::Keyboard> keys_;
};
