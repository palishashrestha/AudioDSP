#include "chordDictionary.h"
#include "helper.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

/// Static chord definitions in root position
static chord A_root_chords[] = {
    {4, {1, 5, 8, 3}, "A add9"},
    {4, {1, 5, 8, 12}, "A Maj7"},
    {4, {1, 4, 8, 11}, "A min7"},
    {4, {1, 5, 8, 11}, "A dom7"},
    {4, {1, 4, 8, 12}, "A minMaj7"},
    {3, {1, 4, 7}, "A dim"},
    {3, {1, 6, 8}, "A sus4"},
    {3, {1, 3, 8}, "A sus2"},
    {3, {1, 5, 8}, "A Maj"},
    {3, {1, 4, 8}, "A min"},
    {2, {1, 8}, "A 5"}};

static chord all_chords[NUM_CHORD_TYPES * 12];

/// Check if a chord contains all the input notes
bool chord::contains(int notes_in[], int num_notes_in)
{
    for (int i = 0; i < num_notes_in; i++)
    {
        bool found = false;
        for (int j = 0; j < num_notes; j++)
        {
            if (notes[j] == notes_in[i])
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

/// Transpose a chord up by semitones
chord transpose_chord(chord old_chord, int semitones_up)
{
    if (semitones_up < -11 || semitones_up > 11)
        throw std::invalid_argument("Transpose semitones must be between -11 and 11.");

    chord new_chord = old_chord;

    for (int i = 0; i < old_chord.num_notes; ++i)
        new_chord.notes[i] = (old_chord.notes[i] + semitones_up - 1) % 12 + 1;

    pitchName(new_chord.name, new_chord.notes[0], true); // Update chord name
    return new_chord;
}

/// Initialize all chord transpositions
void initialize_chord_dictionary()
{
    if (chord_dictionary_initialized)
        return;

    for (int i = 0; i < NUM_CHORD_TYPES; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            all_chords[i * 12 + j] = transpose_chord(A_root_chords[i], j);
        }
    }
    chord_dictionary_initialized = true;
}

/// Identify the chord that matches the input notes
int identify_chord(char *name_out, int notes[], int num_notes)
{
    if (!chord_dictionary_initialized)
        initialize_chord_dictionary();

    if (num_notes <= 0)
        throw std::invalid_argument("Number of input notes must be greater than zero.");

    int candidates[NUM_CHORD_TYPES * 12];
    int num_candidates = 0;

    // Find all chords that contain the input notes
    for (int i = 0; i < NUM_CHORD_TYPES * 12; i++)
    {
        if (all_chords[i].contains(notes, num_notes))
        {
            candidates[num_candidates++] = i;
        }
    }

    if (num_candidates == 0)
        return 0; // No matching chord found

    // Filter candidates: Find smallest chord (minimum num_notes)
    int min_size = all_chords[candidates[0]].num_notes;
    for (int i = 1; i < num_candidates; i++)
    {
        if (all_chords[candidates[i]].num_notes < min_size)
            min_size = all_chords[candidates[i]].num_notes;
    }

    int best_candidate = -1;
    for (int i = 0; i < num_candidates; i++)
    {
        if (all_chords[candidates[i]].num_notes == min_size)
        {
            if (best_candidate == -1 || all_chords[candidates[i]].notes[0] == notes[0])
            {
                best_candidate = candidates[i];
            }
        }
    }

    // Write the chord name to name_out
    int name_length = 0;
    while (all_chords[best_candidate].name[name_length] != '\0')
    {
        name_out[name_length] = all_chords[best_candidate].name[name_length];
        name_length++;
    }
    name_out[name_length] = '\0';

    return name_length;
}
