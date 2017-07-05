#include <iostream>
#include <fstream>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "cpu.h"
#include "memory.h"

Cpu::Cpu(std::shared_ptr<Memory> ram,
        std::shared_ptr<Input> in,
        std::shared_ptr<Screen> scr) :
    ram_{ram}, scr_{scr}, keys_{in}
{
    srand(time(NULL));
    p_instr_ = 0;
    p_stack_ = 0;
    reg_I = 0;
    for (byte &v : regs_)
        v = 0;

    opcodes_ = {
        { 0x00E0, 0xFFFF, [&] (int, int, int)
            {
                scr_->Clear();
                p_instr_ += 2;
            } }, // CLS
        { 0x00EE, 0xFFFF, [&] (int, int, int) // RET
            {
                p_instr_ = stack_[p_stack_--]+2;
            } },
        { 0x0000, 0xF000, [&] (int instr, int, int) // CALL
            {
                p_instr_ = (instr & 0x0FFF);
            } },
        { 0x1000, 0xF000, [&] (int instr, int, int) // JP addr
            {
                p_instr_ = (instr & 0x0FFF);
            } },
        { 0x2000, 0xF000, [&] (int instr, int, int)
            {
                stack_[++p_stack_] = p_instr_;
                p_instr_ = 0x0FFF & instr;
            } },// CALL addra
        { 0x3000, 0xF000, [&] (int instr, int reg1, int)
            {  // SE Vx, byte
                if (regs_[reg1] == (instr & 0x00FF))
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0x4000, 0xF000, [&] (int instr, int reg1, int)
            { // SNE Vx, byte
                if (regs_[reg1] != (instr & 0x00FF))
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0x5000, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SE Vx, Vy
                if (regs_[reg1] == regs_[reg2])
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0x6000, 0xF000, [&] (int instr, int reg1, int)
            { // LD Vx, byte
                regs_[reg1] = instr & 0x00FF;
                p_instr_ += 2;
            } },
        { 0x7000, 0xF000, [&] (int instr, int reg1, int)
            { // ADD Vx, byte
                regs_[reg1] += instr & 0x00FF;
                p_instr_ += 2;
            } },
        { 0x8000, 0xF00F, [&] (int instr, int reg1, int reg2)
            {
                regs_[reg1] = regs_[reg2];
                p_instr_ += 2;
            } },
        { 0x8001, 0xF00F, [&] (int, int reg1, int reg2)
            { // OR Vx, Vy
                regs_[reg1] |= regs_[reg2];
                p_instr_ += 2;
            } },
        { 0x8002, 0xF00F, [&] (int, int reg1, int reg2)
            {// AND Vx, Vy
                regs_[reg1] &= regs_[reg2];
                p_instr_ += 2;
            } },
        { 0x8003, 0xF00F, [&] (int, int reg1, int reg2)
            { // XOR Vx, Vy
                regs_[reg1] ^= regs_[reg2];
                p_instr_ += 2;
            } },
        { 0x8004, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // ADD Vx, Vy
                unsigned int res = regs_[reg1] + regs_[reg2];
                regs_[reg1] = res & 0xFF;
                regs_[15] = res > 255;
                p_instr_ += 2;
            } },
        { 0x8005, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SUB Vx, Vy
                regs_[15] = regs_[reg1] > regs_[reg2];
                regs_[reg1] -= regs_[reg2];
                p_instr_ += 2;
            } },
        { 0x8006, 0xF00F, [&] (int instr, int reg1, int)
            { // SHR Vx {, Vy}
                regs_[15] = regs_[reg1] % 2;
                regs_[reg1] >>=1;
                p_instr_ += 2;
            } },
        { 0x8007, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SUBN Vx, Vy
                regs_[0xF] = regs_[reg2] > regs_[reg1];
                regs_[reg1] = regs_[reg2] - regs_[reg1];
                p_instr_ += 2;
            } },
        { 0x800E, 0xF00F, [&] (int, int reg1, int)
            { // SHL Vx {, Vy}
                regs_[0xF] = regs_[reg1] >> 7;
                regs_[reg1] <<= 1;
                p_instr_ += 2;
            } },
        { 0x9000, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // 9xy0 - SNE Vx, Vy
                if (regs_[reg1] != regs_[reg2])
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0xA000, 0xF000, [&] (int instr, int, int)
            { // LD I, addr
                reg_I = 0x0FFF & instr;
                p_instr_ += 2;
            } },
        { 0xB000, 0xF000, [&] (int instr, int, int)
            { // JP V0, addr
                p_instr_ = (instr & 0x0FFF) + regs_[0];
            } },
        { 0xC000, 0xF000, [&] (int instr, int reg1, int)
            { // RND Vx, byte
                regs_[reg1] = (rand() % 256) & (0x00FF & instr);
                p_instr_ += 2;
            } },
        { 0xD000, 0xF000, [&] (int instr, int reg1, int reg2)
            { // DRW Vx, Vy, nibble
                char n = instr & 0x000F;
                reg_F = 0;

                for (int i = 0 ; i < n ; ++i)
                {
                    reg_F |= scr_->XorSprite((*ram_)[reg_I+i], regs_[reg1],
                            regs_[reg2]+i);
                }
                p_instr_ += 2;
            } },
        { 0xE09E, 0xF0FF, [&] (int instr, int reg1, int)
            {
                if ((*keys_)[regs_[reg1]])
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0xE0A1, 0xF0FF, [&] (int instr, int reg1, int)
            {
                if (!(*keys_)[regs_[reg1]])
                    p_instr_ += 2;
                p_instr_ += 2;
            } },
        { 0xF007, 0xF0FF, [&] (int instr, int reg1, int)
            {
                regs_[reg1] = delay_timer_;
                p_instr_ += 2;
            } },
        { 0xF00A, 0xF0FF, [&] (int instr, int reg1, int)
            {
                regs_[reg1] = keys_->WaitKey();
                p_instr_ += 2;
            } },
        { 0xF015, 0xF0FF, [&] (int instr, int reg1, int)
            {
                delay_timer_ = regs_[reg1];
                p_instr_ += 2;
            } },
        { 0xF018, 0xF0FF, [&] (int instr, int reg1, int)
            {
                sound_timer_ = regs_[reg1];
                p_instr_ += 2;
            } },
        { 0xF01E, 0xF0FF, [&] (int instr, int reg1, int)
            {
                reg_I += regs_[reg1];
                p_instr_ += 2;
            } },
        { 0xF029, 0xF0FF, [&] (int instr, int reg1, int)
            {
                reg_I = regs_[reg1] * 5;
                p_instr_ += 2;
            } },
        { 0xF033, 0xF0FF, [&] (int instr, int reg1, int)
            {
                int nbr = regs_[reg1];
                (*ram_)[reg_I + 2] = nbr % 10;
                nbr /= 10;
                (*ram_)[reg_I + 1] = nbr % 10;
                nbr /= 10;
                (*ram_)[reg_I] = nbr;
                p_instr_ += 2;
            } },
        { 0xF055, 0xF0FF, [&] (int instr, int reg1, int)
            {
                for (int i = 0 ; i <= reg1 ; ++i)
                    (*ram_)[reg_I + i] = regs_[i];
                p_instr_ += 2;
            } },
        { 0xF065, 0xF0FF, [&] (int instr, int reg1, int)
            {
                for (int i = 0 ; i <= reg1 ; ++i)
                    regs_[i] = (*ram_)[reg_I + i];
                p_instr_ += 2;
            }
        }
    };

    for (byte &b : regs_)
        b = 0;
}

