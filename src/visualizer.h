#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include <iostream>
#include <cmath>
#include "helper.h"
#include "chordDictionary.h"

/// Abstract Base Class for Visualizers
class Visualizer
{
protected:
    int numbers;     /// Number of bars in the histogram
    int graphheight; /// Height of the graph
    std::vector<int> bargraph;

    void initializeHistogram(bool logOnce);
    void applyAdaptiveScaling(bool adaptive, float &graphScale, bool logOnce);
    void smoothHistogram(bool logOnce);

public:
    virtual void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale = 0.0008) = 0;
};

/// Semilog Visualizer
class SemilogVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale = 0.0008) override;
};

/// Linear Visualizer
class LinearVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale = 0.0008) override;
};

/// Loglog Visualizer
class LoglogVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, bool logOnce, float graphScale = 0.0008) override;
};

/// Spectral Tuner
void SpectralTuner(AudioQueue &MainAudioQueue, int consoleWidth, int consoleHeight, bool logOnce, bool adaptive = false, float graphScale = 0.0008);
void AutoTuner(AudioQueue &MainAudioQueue, int consoleWidth, bool logOnce, int span_semitones = 4);
void ChordGuesser(AudioQueue &MainAudioQueue, bool logOnce, int max_notes = 4);

#endif // VISUALIZER_H
