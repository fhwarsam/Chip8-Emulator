
#include <stdio.h>
#include "C:\Users\farhanw0\OneDrive - CITY OF MINNEAPOLIS PARKING\Desktop\chip8\include/SDL2/SDL.h"
#include <stdbool.h>
#include "chip8.h"
#include<windows.h>

const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
    SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f};
int main(int argc, char **argv)
{

    if(argc < 2){
        printf("You must provide a file to load\n");
    }


    const char* filename = argv[1];
    printf("The filename to loas is: %s\n", filename);

    FILE* f = fopen(filename, "rb");
    if(!f){
        printf("Failed to open the file");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char buf[size];
    int res = fread(buf, size, 1, f);
    if(res != 1){
        printf("Failed to read from file");
        return -1;
    }
    
    // Testing if buffer contains data from the file
    //printf("%s\n", buf);
    
    struct chip8 chip8;
    chip8_init(&chip8);
     chip8_load(&chip8, buf, size);
    
    // chip8.registers.program_counter = 0;;
     chip8.registers.V[2] = 0x10;
     chip8.registers.V[3] = 0x10;
     chip8_exec(&chip8, 0x8010);
 
   // chip8.registers.sound_timer = 30;
    // chip8.registers.program_counter = 0x00;
    // chip8_exec(&chip8, 0x3022);
     //printf("%x\n", chip8.registers.V[0]);
    //  chip8.registers.V[0x00] = 0x22;
    // // chip8_exec(&chip8, 0x1ff3);
    //     printf("%x\n", chip8.registers.program_counter);
    
    // Test drawing the sprite-  hard-coded
     chip8_screen_draw_sprite(&chip8.screen, 33, 30, &chip8.memory.memory[0x00], 5);

    // Test for opcode-  hard-coded
    // chip8_exec(&chip8, 0x00E0);

     //chip8_screen_set(&chip8.screen, 10,1 );
    // printf("%i\n", chip8_keyboard_map(keyboard_map, 0xFF));
    //  Test for stack
    //  chip8.registers.stack_pointer = 0;
    //  chip8_stack_push(&chip8, 0xff);
    //  chip8_stack_push(&chip8, 0xaa);
    //  printf("%x\n ", chip8_stack_pop(&chip8));
    //  printf("%x\n ", chip8_stack_pop(&chip8));

    // Test for keyboard.
    // printf("%x\n", chip8_keyboard_map(keyboard_map, 0xff));
    // chip8_keyboard_dwon(&chip8.keyboard, 0x0f);
    // chip8_keyboard_up(&chip8.keyboard, 0x0f);
    // bool is_down = chip8_keyboard_is_dwon(&chip8.keyboard, 0xff);
    // printf("%x\n", (int) is_down);

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, 
        CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER,
        SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                goto out;
                break;

            case SDL_KEYDOWN:
            {
                char key = event.key.keysym.sym;
                int vkey = chip8_keyboard_map(keyboard_map, key);
                if (vkey != -1)
                {
                    chip8_keyboard_dwon(&chip8.keyboard, vkey);
                    //printf("Key is down %x\n", vkey);
                }
                // printf("Key is down %x\n", vkey);
            }
            break;

            case SDL_KEYUP:
            {
                char key = event.key.keysym.sym;
                int vkey = chip8_keyboard_map(keyboard_map, key);
                if (vkey != -1)
                {
                    chip8_keyboard_up(&chip8.keyboard, vkey);
                   // printf("Key is up %x\n", vkey);
                }
                // printf("Key is down %x\n", vkey);
            }
            break;
            }; // switch end bracket
        } // while loop end bracket

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        for (int x = 0; x < CHIP8_WIDTH; x++)
        {
            for (int y = 0; y < CHIP8_HEIGHT; y++)
            {
                if (chip8_screen_is_set(&chip8.screen, x, y))
                {

                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        SDL_RenderPresent(renderer);
        if(chip8.registers.delay_timer > 0){
            Sleep(100);
            chip8.registers.delay_timer -=1; 
            //printf( "DElay !!");

        }
        if(chip8.registers.sound_timer > 0){
            Beep(15000, 100*chip8.registers.sound_timer);
            chip8.registers.sound_timer =0;
        }

        unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.program_counter);
          //chip8.registers.program_counter += 2;
          chip8_exec(&chip8, opcode);
          chip8.registers.program_counter += 2;
          
         
         // Test for opcode from INVADERS
         // printf("%x\n", opcode);
    }
  

    printf("Hello world");
out:
    SDL_DestroyWindow(window);
    return 0;
}