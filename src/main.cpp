#include "logger.h"
#include "visualizer.h"
#include <iostream>
#include <filesystem>
#include <math.h>
#include <complex>
#include <windows.h>
#include <conio.h>
#include <climits>
#include <SDL2/SDL.h>

#define REFRESH_TIME 10 // Refresh rate in milliseconds

float echoVolume;                    // Echo playback volume
AudioQueue MainAudioQueue(10000000); // Main AudioQueue for recording and playback

/**
 * @brief Callback for recording audio data.
 *
 * Pushes audio samples from the recording stream into the MainAudioQueue.
 * @param userdata Unused user data pointer.
 * @param stream Pointer to the audio stream buffer.
 * @param streamLength Length of the audio stream buffer in bytes.
 */
void RecCallback(void *userdata, Uint8 *stream, int streamLength)
{
    Uint32 length = (Uint32)streamLength;
    MainAudioQueue.push((sample *)stream, length / sizeof(sample));
}

/**
 * @brief Callback for playing back audio data.
 *
 * Pops audio samples from the MainAudioQueue and writes them to the playback stream.
 * @param userdata Unused user data pointer.
 * @param stream Pointer to the audio stream buffer.
 * @param streamLength Length of the audio stream buffer in bytes.
 */
void PlayCallback(void *userdata, Uint8 *stream, int streamLength)
{
    Uint32 length = (Uint32)streamLength;
    MainAudioQueue.pop((sample *)stream, length / sizeof(sample), ::echoVolume);
}

/**
 * @brief Initializes SDL audio devices for recording and playback.
 *
 * Sets up the SDL audio system, configures recording and playback devices, and starts audio processing.
 * @param RecDevice Reference to the recording audio device ID.
 * @param PlayDevice Reference to the playback audio device ID.
 * @throws std::runtime_error if initialization fails for either recording or playback device.
 */
void InitializeAudio(SDL_AudioDeviceID &RecDevice, SDL_AudioDeviceID &PlayDevice)
{
    SDL_Init(SDL_INIT_AUDIO); // Initialize SDL audio
    logMessage("Initializing SDL audio", "INFO");

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
        logMessage("Failed to open playback device: " + std::string(SDL_GetError()), "ERROR");
        throw std::runtime_error("Failed to open playback device: " + std::string(SDL_GetError()));
    }
    if (RecDevice <= 0)
    {
        logMessage("Failed to open recording device: " + std::string(SDL_GetError()), "ERROR");
        throw std::runtime_error("Failed to open recording device: " + std::string(SDL_GetError()));
    }

    SDL_PauseAudioDevice(RecDevice, 0);  // Start recording
    SDL_Delay(2000);                     // Fill audio buffer
    SDL_PauseAudioDevice(PlayDevice, 0); // Start playback

    logMessage("Audio devices initialized successfully", "INFO");
}

/**
 * @brief Prompts the user for input and validates the range.
 *
 * Ensures the user input is within the specified range and provides feedback for invalid input.
 * @param prompt The message to display to the user.
 * @param minValue The minimum allowable value.
 * @param maxValue The maximum allowable value.
 * @return The validated integer input from the user.
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
            logMessage("Invalid input received", "WARNING");
        }
        else
        {
            logMessage("Valid input received: " + std::to_string(value), "INFO");
            break;
        }
    }
    return value;
}

/**
 * @brief Handles visualization based on user selection.
 *
 * Reduces redundancy in the main function by encapsulating visualization logic.
 * @param choice The user-selected visualization option.
 * @param lim1 The lower frequency limit.
 * @param lim2 The upper frequency limit.
 * @param adaptive Whether adaptive scaling is enabled.
 * @param consoleWidth The width of the console window.
 * @param consoleHeight The height of the console window.
 * @param logOnce Whether to log certain messages only once.
 */
void runVisualizer(int choice, int lim1, int lim2, bool adaptive, int consoleWidth, int consoleHeight, bool logOnce)
{
    // Visualizer Objects
    SemilogVisualizer semilogVis;
    LinearVisualizer linearVis;
    LoglogVisualizer loglogVis;

    switch (choice)
    {
    case 1:
    case 4:
        semilogVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, logOnce, adaptive);
        break;
    case 2:
    case 5:
        linearVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, logOnce, adaptive);
        break;
    case 3:
    case 6:
        loglogVis.visualize(MainAudioQueue, lim1, lim2, consoleWidth, consoleHeight, logOnce, adaptive);
        break;
    case 7:
    case 8:
        SpectralTuner(MainAudioQueue, consoleWidth, consoleHeight, logOnce, adaptive);
        break;
    case 9:
        AutoTuner(MainAudioQueue, consoleWidth, logOnce);
        break;
    case 10:
        ChordGuesser(MainAudioQueue, logOnce);
        break;
    default:
        logMessage("Invalid visualizer option selected", "ERROR");
        throw std::invalid_argument("Invalid visualizer option");
    }
    SDL_Delay(REFRESH_TIME);
}

/**
 * @brief Displays the main menu and prompts the user to select an option.
 *
 * Provides a list of visualization and algorithm options.
 * @return The user-selected menu option.
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

/**
 * @brief Captures a single key press from the user.
 *
 * Checks for and returns a character if a key is pressed.
 * @return The captured character, or 0 if no key is pressed.
 */
char capture_button_press()
{
    if (_kbhit())
        return getch();
    return 0;
}

/**
 * @brief Entry point of the application.
 *
 * Initializes audio, displays the menu, and handles user input and visualization.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Exit status of the application.
 */
int main(int argc, char **argv)
{
    SDL_AudioDeviceID RecDevice, PlayDevice;

    try
    {
        logMessage("Application started", "INFO");
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
        logMessage("Running visualizer with choice: " + std::to_string(choice), "INFO");

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int consoleWidth, consoleHeight;

        std::cout << "\nStarting... Press 'x' to exit or 'm' to return to menu.\n";
        SDL_Delay(1000);
        system("cls");
        static bool logOnce = true;

        for (int i = 0; i < 600000 / REFRESH_TIME; i++)
        {
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left;
            consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top;
            runVisualizer(choice, lowerFreq, upperFreq, (choice >= 4 && choice <= 8), consoleWidth, consoleHeight, logOnce);
            logOnce = false;
            char input = capture_button_press();
            if (input == 'x')
                break;
            if (input == 'm')
                goto MAIN_MENU;
        }

        SDL_CloseAudioDevice(PlayDevice);
        SDL_CloseAudioDevice(RecDevice);
        logMessage("Application terminated successfully", "INFO");
    }
    catch (const std::exception &e)
    {
        logMessage("Error: " + std::string(e.what()), "ERROR");
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
