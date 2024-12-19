#include "visualizer.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

/// ----- Base Visualizer Class Methods -----

void Visualizer::initializeHistogram()
{
    bargraph.assign(numbars, 0);
}

void Visualizer::applyAdaptiveScaling(bool adaptive, float &graphScale)
{
    if (!adaptive)
        return;

    int maxv = *std::max_element(bargraph.begin(), bargraph.end());
    if (maxv > 0)
        graphScale = 1.0f / maxv;
}

void Visualizer::smoothHistogram()
{
    for (int i = 1; i < numbars - 1; i++)
    {
        if (bargraph[i] == 0)
            bargraph[i] = (bargraph[i - 1] + bargraph[i + 1]) / 2;
    }
}

/// ----- Semilog Visualizer -----
void SemilogVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbars = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram();

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    int Freq0idx = freq2index(minfreq);
    int FreqLidx = freq2index(maxfreq);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = static_cast<int>(mapLin2Log(Freq0idx, FreqLidx - Freq0idx, 0, numbars, i));
        bargraph[index] += spectrum[i] / i;
    }

    smoothHistogram();
    applyAdaptiveScaling(adaptive, graphScale);

    system("cls");
    show_bargraph(bargraph.data(), numbars, graphheight, 1, graphScale * graphheight, ':');
}

/// ----- Linear Visualizer -----
void LinearVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbars = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram();

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    int bucketwidth = FFTLEN / numbars;
    int Freq0idx = freq2index(minfreq);
    int FreqLidx = freq2index(maxfreq);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = numbars * (i - Freq0idx) / (FreqLidx - Freq0idx);
        bargraph[index] += spectrum[i] / bucketwidth;
    }

    smoothHistogram();
    applyAdaptiveScaling(adaptive, graphScale);

    system("cls");
    show_bargraph(bargraph.data(), numbars, graphheight, 1, graphScale * graphheight, ':');
}

/// ----- Loglog Visualizer -----
void LoglogVisualizer::visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    numbars = consoleWidth;
    graphheight = consoleHeight;
    initializeHistogram();

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    int Freq0idx = freq2index(minfreq);
    int FreqLidx = freq2index(maxfreq);

    for (int i = Freq0idx; i < FreqLidx; i++)
    {
        int index = static_cast<int>(mapLin2Log(Freq0idx, FreqLidx - Freq0idx, 0, numbars, i));
        bargraph[index] += spectrum[i] / i;
    }

    applyAdaptiveScaling(adaptive, graphScale);
    system("cls");
    show_bargraph(bargraph.data(), numbars, graphheight, 1, graphScale * graphheight, ':');
}

/// ----- Spectral Tuner -----
void SpectralTuner(AudioQueue &MainAudioQueue, int consoleWidth, int consoleHeight, bool adaptive, float graphScale)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    const int numbars = consoleWidth;
    const int graphheight = consoleHeight - 3; /// Minus 3 to leave room for pitch labels.

    std::vector<int> bargraph(numbars, 0);
    std::vector<float> octaveIndices(numbars + 1);

    /// Calculate fractional indices for one octave
    for (int i = 0; i <= numbars; i++)
    {
        octaveIndices[i] = freq2index(55.0 * pow(2, (float)i / numbars)); // Starts from A1 = 55Hz
    }

    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    /// Map frequencies to histogram
    for (int i = 0; i < numbars; i++)
    {
        float startIdx = octaveIndices[i];
        float endIdx = octaveIndices[i + 1];
        for (int j = round(startIdx); j < round(endIdx); j++)
        {
            bargraph[i] += spectrum[j] / (endIdx - startIdx); // Normalize based on width
        }
    }

    /// Adaptive Scaling
    if (adaptive)
    {
        int maxVal = *std::max_element(bargraph.begin(), bargraph.end());
        if (maxVal > 0)
            graphScale = 1.0f / maxVal;
    }

    system("cls");
    std::cout << "A    A#   B    C    C#   D    D#   E    F    F#   G    G#\n";
    show_bargraph(bargraph.data(), numbars, graphheight, 1, graphScale * graphheight, '=');
}

/// ----- Auto Tuner -----
void AutoTuner(AudioQueue &MainAudioQueue, int consoleWidth, int span_semitones)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    // Fetch audio data and compute frequency content
    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    const int numSpikes = 5;
    int spikeIndices[numSpikes];
    float spikeFrequencies[numSpikes];

    /// Find peaks in spectrum
    Find_n_Largest(spikeIndices, spectrum, numSpikes, FFTLEN / 2);
    for (int i = 0; i < numSpikes; i++)
    {
        spikeFrequencies[i] = index2freq(spikeIndices[i]);
    }

    /// Estimate pitch using HCF approximation
    float pitch = approx_hcf(spikeFrequencies, numSpikes, 5, 5);
    if (pitch <= 0)
    {
        std::cerr << "No pitch detected.\n";
        return;
    }

    /// Determine pitch number and cents deviation
    float centsOff = 0.0f;
    int pitchNum = pitchNumber(pitch, &centsOff);

    /// Use std::vector for dynamic size
    std::vector<char> notenames(consoleWidth + 1, ' ');

    int centerPosition = consoleWidth / 2 - static_cast<int>(centsOff * consoleWidth / (span_semitones * 100));
    centerPosition = std::max(0, std::min(centerPosition, consoleWidth - 2)); // Keep it in bounds

    pitchName(notenames.data() + centerPosition, pitchNum);

    notenames[consoleWidth] = '\0'; // Null-terminate the string

    /// Clear the screen and display the output
    system("cls");
    std::cout << "Output note name is: " << notenames.data() << "\n";
}

void ChordGuesser(AudioQueue &MainAudioQueue, int max_notes)
{
    sample workingBuffer[FFTLEN];
    sample spectrum[FFTLEN];

    // Fetch audio data and compute frequency content
    MainAudioQueue.peekFreshData(workingBuffer, FFTLEN);
    FindFrequencyContent(spectrum, workingBuffer, FFTLEN);

    const int numSpikes = 10; // Analyze up to 10 peaks
    int spikeIndices[numSpikes];
    float spikeFrequencies[numSpikes];

    /// Find peaks in spectrum
    Find_n_Largest(spikeIndices, spectrum, numSpikes, FFTLEN / 2, false);
    for (int i = 0; i < numSpikes; i++)
    {
        spikeFrequencies[i] = index2freq(spikeIndices[i]);
    }

    /// Identify unique pitches
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
            chordTones.push_back(pitchNumber(spikeFrequencies[i]));
        }
    }

    /// Remove duplicate pitches
    std::sort(chordTones.begin(), chordTones.end());
    chordTones.erase(std::unique(chordTones.begin(), chordTones.end()), chordTones.end());

    /// Identify the chord
    char chordName[CHORD_NAME_SIZE] = {0};
    int nameLength = identify_chord(chordName, chordTones.data(), chordTones.size());

    /// Display the chord name
    if (nameLength > 0)
    {
        std::cout << "\nDetected Chord: " << chordName << " (";
        for (size_t i = 0; i < chordTones.size(); i++)
        {
            char noteName[3];
            pitchName(noteName, chordTones[i]);
            std::cout << noteName << (i < chordTones.size() - 1 ? " " : "");
        }
        std::cout << ")\n";
    }
    else
    {
        std::cout << "\nNo Chord Detected\n";
    }
}
