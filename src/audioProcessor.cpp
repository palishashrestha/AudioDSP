#include "audioProcessor.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AudioQueue::AudioQueue(int QueueLength) : len(QueueLength), inpos(0), outpos(0)
{
    if (QueueLength <= 0)
    {
        throw std::invalid_argument("Queue length must be greater than zero.");
    }
    audio = new sample[len];
}

AudioQueue::~AudioQueue()
{
    delete[] audio;
}

void AudioQueue::validate_space(int n_samples) const
{
    if (!space_available(n_samples))
    {
        throw std::overflow_error("Audio queue overflow: insufficient space available.");
    }
}

void AudioQueue::validate_data(int n_samples) const
{
    if (!data_available(n_samples))
    {
        throw std::underflow_error("Audio queue underflow: insufficient data available.");
    }
}

bool AudioQueue::data_available(int n_samples) const
{
    if (inpos >= outpos)
    {
        return (inpos - outpos) >= n_samples;
    }
    else
    {
        return (inpos + len - outpos) >= n_samples;
    }
}

bool AudioQueue::space_available(int n_samples) const
{
    if (inpos >= outpos)
    {
        return (outpos + len - inpos) > n_samples;
    }
    else
    {
        return (outpos - inpos) > n_samples;
    }
}

void AudioQueue::push(const sample *input, int n_samples, float volume)
{
    validate_space(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        audio[(inpos + i) % len] = input[i] * volume;
    }
    inpos = (inpos + n_samples) % len;
}

void AudioQueue::pop(sample *output, int n_samples, float volume)
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[i] = audio[(outpos + i) % len] * volume;
    }
    outpos = (outpos + n_samples) % len;
}

void AudioQueue::peek(sample *output, int n_samples, float volume) const
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[i] = audio[(outpos + i) % len] * volume;
    }
}

void AudioQueue::peekFreshData(sample *output, int n_samples, float volume) const
{
    validate_data(n_samples);
    for (int i = 0; i < n_samples; i++)
    {
        output[n_samples - i - 1] = audio[(len + inpos - i - 1) % len] * volume;
    }
}

void dftmag(sample *output, const sample *input, int n)
{
    if (n <= 0)
    {
        throw std::invalid_argument("Input size for DFT must be greater than zero.");
    }

    std::vector<float> sinArr(n), cosArr(n);
    for (int i = 0; i < n; i++)
    {
        sinArr[i] = sin(i * 2 * M_PI / n);
        cosArr[i] = cos(i * 2 * M_PI / n);
    }

    for (int i = 0; i < n; i++)
    {
        double real = 0, imag = 0;
        for (int j = 0; j < n; j++)
        {
            real += input[j] * cosArr[(i * j) % n];
            imag += input[j] * sinArr[(i * j) % n];
        }
        output[i] = static_cast<sample>(sqrt(real * real + imag * imag));
    }
}

void fft(cmplx *output, const cmplx *input, int n)
{
    if (n <= 0 || (n & (n - 1)) != 0)
    {
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

void FindFrequencyContent(sample *output, const sample *input, int n, float vScale)
{
    if (n <= 0 || (n & (n - 1)) != 0)
    {
        throw std::invalid_argument("Input size for FFT must be a power of two and greater than zero.");
    }

    std::vector<cmplx> fftin(n), fftout(n);
    for (int i = 0; i < n; i++)
    {
        fftin[i] = static_cast<cmplx>(input[i]);
    }

    fft(fftout.data(), fftin.data(), n);

    for (int i = 0; i < n; i++)
    {
        double magnitude = abs(fftout[i]) * vScale;
        output[i] = static_cast<sample>(std::min(magnitude, static_cast<double>(MAX_SAMPLE_VALUE)));
    }
}
