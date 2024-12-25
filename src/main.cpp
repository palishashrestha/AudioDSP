#include "logger.h"
#include <iostream>
#include <filesystem>
#include <math.h>
#include <complex>
#include <windows.h>
#include <conio.h>
#include <climits>
#include <SDL2/SDL.h>
#include "visualizer.h"

#define REFRESH_TIME 10 // Refresh rate in milliseconds

float echoVolume;                    // Echo playback volume
AudioQueue MainAudioQueue(10000000); // Main AudioQueue for recording and playback

void RecCallback(void *userdata, Uint8 *stream, int streamLength)
{
    Uint32 length = (Uint32)streamLength;
    MainAudioQueue.push((sample *)stream, length / sizeof(sample));
    Logger::getInstance().log("RecCallback executed");
}

void PlayCallback(void *userdata, Uint8 *stream, int streamLength)
{
    Uint32 length = (Uint32)streamLength;
    MainAudioQueue.pop((sample *)stream, length / sizeof(sample), ::echoVolume);
    Logger::getInstance().log("PlayCallback executed");
}

/**
 * Error handling wrapper for SDL audio initialization
 */
void InitializeAudio(SDL_AudioDeviceID &RecDevice, SDL_AudioDeviceID &PlayDevice)
{
    SDL_Init(SDL_INIT_AUDIO); // Initialize SDL audio
    Logger::getInstance().log("Initializing SDL audio");

    SDL_AudioSpec RecSpec{}, PlaySpec{};
    RecSpec.freq = RATE;
    RecSpec.format = AUDIO_S16SYS;
    RecSpec.samples = CHUNK;
    RecSpec.callback = RecCallback; // Callback for recording
    RecSpec.channels = 1;

    PlaySpec = RecSpec;
    PlaySpec.callback = PlayCallback;

    RecDevice = SDL_OpenAudioDevice(NULL, 1, &RecSpec, NULL, 0);
    PlayDevice = SDL_OpenAudioDevice(NULL, 0, &PlaySpec, NULL, 0);

    if (PlayDevice <= 0)
    {
        Logger::getInstance().log("Failed to open playback device: " + std::string(SDL_GetError()));
        throw std::runtime_error("Failed to open playback device: " + std::string(SDL_GetError()));
    }
    if (RecDevice <= 0)
    {
        Logger::getInstance().log("Failed to open recording device: " + std::string(SDL_GetError()));
        throw std::runtime_error("Failed to open recording device: " + std::string(SDL_GetError()));
    }

    SDL_PauseAudioDevice(RecDevice, 0);  // Start recording
    SDL_Delay(2000);                     // Fill audio buffer
    SDL_PauseAudioDevice(PlayDevice, 0); // Start playback

    Logger::getInstance().log("Audio devices initialized successfully");
}

/**
 * Function to get user input with basic validation
 */
int getValidatedInput(const std::string &prompt, int minValue, int maxValue)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail() || value < minValue || value > maxValue)
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::cout << "Invalid input. Try again.\n";
            Logger::getInstance().log("Invalid input received");
        }
        else
        {
            Logger::getInstance().log("Valid input received: " + std::to_string(value));
            break;
        }
    }
    return value;
}

/**
 * Main visualization handler to reduce switch-case redundancy
 */
void runVisualizer(int choice, int lim1, int lim2, bool adaptive, int consoleWidth, int consoleHeight)
{
    // Visualizer Objects
    SemilogVisualizer semilogVis;
    LinearVisualizer linearVis;
    LoglogVisualizer loglogVis;

    Logger::getInstance().log("Running visualizer with choice: " + std::to_string(choice));

    switch (choice)
    {
    case 1:
    case 4:
        semilogVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, adaptive);
        break;
    case 2:
    case 5:
        linearVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, adaptive);
        break;
    case 3:
    case 6:
        loglogVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, adaptive);
        break;
    case 7:
    case 8:
        SpectralTuner(MainAudioQueue, consoleWidth, consoleHeight, adaptive);
        break;
    case 9:
        AutoTuner(MainAudioQueue, consoleWidth);
        break;
    case 10:
        ChordGuesser(MainAudioQueue);
        break;
    default:
        Logger::getInstance().log("Invalid visualizer option selected");
        throw std::invalid_argument("Invalid visualizer option");
    }
    SDL_Delay(REFRESH_TIME);
}

/**
 * Main menu display
 */
int displayMenu()
{
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    std::cout << "VISUALIZER OPTIONS\n"
              << "\nScaled Spectrum\n----------------"
              << "\n1 . Fixed semilog"
              << "\n2 . Fixed linear"
              << "\n3 . Fixed log-log"
              << "\n4 . Adaptive semilog"
              << "\n5 . Adaptive linear"
              << "\n6 . Adaptive log-log"
              << "\n\nWrapped Spectrum (Spectral Guitar Tuner)\n----------------------------------------"
              << "\n7 . Fixed"
              << "\n8 . Adaptive"
              << "\n\nMusic Algorithms\n----------------"
              << "\n9 . Pitch recognition (automatic tuner)"
              << "\n10. Chord Guesser"
              << "\n\nEnter choice: ";
    return getValidatedInput("", 1, 10);
}

char capture_button_press()
{
    if (_kbhit())
        return getch();
    return 0;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    SDL_AudioDeviceID RecDevice, PlayDevice;

    try
    {
        Logger::getInstance().log("Application started");
        InitializeAudio(RecDevice, PlayDevice);

        int choice, lowerFreq, upperFreq;
        bool adaptive = false;

    MAIN_MENU:
        system("cls");
        choice = displayMenu();

        if (choice < 7)
        {
            lowerFreq = getValidatedInput("Enter lower frequency limit: ", 20, 10000);
            upperFreq = getValidatedInput("Enter upper frequency limit: ", lowerFreq + 1, 20000);
        }

        echoVolume = getValidatedInput("Enter echo volume (0 = no echo): ", 0, 100);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int consoleWidth, consoleHeight;

        std::cout << "\nStarting... Press 'x' to exit or 'm' to return to menu.\n";
        SDL_Delay(1000);
        system("cls");

        for (int i = 0; i < 600000 / REFRESH_TIME; i++)
        {
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left;
            consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top;

            runVisualizer(choice, lowerFreq, upperFreq, (choice >= 4 && choice <= 8), consoleWidth, consoleHeight);

            char input = capture_button_press();
            if (input == 'x')
                break;
            if (input == 'm')
                goto MAIN_MENU;
        }

        SDL_CloseAudioDevice(PlayDevice);
        SDL_CloseAudioDevice(RecDevice);
        Logger::getInstance().log("Application terminated successfully");
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().log("Error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
