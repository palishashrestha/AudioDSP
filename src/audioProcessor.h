
#ifndef AUDIODSP_H
#define AUDIODSP_H

#include <iostream>
#include <math.h>
#include <complex>
#include <stdexcept> // For exception handling

typedef short sample;               /// Datatype of samples. Also used to store frequency coefficients.
typedef std::complex<double> cmplx; /// Complex number datatype for FFT

#define MAX_SAMPLE_VALUE 32767 /// Max sample value based on sample datatype
#define RATE 44100             /// Sample rate
#define CHUNK 64               /// Buffer size
#define CHANNELS 1             /// Mono audio
#define FFTLEN 65536           /// Number of samples to perform FFT on. Must be power of 2.

/**
 * ------------------------
 * ----class AudioQueue----
 * ------------------------
 * Handles audio data buffering for recording/playback, preventing threading issues
 * such as skipping, repeating samples, or incorrect buffer sizes.
 */
class AudioQueue
{
private:
  int len;       /// Maximum length of queue
  sample *audio; /// Pointer to audio data array
  int inpos;     /// Index in audio[] of back of queue
  int outpos;    /// Index in audio[] of front of queue

  void validate_space(int n_samples) const; /// Ensures there is enough space for pushing samples
  void validate_data(int n_samples) const;  /// Ensures there is enough data for popping/peeking

public:
  AudioQueue(int QueueLength = 10000); /// Constructor. Takes maximum length.
  ~AudioQueue();                       /// Destructor.

  bool data_available(int n_samples = 1) const;  /// Check if the queue has n_samples of data.
  bool space_available(int n_samples = 1) const; /// Check if the queue has space for n_samples.

  void push(const sample *input, int n_samples, float volume = 1);           /// Push n_samples to the queue.
  void pop(sample *output, int n_samples, float volume = 1);                 /// Pop n_samples from the queue.
  void peek(sample *output, int n_samples, float volume = 1) const;          /// Peek at n_samples to be popped.
  void peekFreshData(sample *output, int n_samples, float volume = 1) const; /// Peek freshest n_samples.
};

/**
 * fft()
 * Performs a Fast Fourier Transform (FFT) using the Cooley-Tukey algorithm.
 * @param output: Array to store the FFT result.
 * @param input: Input complex array.
 * @param n: Size of input/output arrays (must be a power of 2; throws exception otherwise).
 */
void fft(cmplx *output, const cmplx *input, int n);

/**
 * FindFrequencyContent()
 * Calculates the magnitude of frequency components using FFT.
 * @param output: Array to store magnitude values.
 * @param input: Input audio samples.
 * @param n: Number of samples (must be a power of 2).
 * @param vScale: Volume scaling factor (default = 0.005).
 * @throws std::invalid_argument if n is not a power of 2.
 */
void FindFrequencyContent(sample *output, const sample *input, int n, bool logOnce, float vScale = 0.005);

#endif // AUDIODSP_H
