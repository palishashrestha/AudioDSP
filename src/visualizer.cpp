#include "visualizer.h"
#include "logger.h" // Include Logger
#include <algorithm>
#include <stdexcept>
#include <cmath>

/// ----- Base Visualizer Class Methods -----

void Visualizer::initializeHistogram(bool logOnce)
{
    bargraph.assign(numbers, 0);
    logMessage("Initialized histogram with " + std::to_string(numbers) + " bars.", "INFO", logOnce);
}

void Visualizer::applyAdaptiveScaling(bool adaptive, float &graphScale, bool logOnce)
{
    if (!adaptive)
        return;

    int maxv = *std::max_element(bargraph.begin(), bargraph.end());
    if (maxv > 0)
        graphScale = 1.0f / maxv;

    logMessage("Applied adaptive scaling with graph scale: " + std::to_string(graphScale), "INFO", logOnce);
}

void Visualizer::smoothHistogram(bool logOnce)
{
    for (int i = 1; i < numbers - 1; i++)
    {
        if (bargraph[i] == 0)
            bargraph[i] = (bargraph[i - 1] + bargraph[i + 1]) / 2;
    }

    logMessage("Smoothed histogram for " + std::to_string(numbers) + " bars.", "INFO", logOnce);
}

/// ----- Semilog Visualizer -----
void SemilogVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale)
{
    logMessage("Semilog visualization started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbers = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram(logOnce);

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    int Freq0idx = freq2index(minfreq, logOnce);
    int FreqLidx = freq2index(maxfreq, logOnce);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = static_cast<int>(mapLin2Log(Freq0idx, FreqLidx - Freq0idx, 0, numbers, i, logOnce));
        bargraph[index] += spectrum[i] / i;
    }

    smoothHistogram(logOnce);
    applyAdaptiveScaling(adaptive, graphScale, logOnce);

    system("cls");
    show_bargraph(bargraph.data(), numbers, logOnce, graphheight, 1, graphScale * graphheight, ':');
    logMessage("Semilog visualization completed.", "INFO", logOnce);
}

/// ----- Linear Visualizer -----
void LinearVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale)
{
    logMessage("Linear visualization started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbers = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram(logOnce);

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    int bucketwidth = FFTLEN / numbers;
    int Freq0idx = freq2index(minfreq, logOnce);
    int FreqLidx = freq2index(maxfreq, logOnce);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = numbers * (i - Freq0idx) / (FreqLidx - Freq0idx);
        bargraph[index] += spectrum[i] / bucketwidth;
    }

    smoothHistogram(logOnce);
    applyAdaptiveScaling(adaptive, graphScale, logOnce);

    system("cls");
    show_bargraph(bargraph.data(), numbers, logOnce, graphheight, 1, graphScale * graphheight, ':');
    logMessage("Linear visualization completed.", "INFO", logOnce);
}

/// ----- Loglog Visualizer -----
void LoglogVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale)
{
    logMessage("Loglog visualization started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbers = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram(logOnce);

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    int Freq0idx = freq2index(minfreq, logOnce);
    int FreqLidx = freq2index(maxfreq, logOnce);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = static_cast<int>(mapLin2Log(Freq0idx, FreqLidx - Freq0idx, 0, numbers, i, logOnce));
        bargraph[index] += spectrum[i] / i;
    }

    applyAdaptiveScaling(adaptive, graphScale, logOnce);

    system("cls");
    show_bargraph(bargraph.data(), numbers, logOnce, graphheight, 1, graphScale * graphheight, ':');
    logMessage("Loglog visualization completed.", "INFO", logOnce);
}

