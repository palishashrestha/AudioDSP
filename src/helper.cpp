#include "helper.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

/// Generic function to validate input size
void validate_size(int size, int threshold, const std::string &message)
{
    if (size <= threshold)
    {
        throw std::invalid_argument(message);
    }
}

/// Utility to print a line of symbols
void print_line(int length, char symbol)
{
    for (int j = 0; j < length; j++)
    {
        std::cout << symbol;
    }
    std::cout << '\n';
}

/// Histogram Plotter
void show_bargraph(int bars[], int n_bars, int height, int hScale, float vScale, char symbol)
{
    validate_size(n_bars, 0, "Number of bars must be greater than zero.");
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
    print_line(n_bars * hScale, symbol); // Base line
}

/// Frequency Mapping Functions
float index2freq(int index)
{
    return 2 * static_cast<float>(index) * RATE / FFTLEN;
}

float freq2index(float freq)
{
    return 0.5 * freq * FFTLEN / RATE;
}

float mapLin2Log(float LinMin, float LinRange, float LogMin, float LogRange, float LinVal)
{
    if (LinVal < LinMin)
    {
        throw std::out_of_range("Linear value is out of range.");
    }
    return LogMin + (std::log(LinVal + 1 - LinMin) / std::log(LinRange + LinMin)) * LogRange;
}

/// Approximate HCF
float approx_hcf(float inputs[], int num_inputs, int max_iter, int accuracy_threshold)
{
    validate_size(num_inputs, 1, "At least two inputs are required to compute HCF.");

    if (num_inputs == 2)
    {
        float ratio = std::max(inputs[0], inputs[1]) / std::min(inputs[0], inputs[1]);
        return ratio > accuracy_threshold ? 0 : inputs[0] / ratio;
    }

    std::vector<float> temp_inputs(inputs, inputs + num_inputs);
    float smaller_hcf = approx_hcf(temp_inputs.data() + 1, num_inputs - 1, max_iter, accuracy_threshold);
    float hcf_inputs[2] = {temp_inputs[0], smaller_hcf};
    return approx_hcf(hcf_inputs, 2, max_iter, accuracy_threshold);
}

/// Find Largest N Elements
void Find_n_Largest(int *output, sample *input, int n_out, int n_in, bool ignore_clumped)
{
    validate_size(n_in, 0, "Input array size must be greater than zero.");
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
}

/// Pitch Detection
int pitchNumber(float freq, float *centsSharp)
{
    const double semitone = std::pow(2.0, 1.0 / 12.0);
    if (freq <= 0)
        throw std::invalid_argument("Frequency must be positive.");

    while (freq < 440)
        freq *= 2;
    while (freq > 880)
        freq /= 2;

    int pitch_num = static_cast<int>(std::round(std::log(freq / 440.0) / std::log(semitone)));
    if (pitch_num < 0)
        pitch_num = 0;
    else if (pitch_num > 11)
        pitch_num = 11;

    if (centsSharp)
    {
        *centsSharp = 1200 * std::log2(freq / (440.0 * std::pow(semitone, pitch_num)));
    }
    return pitch_num + 1;
}

int pitchName(char *name, int pitch_num)
{
    static const char *names[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    if (pitch_num < 1 || pitch_num > 12)
        throw std::out_of_range("Invalid pitch number.");
    std::string pitch = names[pitch_num - 1];
    std::copy(pitch.begin(), pitch.end(), name);
    return pitch.size();
}