void Cpu::Execute(int instr)
{
    int reg1 = (instr & 0x0F00) >> 8 ;
    int reg2 = (instr & 0x00F0) >> 4;


    for (opcode& o : opcodes_)
        if ((o.mask & instr) == o.value)
        {
            o.func(instr, reg1, reg2);
            break;
        }
    scr_->Display();
    keys_->Update();
}

void Cpu::LaunchProgram()
{
    p_instr_ = 0x200;
    while (true)
    {
        int instr = (*ram_)[p_instr_];
        instr <<= 8;
        instr += (*ram_)[p_instr_+1];
        std::cout << "instr : " << std::hex << p_instr_ << " = " << instr << "        " << "   V0 : " << (int)regs_[0] << std::endl;
        Execute(instr);
        if (delay_timer_ > 0)
            --delay_timer_;
        else
            delay_timer_ = 0;
        --sound_timer_;
    }
}

void Cpu::PrintDebug(void)
{
    std::cout << "=========== CPU =============" << std::endl;
    std::cout << "REGISTERS" << std::endl;
    int i = 0;
    for (byte b : regs_)
        std::cout << std::hex << i++ << " :  0x" << (int)b <<  std::endl;
    std::cout << "I :  0x" << std::hex << reg_I << std::endl;
}

int main(int argc, char** argv)
{
#if 0
    std::vector<byte> v { 0x62, 0x0A, 0xF2, 0x29, 0x60, 0x0a, 0xD0, 0x05 };
#else
    std::vector<byte> v;
    std::ifstream file(argv[1], std::ios::in | std::ios::binary);
    while (!file.eof())
    {
        v.push_back(file.get());
    }
#endif
    auto s = std::make_shared<Screen>();
    auto m = std::make_shared<Memory>(v);
    auto k = std::make_shared<Input>();
    Cpu c(m, k, s);
    c.LaunchProgram();
    int a;
    std::cin >> a;
    return 0;
}