/// ----- Spectral Tuner -----
void SpectralTuner(AudioQueue &MainAudioQueue, int consoleWidth, int consoleHeight, bool logOnce, bool adaptive, float graphScale)
{
    logMessage("Spectral tuner visualization started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    const int numbers = consoleWidth;
    const int graphheight = consoleHeight - 3; // Leave room for pitch labels

    std::vector<int> bargraph(numbers, 0);
    std::vector<float> octaveIndices(numbers + 1);

    for (int i = 0; i <= numbers; i++)
    {
        octaveIndices[i] = freq2index(55.0 * pow(2, (float)i / numbers), logOnce); // Start from A1 = 55Hz
    }

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    for (int i = 0; i < numbers; i++)
    {
        float startIdx = octaveIndices[i];
        float endIdx = octaveIndices[i + 1];
        for (int j = round(startIdx); j < round(endIdx); j++)
        {
            bargraph[i] += spectrum[j] / (endIdx - startIdx);
        }
    }

    if (adaptive)
    {
        int maxVal = *std::max_element(bargraph.begin(), bargraph.end());
        if (maxVal > 0)
            graphScale = 1.0f / maxVal;
    }

    system("cls");
    std::cout << "A    A#   B    C    C#   D    D#   E    F    F#   G    G#\n";
    show_bargraph(bargraph.data(), numbers, logOnce, graphheight, 1, graphScale * graphheight, '=');
    logMessage("Spectral tuner visualization completed.", "INFO", logOnce);
}

/// ----- Auto Tuner -----
void AutoTuner(AudioQueue &MainAudioQueue, int consoleWidth, bool logOnce, int span_semitones)
{
    logMessage("Auto tuner visualization started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    const int numSpikes = 5;
    int spikeIndices[numSpikes];
    float spikeFrequencies[numSpikes];

    Find_n_Largest(spikeIndices, spectrum, numSpikes, FFTLEN / 2, logOnce);
    for (int i = 0; i < numSpikes; i++)
    {
        spikeFrequencies[i] = index2freq(spikeIndices[i], logOnce);
    }

    float pitch = approx_hcf(spikeFrequencies, numSpikes, logOnce, 5, 5);
    if (pitch <= 0)
    {
        logMessage("No pitch detected.", "WARNING", logOnce);
        std::cerr << "No pitch detected.\n";
        return;
    }

    float centsOff = 0.0f;
    int pitchNum = pitchNumber(pitch, &centsOff);

    std::vector<char> notenames(consoleWidth + 1, ' ');

    int centerPosition = consoleWidth / 2 - static_cast<int>(centsOff * consoleWidth / (span_semitones * 100));
    centerPosition = std::max(0, std::min(centerPosition, consoleWidth - 2));

    pitchName(notenames.data() + centerPosition, pitchNum, logOnce);
    notenames[consoleWidth] = '\0';

    system("cls");
    std::cout << "Output note name is: " << notenames.data() << "\n";
    logMessage("Auto tuner visualization completed.", "INFO", logOnce);
}

/// ----- Chord Guesser -----
void ChordGuesser(AudioQueue &MainAudioQueue, bool logOnce, int max_notes)
{
    logMessage("Chord guesser started.", "INFO", logOnce);

    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN, logOnce);

    const int numSpikes = 10;
    int spikeIndices[numSpikes];
    float spikeFrequencies[numSpikes];

    Find_n_Largest(spikeIndices, spectrum, numSpikes, FFTLEN / 2, false);
    for (int i = 0; i < numSpikes; i++)
    {
        spikeFrequencies[i] = index2freq(spikeIndices[i], logOnce);
    }

    const float quartertone = pow(2.0, 1.0 / 24.0);
    std::vector<int> chordTones;

    for (int i = 0; i < numSpikes && chordTones.size() < max_notes; i++)
    {
        bool distinct = true;
        for (int tone : chordTones)
        {
            float separation = std::max(spikeFrequencies[i], static_cast<float>(tone)) / std::min(spikeFrequencies[i], static_cast<float>(tone));
            if (separation < quartertone)
            {
                distinct = false;
                break;
            }
        }
        if (distinct)
        {
            chordTones.push_back(pitchNumber(spikeFrequencies[i], logOnce));
        }
    }

    std::sort(chordTones.begin(), chordTones.end());
    chordTones.erase(std::unique(chordTones.begin(), chordTones.end()), chordTones.end());

    char chordName[CHORD_NAME_SIZE] = {0};
    int nameLength = identify_chord(chordName, chordTones.data(), chordTones.size());

    if (nameLength > 0)
    {
        std::cout << "\nDetected Chord: " << chordName << " (";
        for (size_t i = 0; i < chordTones.size(); i++)
        {
            char noteName[3];
            pitchName(noteName, chordTones[i], logOnce);
            std::cout << noteName << (i < chordTones.size() - 1 ? " " : "");
        }
        std::cout << ")\n";
        logMessage("Detected Chord: " + std::string(chordName), "INFO", logOnce);
    }
    else
    {
        std::cout << "\nNo Chord Detected\n";
        logMessage("No chord detected.", "WARNING", logOnce);
    }
    logMessage("Chord guesser completed.", "INFO", logOnce);
}
