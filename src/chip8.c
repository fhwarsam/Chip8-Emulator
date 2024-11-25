#include "chip8.h"
#include <memory.h>
#include <assert.h>
const char chip8_default_character_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // character 0
    0x20, 0x60, 0x20, 0x20, 0x70, // character 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // character 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // character 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // character 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // character 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // character 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // character 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // character 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // character 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // character A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // character B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // character C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // character D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // character E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //  character F

};
void chip8_init(struct chip8 *chip8)
{

    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
}

void chip8_load(struct chip8 *chip8, const char *buf, size_t size)
{
    assert(size + CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);
    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registers.program_counter = CHIP8_PROGRAM_LOAD_ADDRESS;
}
static void chip8_exec_extended_eight(struct chip8 *chip8, unsigned short opcode)
{
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x00f;
    unsigned char final_four_bit = opcode & 0x00f;

    switch (final_four_bit)
    {
    // 8xy0 - LD Vx, Vy. Vx = Vy
    case 0x00:
        chip8->registers.V[x] = chip8->registers.V[y];
        break;

    // 0x8xy1 - OR Vx, Vy. Perform bitwise OR on Vx and Vy - stores the result in Vx
    case 0x01:
        chip8->registers.V[x] |= chip8->registers.V[y];

        break;

    // 0x8xy2 - AND Vx, Vy. Perform bitwise AND on Vx and Vy - store the result in Vx
    case 0x02:
        chip8->registers.V[x] = chip8->registers.V[x] & chip8->registers.V[y];
        break;

    // 8xy2  - XOR Vx, Vy. Perform XOR on Vx and Vy and store the result in Vx
    case 0x03:
        chip8->registers.V[x] = chip8->registers.V[x] ^ chip8->registers.V[y];
        break;


        // 8xy4  - AND Vx, Vy. Perform XOR on Vx and Vy and store the result in Vx
    case 0x04:
        chip8->registers.V[x] = chip8->registers.V[x] ^ chip8->registers.V[y];
        break;
    }
}
// Function to be executed when opcode is not clearing the screen program counter incremeneting
static void chip8_exec_extended(struct chip8 *chip8, unsigned short opcode)
{
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x00f;
    unsigned char kk = opcode & 0x00ff;
    switch (opcode & 0xf00)
    {
    // jp addr, 1nnn jump to location nnn's
    case 0x1000:
        chip8->registers.program_counter = nnn;
        break;

    // Call addr, 2nnn call subroutine at location nnn
    case 0x2000:
        chip8_stack_push(chip8, chip8->registers.program_counter);
        chip8->registers.program_counter = nnn;
        break;

    // SE Vx, byte- 3xkk skip nexr instruction if vx ==kk
    case 0x3000:
        if (chip8->registers.V[x] == kk)
        {
            chip8->registers.program_counter += 2;
        }

        break;

        // SNE Vx, byte- 3xkk skip nexr instruction if vx !=kk
    case 0x4000:
        if (chip8->registers.V[x] != kk)
        {
            chip8->registers.program_counter += 2;
        }

        break;

    // 5xy0 - SE, Vx, Vy, skip the next instruction if Vx == Vy
    case 0x5000:
        if (chip8->registers.V[x] == chip8->registers.V[y])
        {
            chip8->registers.program_counter += 2;
        }
        break;

    // LD Vx, byte, Vx ==kk
    case 0x6000:
        chip8->registers.V[x] = kk;

        break;

    // 7xkk - ADD Vx, byte. set Vx = Vx+kk
    case 0x7000:
        chip8->registers.V[x] += kk;

        break;

    case 0x8000:
        chip8_exec_extended_eight(&chip8, opcode);
    }
}

void chip8_exec(struct chip8 *chip8, unsigned short opcode)
{
    switch (opcode)
    {
    case 0x00e:
        chip8_screen_clear(&chip8->screen);
        break;

    case 0x00EE:
        chip8->registers.program_counter = chip8_stack_pop(chip8);
        break;

    default:
        chip8_exec_extended(chip8, opcode);
    }
}