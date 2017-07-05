#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <fstream>

using std::ostringstream;
using std::string;

struct opcode
{
    int value;
    int mask;
    std::function<string(int, int, int)> func;
};

int main(int argc, char** argv)
{
    std::vector<opcode> opcodes_ {
        { 0x00E0, 0xFFFF, [&] (int i, int, int)
            {
                return string("CLS");
            } }, // CLS
        { 0x00EE, 0xFFFF, [&] (int i, int, int) // RET
            {
                return string("RET");
            } },
        { 0x0000, 0xF000, [&] (int instr, int, int) // CALL
            {
                ostringstream oss;
                oss << "SYS " << (instr & 0x0FFF);
                return oss.str();
            } },
        { 0x1000, 0xF000, [&] (int instr, int, int) // JP addr
            {
                ostringstream oss;
                oss << "JP 0x" << std::hex << (instr & 0x0FFF);
                return oss.str();
            } },
        { 0x2000, 0xF000, [&] (int instr, int, int)
            {
                ostringstream oss;
                oss << "CALL 0x" << std::hex << (instr & 0x0FFF);
                return oss.str();
            } },// CALL addra
        { 0x3000, 0xF000, [&] (int instr, int reg1, int)
            {  // SE Vx, byte
                ostringstream oss;
                oss << "SE V" << reg1 << ", " << (instr & 0x00FF);
                return oss.str();
            } },
        { 0x4000, 0xF000, [&] (int instr, int reg1, int)
            { // SNE Vx, byte
                ostringstream oss;
                oss << "SNE V" << reg1 << ", " << (instr & 0x00FF);
                return oss.str();
            } },
        { 0x5000, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SE Vx, Vy
                ostringstream oss;
                oss << "SE V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x6000, 0xF000, [&] (int instr, int reg1, int)
            { // LD Vx, byte
                ostringstream oss;
                oss << "LD V" << reg1 << ", " << (instr & 0x00FF);
                return oss.str();
            } },
        { 0x7000, 0xF000, [&] (int instr, int reg1, int)
            { // ADD Vx, byte
                ostringstream oss;
                oss << "ADD V" << reg1 << ", " << (instr & 0x00FF);
                return oss.str();
            } },
        { 0x8000, 0xF00F, [&] (int instr, int reg1, int reg2)
            {
                ostringstream oss;
                oss << "LD V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8001, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // OR Vx, Vy
                ostringstream oss;
                oss << "OR V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8002, 0xF00F, [&] (int instr, int reg1, int reg2)
            {// AND Vx, Vy
                ostringstream oss;
                oss << "AND V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8003, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // XOR Vx, Vy
                ostringstream oss;
                oss << "XOR V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8004, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // ADD Vx, Vy
                ostringstream oss;
                oss << "ADD V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8005, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SUB Vx, Vy
                ostringstream oss;
                oss << "SUB V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x8006, 0xF00F, [&] (int instr, int reg1, int)
            { // SHR Vx {, Vy}
                ostringstream oss;
                oss << "SHR V" << reg1;
                return oss.str();
            } },
        { 0x8007, 0xF00F, [&] (int instr, int reg1, int reg2)
            { // SUBN Vx, Vy
                ostringstream oss;
                oss << "SUBN V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0x800E, 0xF00F, [&] (int instr, int reg1, int)
            { // SHL Vx {, Vy}
                ostringstream oss;
                oss << "SHL V" << reg1;
                return oss.str();
            } },
        { 0x9000, 0xF000, [&] (int instr, int reg1, int reg2)
            { // 9xy0 - SNE Vx, Vy
                ostringstream oss;
                oss << "SNE V" << reg1 << ", V" << reg2;
                return oss.str();
            } },
        { 0xA000, 0xF000, [&] (int instr, int, int)
            { // LD I, addr
                ostringstream oss;
                oss << "LD I, 0x" << std::hex << (instr & 0x0FFF);
                return oss.str();
            } },
        { 0xB000, 0xF000, [&] (int instr, int, int)
            { // JP V0, addr
                ostringstream oss;
                oss << "JP V0, " << std::hex << (instr & 0x0FFF);
                return oss.str();
            } },
        { 0xC000, 0xF000, [&] (int instr, int reg1, int)
            { // RND Vx, byte
                ostringstream oss;
                oss << "RND V" << reg1 << ", " << (instr & 0x00FF);
                return oss.str();
            } },
        { 0xD000, 0xF000, [&] (int instr, int reg1, int reg2)
            { // DRW Vx, Vy, nibble
                ostringstream oss;
                oss << "DRW V" << reg1 << ", V" << reg2 << ", "
                    << (instr & 0x000F);
                return oss.str();
            } },
        { 0xE09E, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "SKP V" << reg1;
                return oss.str();
            } },
        { 0xE0A1, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "SKNP V" << reg1;
                return oss.str();
            } },
        { 0xF007, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD V" << reg1 << ", DT";
                return oss.str();
            } },
        { 0xF00A, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD V" << reg1 << ", K";
                return oss.str();
            } },
        { 0xF015, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD DT, V" << reg1;
                return oss.str();
            } },
        { 0xF018, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD ST, V" << reg1;
                return oss.str();
            } },
        { 0xF01E, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "ADD I, V" << reg1;
                return oss.str();
            } },
        { 0xF029, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD F, V" << reg1;
                return oss.str();
            } },
        { 0xF033, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD B, V" << reg1;
                return oss.str();
            } },
        { 0xF055, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD [I], V" << reg1;
                return oss.str();
            } },
        { 0xF065, 0xF0FF, [&] (int instr, int reg1, int)
            {
                ostringstream oss;
                oss << "LD V" << reg1 << ", [I]";
                return oss.str();
            }
        }
    };

    std::ifstream file(argv[1], std::ios::binary);

    std::vector<std::string> p;
    while (!file.eof())
    {
        int instr = file.get();
        instr <<= 8;
        instr += file.get();

        int reg1 = (instr & 0x0F00) >> 8;
        int reg2 = (instr & 0x00F0) >> 4;


        for (opcode& op : opcodes_)
            if ((op.mask & instr) == op.value)
            {
                p.push_back(op.func(instr, reg1, reg2));
                break;
            }
    }
    for (int i = 0 ; i < p.size() ; ++i)
        std::cout << "0x" << std::hex << i + 0x200 << " : "
            << p[i] << std::endl;

    return 0;
}

