#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <stdexcept>
#include "audioProcessor.h"

typedef short sample;

/// Function declarations
void show_bargraph(int bars[], int n_bars, int height = 50, int hScale = 1, float vScale = 1, char symbol = '|');
float index2freq(int index);
float freq2index(float freq);
float mapLin2Log(float LinMin, float LinRange, float LogMin, float LogRange, float LinVal);
float approx_hcf(float inputs[], int num_inputs, int max_iter = 5, int accuracy_threshold = 10);
void Find_n_Largest(int *output, sample *input, int n_out, int n_in, bool ignore_clumped = true);
int pitchNumber(float freq, float *centsSharp = nullptr);
int pitchName(char *name, int pitch_num);

#endif // HELPER_H
