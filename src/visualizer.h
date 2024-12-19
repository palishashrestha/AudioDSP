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
    int numbars;     /// Number of bars in the histogram
    int graphheight; /// Height of the graph
    std::vector<int> bargraph;

    void initializeHistogram();
    void applyAdaptiveScaling(bool adaptive, float &graphScale);
    void smoothHistogram();

public:
    virtual void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale = 0.0008) = 0;
};

/// Semilog Visualizer
class SemilogVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale = 0.0008) override;
};

/// Linear Visualizer
class LinearVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale = 0.0008) override;
};

/// Loglog Visualizer
class LoglogVisualizer : public Visualizer
{
public:
    void visualize(AudioQueue &MainAudioQueue, int minfreq, int maxfreq, int consoleWidth, int consoleHeight, bool adaptive, float graphScale = 0.0008) override;
};

/// Spectral Tuner
void SpectralTuner(AudioQueue &MainAudioQueue, int consoleWidth, int consoleHeight, bool adaptive = false, float graphScale = 0.0008);
void AutoTuner(AudioQueue &MainAudioQueue, int consoleWidth, int span_semitones = 4);
void ChordGuesser(AudioQueue &MainAudioQueue, int max_notes = 4);

#endif // VISUALIZER_H
