#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <stdexcept>
#include "audioProcessor.h"

typedef short sample;

/// Function declarations
void show_bargraph(int bars[], int n_bars, bool logOnce, int height = 50, int hScale = 1, float vScale = 1, char symbol = '|');
float index2freq(int index, bool logOnce);
float freq2index(float freq, bool logOnce);
float mapLin2Log(float LinMin, float LinRange, float LogMin, float LogRange, float LinVal, bool logOnce);
float approx_hcf(float inputs[], int num_inputs, bool logOnce, int max_iter = 5, int accuracy_threshold = 10);
void Find_n_Largest(int *output, sample *input, int n_out, int n_in, bool logOnce, bool ignore_clumped = true);
int pitchNumber(float freq, bool logOnce, float *centsSharp = nullptr);
int pitchName(char *name, int pitch_num, bool logOnce);

#endif
