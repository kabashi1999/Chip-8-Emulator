#ifndef CHIP8
#define CHIP8
#include <cstdint>
#include <chrono>
#include <random>

class chip8
{
private:
    /* data */
public:
    chip8();
    ~chip8();
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index_register;
    uint16_t program_counter;
    uint16_t stack[16]{};
    uint8_t stack_pointer;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t key_pad[16];
    uint32_t video[64 * 32]{};
    uint16_t opcode;
    const unsigned int START_ADDRESS = 0x200;
    const unsigned int FONTSET_START_ADDRESS = 0x50;
    const static unsigned int FONTSET_SIZE = 80;
    const static unsigned int VIDEO_WIDTH = 64;
    const static unsigned int VIDEO_HEIGHT = 32;
    typedef void (chip8::*chip8Func)();
    chip8Func table[0xF + 1];
    chip8Func table0[0xE + 1];
    chip8Func table8[0xE + 1];
    chip8Func tableE[0xE + 1];
    chip8Func tableF[0x65 + 1];
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    uint8_t fontset[FONTSET_SIZE] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    void LoadROM(char const *file_name);
    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();
    void OP_2NNN();
    void OP_3XKK();
    void OP_4XKK();
    void OP_5XY0();
    void OP_6XKK();
    void OP_7XKK();
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY6();
    void OP_8XY7();
    void OP_8XYE();
    void OP_9XY0();
    void OP_ANNN();
    void OP_BNNN();
    void OP_CXKK();
    void OP_DXYN();
    void OP_EX9E();
    void OP_EXA1();
    void OP_FX07();
    void OP_FX0A();
    void OP_FX15();
    void OP_FX18();
    void OP_FX1E();
    void OP_FX29();
    void OP_FX33();
    void OP_FX55();
    void OP_FX65();
    void OP_NULL();
    // table Funtions
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void Cycle();
};

#endif
