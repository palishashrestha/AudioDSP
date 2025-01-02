#include "helper.h"
#include "logger.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

/**
 * @brief Validates the size of an input against a threshold.
 *
 * If the size is less than or equal to the threshold, logs an error message and throws an exception.
 * Otherwise, logs a success message.
 *
 * @param size The size to validate.
 * @param threshold The minimum acceptable size.
 * @param message The error message to log and throw if validation fails.
 * @param logOnce Whether to log the message only once.
 */
void validate_size(int size, int threshold, const std::string &message, bool logOnce)
{
    if (size <= threshold)
    {
        logMessage("Validation failed: " + message, "ERROR", logOnce);
        throw std::invalid_argument(message);
    }
    logMessage("Validation passed for size: " + std::to_string(size), "INFO", logOnce);
}

/**
 * @brief Prints a line of symbols to the console.
 *
 * @param length The length of the line.
 * @param symbol The symbol to repeat.
 * @param logOnce Whether to log the message only once.
 */
void print_line(int length, char symbol, bool logOnce)
{
    logMessage("Printing line of length: " + std::to_string(length) + " with symbol: " + std::string(1, symbol), "INFO", logOnce);
    for (int j = 0; j < length; j++)
    {
        std::cout << symbol;
    }
    std::cout << '\n';
}

/**
 * @brief Generates and displays a bar graph.
 *
 * @param bars An array of bar heights.
 * @param n_bars The number of bars.
 * @param logOnce Whether to log the message only once.
 * @param height The maximum height of the bar graph.
 * @param hScale The horizontal scaling factor.
 * @param vScale The vertical scaling factor.
 * @param symbol The symbol to use for the bars.
 */
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

/**
 * @brief Converts an index to a frequency value.
 *
 * @param index The index to convert.
 * @param logOnce Whether to log the message only once.
 * @return The corresponding frequency.
 */
float index2freq(int index, bool logOnce)
{
    float freq = 2 * static_cast<float>(index) * RATE / FFTLEN;
    logMessage("Converted index " + std::to_string(index) + " to frequency: " + std::to_string(freq), "INFO", logOnce);
    return freq;
}

/**
 * @brief Converts a frequency value to an index.
 *
 * @param freq The frequency to convert.
 * @param logOnce Whether to log the message only once.
 * @return The corresponding index.
 */
float freq2index(float freq, bool logOnce)
{
    float index = 0.5 * freq * FFTLEN / RATE;
    logMessage("Converted frequency " + std::to_string(freq) + " to index: " + std::to_string(index), "INFO", logOnce);
    return index;
}

/**
 * @brief Maps a linear value to a logarithmic scale.
 *
 * @param LinMin The minimum linear value.
 * @param LinRange The range of linear values.
 * @param LogMin The minimum logarithmic value.
 * @param LogRange The range of logarithmic values.
 * @param LinVal The linear value to map.
 * @param logOnce Whether to log the message only once.
 * @return The mapped logarithmic value.
 */
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

/**
 * @brief Computes an approximate highest common factor (HCF) for an array of inputs.
 *
 * @param inputs An array of input values.
 * @param num_inputs The number of inputs.
 * @param logOnce Whether to log the message only once.
 * @param max_iter The maximum number of iterations for approximation.
 * @param accuracy_threshold The accuracy threshold for computation.
 * @return The approximate HCF.
 */
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

/**
 * @brief Finds the largest N elements from an array.
 *
 * @param output The array to store the indices of the largest elements.
 * @param input The input array of elements.
 * @param n_out The number of largest elements to find.
 * @param n_in The size of the input array.
 * @param logOnce Whether to log the message only once.
 * @param ignore_clumped Whether to ignore consecutive elements.
 */
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

/**
 * @brief Computes the pitch number for a given frequency.
 *
 * @param freq The frequency to analyze.
 * @param logOnce Whether to log the message only once.
 * @param centsSharp Pointer to store the cents deviation.
 * @return The pitch number corresponding to the frequency.
 */
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

/**
 * @brief Retrieves the pitch name corresponding to a pitch number.
 *
 * @param name The character array to store the pitch name.
 * @param pitch_num The pitch number (1-12).
 * @param logOnce Whether to log the message only once.
 * @return The length of the pitch name.
 */
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
