#include "audioProcessor.h"
#include "logger.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Constructs an AudioQueue instance with a specified queue length.
 *
 * @param QueueLength The length of the audio queue.
 * @throws std::invalid_argument if QueueLength is less than or equal to zero.
 */
AudioQueue::AudioQueue(int QueueLength) : len(QueueLength), inpos(0), outpos(0)
{
    if (QueueLength <= 0)
    {
        logMessage("Queue length must be greater than zero.", "ERROR");
        throw std::invalid_argument("Queue length must be greater than zero.");
    }
    audio = new sample[len];
    logMessage("AudioQueue created with length: " + std::to_string(QueueLength), "INFO");
}

/**
 * @brief Destructor for AudioQueue.
 *
 * Frees the allocated audio buffer.
 */
AudioQueue::~AudioQueue()
{
    delete[] audio;
    logMessage("AudioQueue destroyed.", "INFO");
}

/**
 * @brief Validates if there is enough space available in the queue to push samples.
 *
 * @param n_samples Number of samples to validate.
 * @throws std::overflow_error if there is insufficient space available.
 */
void AudioQueue::validate_space(int n_samples) const
{
    if (!space_available(n_samples))
    {
        logMessage("Audio queue overflow: insufficient space available.", "ERROR");
        throw std::overflow_error("Audio queue overflow: insufficient space available.");
    }
}

/**
 * @brief Validates if there is enough data available in the queue to pop samples.
 *
 * @param n_samples Number of samples to validate.
 * @throws std::underflow_error if there is insufficient data available.
 */
void AudioQueue::validate_data(int n_samples) const
{
    if (!data_available(n_samples))
    {
        logMessage("Audio queue underflow: insufficient data available.", "ERROR");
        throw std::underflow_error("Audio queue underflow: insufficient data available.");
    }
}

/**
 * @brief Checks if there is enough data available in the queue.
 *
 * @param n_samples Number of samples to check.
 * @return true if sufficient data is available, false otherwise.
 */
bool AudioQueue::data_available(int n_samples) const
{
    return (inpos >= outpos) ? (inpos - outpos) >= n_samples : (inpos + len - outpos) >= n_samples;
}

/**
 * @brief Checks if there is enough space available in the queue.
 *
 * @param n_samples Number of samples to check.
 * @return true if sufficient space is available, false otherwise.
 */
bool AudioQueue::space_available(int n_samples) const
{
    return (inpos >= outpos) ? (outpos + len - inpos) > n_samples : (outpos - inpos) > n_samples;
}

/**
 * @brief Pushes audio samples into the queue.
 *
 * @param input Array of input samples.
 * @param n_samples Number of samples to push.
 * @param volume Volume multiplier to apply to the input samples.
 */
void AudioQueue::push(const sample *input, int n_samples, float volume)
{
    validate_space(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        audio[(inpos + i) % len] = input[i] * volume;
    }
    inpos = (inpos + n_samples) % len;
}

/**
 * @brief Pops audio samples from the queue.
 *
 * @param output Array to store the output samples.
 * @param n_samples Number of samples to pop.
 * @param volume Volume multiplier to apply to the output samples.
 */
void AudioQueue::pop(sample *output, int n_samples, float volume)
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[i] = audio[(outpos + i) % len] * volume;
    }
    outpos = (outpos + n_samples) % len;
}

/**
 * @brief Peeks at audio samples from the queue without removing them.
 *
 * @param output Array to store the output samples.
 * @param n_samples Number of samples to peek.
 * @param volume Volume multiplier to apply to the output samples.
 */
void AudioQueue::peek(sample *output, int n_samples, float volume) const
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[i] = audio[(outpos + i) % len] * volume;
    }
}

/**
 * @brief Peeks at the most recent audio samples from the queue without removing them.
 *
 * @param output Array to store the output samples.
 * @param n_samples Number of samples to peek.
 * @param volume Volume multiplier to apply to the output samples.
 */
void AudioQueue::peekFreshData(sample *output, int n_samples, float volume) const
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[n_samples - i - 1] = audio[(len + inpos - i - 1) % len] * volume;
    }
}

/**
 * @brief Computes the Fast Fourier Transform (FFT) for a given input.
 *
 * @param output Array to store the FFT result.
 * @param input Array of complex input samples.
 * @param n Number of samples, must be a power of two.
 * @throws std::invalid_argument if n is not a power of two or is less than or equal to zero.
 */
void fft(cmplx *output, const cmplx *input, int n)
{
    static bool logOnce = true; // Ensure single logging for the entire FFT computation
    if (logOnce)
    {
        logMessage("Starting FFT computation for " + std::to_string(n) + " samples.", "INFO", logOnce);
        logOnce = false;
    }

    if (n <= 0 || (n & (n - 1)) != 0)
    {
        logMessage("Input size for FFT must be a power of two and greater than zero.", "ERROR");
        throw std::invalid_argument("Input size for FFT must be a power of two and greater than zero.");
    }

    if (n == 1)
    {
        output[0] = input[0];
        return;
    }

    std::vector<cmplx> even(n / 2), odd(n / 2);
    for (int i = 0; i < n / 2; i++)
    {
        even[i] = input[2 * i];
        odd[i] = input[2 * i + 1];
    }

    std::vector<cmplx> evenOut(n / 2), oddOut(n / 2);
    fft(evenOut.data(), even.data(), n / 2);
    fft(oddOut.data(), odd.data(), n / 2);

    for (int i = 0; i < n / 2; i++)
    {
        cmplx t = std::polar(1.0, -2 * M_PI * i / n) * oddOut[i];
        output[i] = evenOut[i] + t;
        output[i + n / 2] = evenOut[i] - t;
    }
}

/**
 * @brief Computes the frequency content of an input signal using FFT.
 *
 * @param output Array to store the computed frequency magnitudes.
 * @param input Array of input samples.
 * @param n Number of samples, must be a power of two.
 * @param logOnce Whether to log this computation only once.
 * @param vScale Scale factor for the output magnitudes.
 * @throws std::invalid_argument if n is not a power of two or is less than or equal to zero.
 */
void FindFrequencyContent(sample *output, const sample *input, int n, bool logOnce, float vScale)
{
    if (n <= 0 || (n & (n - 1)) != 0)
    {
        logMessage("Input size for FFT must be a power of two and greater than zero.", "ERROR");
        throw std::invalid_argument("Input size for FFT must be a power of two and greater than zero.");
    }
    logMessage("Starting Frequency Content computation for " + std::to_string(n) + " samples.", "INFO", logOnce);

    std::vector<cmplx> fftin(n), fftout(n);
    for (int i = 0; i < n; i++)
    {
        fftin[i] = static_cast<cmplx>(input[i]);
    }

    fft(fftout.data(), fftin.data(), n); // Call FFT recursively

    for (int i = 0; i < n; i++)
    {
        double magnitude = abs(fftout[i]) * vScale;
        output[i] = static_cast<sample>(std::min(magnitude, static_cast<double>(MAX_SAMPLE_VALUE)));
    }
    logMessage("Frequency Content computation completed for " + std::to_string(n) + " samples.", "INFO", logOnce);
}
