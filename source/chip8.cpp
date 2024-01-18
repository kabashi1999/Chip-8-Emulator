#include <fstream>
#include "chip8.h"
#include <cstring>

chip8::chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{

    // initilize pc
    program_counter = START_ADDRESS;

    // load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // random generator
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // set up function poniter tables
    table[0x0] = &chip8::Table0;
    table[0x1] = &chip8::OP_1NNN;
    table[0x2] = &chip8::OP_2NNN;
    table[0x3] = &chip8::OP_3XKK;
    table[0x4] = &chip8::OP_4XKK;
    table[0x5] = &chip8::OP_5XY0;
    table[0x6] = &chip8::OP_6XKK;
    table[0x7] = &chip8::OP_7XKK;
    table[0x8] = &chip8::Table8;
    table[0x9] = &chip8::OP_9XY0;
    table[0xA] = &chip8::OP_ANNN;
    table[0xB] = &chip8::OP_BNNN;
    table[0xC] = &chip8::OP_CXKK;
    table[0xD] = &chip8::OP_DXYN;
    table[0xE] = &chip8::TableE;
    table[0xF] = &chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &chip8::OP_NULL;
        table8[i] = &chip8::OP_NULL;
        tableE[i] = &chip8::OP_NULL;
    }

    table0[0x0] = &chip8::OP_00E0;
    table0[0xE] = &chip8::OP_00EE;

    table8[0x0] = &chip8::OP_8XY0;
    table8[0x1] = &chip8::OP_8XY1;
    table8[0x2] = &chip8::OP_8XY2;
    table8[0x3] = &chip8::OP_8XY3;
    table8[0x4] = &chip8::OP_8XY4;
    table8[0x5] = &chip8::OP_8XY5;
    table8[0x6] = &chip8::OP_8XY6;
    table8[0x7] = &chip8::OP_8XY7;
    table8[0xE] = &chip8::OP_8XYE;

    tableE[0x1] = &chip8::OP_EXA1;
    tableE[0xE] = &chip8::OP_EX9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &chip8::OP_NULL;
    }

    tableF[0x07] = &chip8::OP_FX07;
    tableF[0x0A] = &chip8::OP_FX0A;
    tableF[0x15] = &chip8::OP_FX15;
    tableF[0x18] = &chip8::OP_FX18;
    tableF[0x1E] = &chip8::OP_FX1E;
    tableF[0x29] = &chip8::OP_FX29;
    tableF[0x33] = &chip8::OP_FX33;
    tableF[0x55] = &chip8::OP_FX55;
    tableF[0x65] = &chip8::OP_FX65;
}
chip8::~chip8()
{
}

void chip8::LoadROM(char const *file_name)
{

    std::ifstream file(file_name, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        // we get the sie of the file and create a buffer to hold the data of the file
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        // we read the file into the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // load rom into the memory starting from address 0x200

        for (long i = 0; i < size; ++i)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete (buffer);
    }
}

void chip8::Cycle()
{
    // Fetching
    opcode = (memory[program_counter] << 8u) | (memory[program_counter + 1]);

    program_counter += 2;
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if (delay_timer > 0)
    {
        --delay_timer;
    }
    if (sound_timer)
    {
        --sound_timer;
    }
}

void chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

// CLS : clear the display
void chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

// RET : return from subroutine
void chip8::OP_00EE()
{
    --stack_pointer;
    program_counter = stack[stack_pointer];
}

// JP : addr jump to location nnn
void chip8::OP_1NNN()
{
    uint16_t address = opcode & 0x0FFFu;

    program_counter = address;
}

// CALL addr : call subroutine at nnn
void chip8::OP_2NNN()
{
    uint16_t address = opcode & 0x0FFFu;

    stack[stack_pointer] = program_counter;
    ++stack_pointer;
    program_counter = address;
}

// SE Vx, kk : skip next instruction if Vx == kk
void chip8::OP_3XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    if (registers[Vx] == kk)
    {
        program_counter += 2;
    }
}

// SNE Vx, kk : skip next instruction if Vx != kk
void chip8::OP_4XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    if (registers[Vx] != kk)
    {
        program_counter += 2;
    }
}

// SE Vx, Vy : skip next instruction if Vx = Vy
void chip8::OP_5XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Yx = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Yx])
    {
        program_counter += 2;
    }
}

// LD Vx, kk : set Vx = kk
void chip8::OP_6XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    registers[Vx] = kk;
}

// ADD Vx, kk : set Vx = Vx + kk
void chip8::OP_7XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    registers[Vx] += kk;
}

// LD Vx, Vy : set Vx = Vy
void chip8::OP_8XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

// OR Vx, Vy : set Vx = Vx OR Vy
void chip8::OP_8XY1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

// AND Vx, Vy : set Vx = Vx AND Vy
void chip8::OP_8XY2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

// AND Vx, Vy : set Vx = Vx XOR Vy
void chip8::OP_8XY3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

