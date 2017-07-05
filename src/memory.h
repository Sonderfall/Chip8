#pragma once

#include <array>
#include <vector>
#include <cstdint>

typedef uint8_t byte;
typedef uint16_t word;

/**
 * @brief This is the RAM
 */
class Memory : public std::array<byte, 0xFFF>
{
    public:
        /**
         * @brief The constructor wich initilizes the memomry and copies the
         * program code
         *
         * @param program
         */
        Memory(std::vector<byte> program);
};

