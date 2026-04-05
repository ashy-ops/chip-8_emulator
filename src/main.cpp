#include "chip8.h"
#include "platform.h"

using namespace std;

int main()
{
    float videoScale, cycleDelay;
    string romFilename;

    cout << "Enter video scale : ";
    cin >> videoScale;

    cout << "Enter cycle delay (ms): ";
    cin >> cycleDelay;

    cout << "Enter ROM filename: ";
    cin >> romFilename;

    // Create SDL Platform
    Platform platform("CHIP-8 Emulator",
                      64 * videoScale,
                      32 * videoScale,
                      64,
                      32);

    CHIP8 chip8;
    chip8.Initialize();
    chip8.LoadROM(romFilename);

    int videoPitch = sizeof(chip8.video[0]) * 64;

    auto lastCycleTime = chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = chrono::high_resolution_clock::now();
        float dt = chrono::duration<float, chrono::milliseconds::period>(
            currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);

            platform.PlayBeep(chip8.SoundTimer);
        }
    }

    return 0;
}