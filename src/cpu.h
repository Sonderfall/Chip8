#pragma once

#include <array>
#include <memory>

#include "memory.h"
#include "screen.h"
#include "input.h"

typedef std::array<byte, 0x10> Registers;

typedef std::array<word, 2> SRegisters;

/**
 * @brief Operation code
 */
struct opcode
{
    int value;
    int mask;
    std::function<void(int, int, int)> func;
};

/**
 * @brief This class represents the CPU, and defines all methods to simulate
 * it
 */
class Cpu
{
    public:
        /**
         * @brief Initializes the Cpu
         *
         * @param ram The Memory with program loaded inside
         * @param in The input
         * @param scr The screen
         */
        Cpu(std::shared_ptr<Memory> ram,
                std::shared_ptr<Input> in,
                std::shared_ptr<Screen> scr);
        /**
         * @brief Prints some debug infos (registers values etc)
         */
        void PrintDebug(void);
        /**
         * @brief Executes the program.
         */
        void LaunchProgram();

    private:
        ///
        /// @brief Executes an instruction
        ///
        /// @param instr Instruction code
        ///
        void Execute(int instr);
        Registers regs_;
        std::shared_ptr<Input> keys_;
        std::shared_ptr<Memory> ram_;
        std::shared_ptr<Screen> scr_;
        std::vector<opcode> opcodes_;
        std::array<word, 0x10> stack_;
        word p_instr_;
        byte p_stack_;
        word reg_I;
        word reg_F;
        byte delay_timer_;
        byte sound_timer_;
};