// ADD Vx, Vy : set Vx = Vx + Vy, set VF = carry
void chip8::OP_8XY4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255u)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0x00FFu;
}

// SUB Vx , Vy : set Vx = Vx - Vy, set VF = NOt borrow
void chip8::OP_8XY5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

// SHR Vx : set Vx = Vx SHR 1
void chip8::OP_8XY6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x1u);

    registers[Vx] >>= 1;
}

// SUBN Vx, Vy : set Vx = Vy, set VF = not borrow
void chip8::OP_8XY7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

// SHL Vx {,Vy} : set Vx = Vx SHL 1
void chip8::OP_8XYE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}

// SNE Vx, Vy : skip next instruction if Vx != Vy
void chip8::OP_9XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        program_counter += 2;
    }
}

// LD I, addr : set I = nnn;
void chip8::OP_ANNN()
{
    uint16_t address = opcode & 0x0FFFu;

    index_register = address;
}

// JP V0, addr : jump to location nnn + V0
void chip8::OP_BNNN()
{
    uint16_t address = opcode & 0x0FFFu;

    program_counter = address + registers[0x0u];
}

// RND Vx, kk : set Vx = random byte AND kk
void chip8::OP_CXKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    registers[Vx] = randByte(randGen) & kk;
}

// DRW Vx, Vy, nibble : display n-byte sprite starting at memeory location I at(Vx,Vy), set VF = collision
void chip8::OP_DXYN()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t n = (opcode & 0x000Fu);

    // wrap around screen if going over the boundry
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < n; row++)
    {
        uint8_t spriteByte = memory[index_register + row];
        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *displayPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                if (*displayPixel == 0xFFFFFFFF)
                {
                    registers[0xf] = 1;
                }
                *displayPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// SKP Vx : skip next instruction if key with the value of Vx is pressed
void chip8::OP_EX9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if (key_pad[key])
    {
        program_counter += 2;
    }
}

// SKNP Vx : skip next isntriction if key with the value of Vx is not pressed
void chip8::OP_EXA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if (!key_pad[key])
    {
        program_counter += 2;
    }
}

// LD Vx, DT : set Vx = dealy timer value
void chip8::OP_FX07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delay_timer;
}

// LD Vx, K : wait for a key press, store the value of the key in Vx
void chip8::OP_FX0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (key_pad[0])
    {
        registers[Vx] = 0;
    }
    else if (key_pad[1])
    {
        registers[Vx] = 1;
    }
    else if (key_pad[2])
    {
        registers[Vx] = 2;
    }
    else if (key_pad[3])
    {
        registers[Vx] = 3;
    }
    else if (key_pad[4])
    {
        registers[Vx] = 4;
    }
    else if (key_pad[5])
    {
        registers[Vx] = 5;
    }
    else if (key_pad[6])
    {
        registers[Vx] = 6;
    }
    else if (key_pad[7])
    {
        registers[Vx] = 7;
    }
    else if (key_pad[8])
    {
        registers[Vx] = 8;
    }
    else if (key_pad[9])
    {
        registers[Vx] = 9;
    }
    else if (key_pad[10])
    {
        registers[Vx] = 10;
    }
    else if (key_pad[11])
    {
        registers[Vx] = 11;
    }
    else if (key_pad[12])
    {
        registers[Vx] = 12;
    }
    else if (key_pad[13])
    {
        registers[Vx] = 13;
    }
    else if (key_pad[14])
    {
        registers[Vx] = 14;
    }
    else if (key_pad[15])
    {
        registers[Vx] = 15;
    }
    else
    {
        program_counter -= 2;
    }
}

// LD DT, Vx : set deaky timer = Vx
void chip8::OP_FX15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delay_timer = registers[Vx];
}

// LD ST, Vx : set sound timer = Vx
void chip8::OP_FX18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    sound_timer = registers[Vx];
}

// ADD I, Vx : set I = I + Vx
void chip8::OP_FX1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index_register += registers[Vx];
}

// LD F, Vx : set I = location of the sprite for digit Vx
void chip8::OP_FX29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index_register = FONTSET_START_ADDRESS + (5 * digit);
}

// LD B, Vx : store BCD representation of Vx in memeory location I, I+1, and I+2
void chip8::OP_FX33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t number = registers[Vx];

    // ones
    memory[index_register + 2] = number % 10;
    number /= 10;

    // tens
    memory[index_register + 1] = number % 10;
    number /= 10;

    // hundreds
    memory[index_register] = number % 10;
}

// LD [I], Vx : store registers V0 through vx in memory starting at location I
void chip8::OP_FX55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (unsigned int i = 0; i < Vx; i++)
    {
        memory[index_register + i] = registers[i];
    }
}

// LD Vx, [I] : read registers V0 through Vx from memory starting at location I
void chip8::OP_FX65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (unsigned int i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index_register + i];
    }
}

void chip8::OP_NULL()
{
}