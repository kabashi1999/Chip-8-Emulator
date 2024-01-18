#include <iostream>
#include <chrono>
#include "platform.h"
#include "chip8.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage : " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFileName = argv[3];

    char title[] = "CHIP-8 Emulator";

    platform platform(title, chip8::VIDEO_WIDTH * videoScale, chip8::VIDEO_HEIGHT * videoScale, chip8::VIDEO_WIDTH, chip8::VIDEO_HEIGHT);

    chip8 chip8;
    chip8.LoadROM(romFileName);

    int videoPitch = sizeof(chip8.video[0]) * chip8::VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.key_pad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.Cycle();

            platform.Update(chip8.video, videoPitch);
        }
    }

    std::cout << "error" << std::endl;
    return 0;
}
