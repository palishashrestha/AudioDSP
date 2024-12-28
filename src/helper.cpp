#include "helper.h"
#include "logger.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

/// Generic function to validate input size
void validate_size(int size, int threshold, const std::string &message, bool logOnce)
{
    if (size <= threshold)
    {
        logMessage("Validation failed: " + message, "ERROR", logOnce);
        throw std::invalid_argument(message);
    }
    logMessage("Validation passed for size: " + std::to_string(size), "INFO", logOnce);
}

/// Utility to print a line of symbols
void print_line(int length, char symbol, bool logOnce)
{
    logMessage("Printing line of length: " + std::to_string(length) + " with symbol: " + std::string(1, symbol), "INFO", logOnce);
    for (int j = 0; j < length; j++)
    {
        std::cout << symbol;
    }
    std::cout << '\n';
}

/// Histogram Plotter
void show_bargraph(int bars[], int n_bars, bool logOnce, int height, int hScale, float vScale, char symbol)
{
    validate_size(n_bars, 0, "Number of bars must be greater than zero.", logOnce);
    logMessage("Generating bar graph with " + std::to_string(n_bars) + " bars and height: " + std::to_string(height), "INFO", logOnce);

    std::vector<std::string> graph(height + 1, std::string(n_bars * hScale, ' '));

    for (int j = 0; j < n_bars; ++j)
    {
        int bar_height = std::min(static_cast<int>(bars[j] * vScale), height);
        for (int i = 0; i < bar_height; ++i)
        {
            std::fill_n(&graph[height - i][j * hScale], hScale, symbol);
        }
    }

    for (size_t i = 0; i < graph.size(); ++i)
    {
        std::cout << graph[i] << '\n';
    }
    print_line(n_bars * hScale, symbol, logOnce); // Base line
}

/// Frequency Mapping Functions
float index2freq(int index, bool logOnce)
{
    float freq = 2 * static_cast<float>(index) * RATE / FFTLEN;
    logMessage("Converted index " + std::to_string(index) + " to frequency: " + std::to_string(freq), "INFO", logOnce);
    return freq;
}

float freq2index(float freq, bool logOnce)
{
    float index = 0.5 * freq * FFTLEN / RATE;
    logMessage("Converted frequency " + std::to_string(freq) + " to index: " + std::to_string(index), "INFO", logOnce);
    return index;
}

float mapLin2Log(float LinMin, float LinRange, float LogMin, float LogRange, float LinVal, bool logOnce)
{
    if (LinVal < LinMin)
    {
        logMessage("Linear value " + std::to_string(LinVal) + " is out of range.", "ERROR", logOnce);
        throw std::out_of_range("Linear value is out of range.");
    }
    float result = LogMin + (std::log(LinVal + 1 - LinMin) / std::log(LinRange + LinMin)) * LogRange;
    logMessage("Mapped linear value " + std::to_string(LinVal) + " to logarithmic scale: " + std::to_string(result), "INFO", logOnce);
    return result;
}

/// Approximate HCF
float approx_hcf(float inputs[], int num_inputs, bool logOnce, int max_iter, int accuracy_threshold)
{
    validate_size(num_inputs, 1, "At least two inputs are required to compute HCF.", logOnce);
    logMessage("Starting HCF computation for " + std::to_string(num_inputs) + " inputs.", "INFO", logOnce);

    if (num_inputs == 2)
    {
        float ratio = std::max(inputs[0], inputs[1]) / std::min(inputs[0], inputs[1]);
        if (ratio > accuracy_threshold)
        {
            logMessage("Inputs are not accurate enough for HCF computation.", "WARNING", logOnce);
            return 0;
        }
        return inputs[0] / ratio;
    }

    std::vector<float> temp_inputs(inputs, inputs + num_inputs);
    float smaller_hcf = approx_hcf(temp_inputs.data() + 1, num_inputs - 1, logOnce, max_iter, accuracy_threshold);
    float hcf_inputs[2] = {temp_inputs[0], smaller_hcf};
    float result = approx_hcf(hcf_inputs, 2, logOnce, max_iter, accuracy_threshold);

    logMessage("Computed approximate HCF: " + std::to_string(result), "INFO", logOnce);
    return result;
}

/// Find Largest N Elements
void Find_n_Largest(int *output, sample *input, int n_out, int n_in, bool logOnce, bool ignore_clumped)
{
    validate_size(n_in, 0, "Input array size must be greater than zero.", true);
    logMessage("Finding " + std::to_string(n_out) + " largest elements from array of size " + std::to_string(n_in), "INFO", logOnce);

    std::vector<std::pair<int, sample>> indexed_input;

    for (int i = 0; i < n_in; ++i)
    {
        indexed_input.push_back(std::make_pair(i, input[i]));
    }

    std::sort(indexed_input.begin(), indexed_input.end(), [](const std::pair<int, sample> &a, const std::pair<int, sample> &b)
              { return a.second > b.second; });

    int count = 0;
    for (size_t i = 0; i < indexed_input.size() && count < n_out; ++i)
    {
        int index = indexed_input[i].first;
        if (ignore_clumped && count > 0 && std::abs(output[count - 1] - index) == 1)
            continue;
        output[count++] = index;
    }

    logMessage("Found largest elements: count = " + std::to_string(count), "INFO", logOnce);
}

/// Pitch Detection
int pitchNumber(float freq, bool logOnce, float *centsSharp)
{
    const double semitone = std::pow(2.0, 1.0 / 12.0);
    if (freq <= 0)
    {
        logMessage("Invalid frequency: " + std::to_string(freq), "ERROR", logOnce);
        throw std::invalid_argument("Frequency must be positive.");
    }

    while (freq < 440)
        freq *= 2;
    while (freq > 880)
        freq /= 2;

    int pitch_num = static_cast<int>(std::round(std::log(freq / 440.0) / std::log(semitone)));
    pitch_num = std::clamp(pitch_num, 0, 11);

    if (centsSharp)
    {
        *centsSharp = 1200 * std::log2(freq / (440.0 * std::pow(semitone, pitch_num)));
    }

    logMessage("Computed pitch number: " + std::to_string(pitch_num) + " for frequency: " + std::to_string(freq), "INFO", logOnce);
    return pitch_num + 1;
}

int pitchName(char *name, int pitch_num, bool logOnce)
{
    static const char *names[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    if (pitch_num < 1 || pitch_num > 12)
    {
        logMessage("Invalid pitch number: " + std::to_string(pitch_num), "ERROR", logOnce);
        throw std::out_of_range("Invalid pitch number.");
    }
    std::string pitch = names[pitch_num - 1];
    std::copy(pitch.begin(), pitch.end(), name);

    logMessage("Computed pitch name: " + pitch + " for pitch number: " + std::to_string(pitch_num), "INFO", logOnce);
    return pitch.size();
}
