#include "chordDictionary.h"
#include "helper.h"
#include "logger.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

/**
 * @brief Static definitions for chords in root position.
 *
 * This array contains predefined chords with their notes and names.
 */
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

/**
 * @brief Array containing all transposed chords.
 *
 * This array is populated during the initialization of the chord dictionary.
 */
static chord all_chords[NUM_CHORD_TYPES * 12];

/**
 * @brief Checks if a chord contains all input notes.
 *
 * @param notes_in Array of input notes.
 * @param num_notes_in Number of input notes.
 * @return true if the chord contains all the input notes, false otherwise.
 */
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

/**
 * @brief Transposes a chord by a specified number of semitones.
 *
 * @param old_chord The chord to be transposed.
 * @param semitones_up The number of semitones to transpose.
 * @return The transposed chord.
 * @throws std::invalid_argument if semitones_up is outside the range [-11, 11].
 */
chord transpose_chord(chord old_chord, int semitones_up)
{
    if (semitones_up < -11 || semitones_up > 11)
    {
        logMessage("Invalid transpose semitones: " + std::to_string(semitones_up), "ERROR");
        throw std::invalid_argument("Transpose semitones must be between -11 and 11.");
    }

    chord new_chord = old_chord;

    for (int i = 0; i < old_chord.num_notes; ++i)
    {
        new_chord.notes[i] = ((old_chord.notes[i] + semitones_up - 1) % 12 + 12) % 12 + 1;
    }

    pitchName(new_chord.name, new_chord.notes[0], true); // Update chord name

    // Log original and transposed notes
    std::string original_notes = "[";
    for (int i = 0; i < old_chord.num_notes; ++i)
    {
        original_notes += std::to_string(old_chord.notes[i]) + (i < old_chord.num_notes - 1 ? ", " : "]");
    }

    std::string transposed_notes = "[";
    for (int i = 0; i < new_chord.num_notes; ++i)
    {
        transposed_notes += std::to_string(new_chord.notes[i]) + (i < new_chord.num_notes - 1 ? ", " : "]");
    }

    logMessage("Transposed chord from " + original_notes + " to " + transposed_notes + " by " + std::to_string(semitones_up) + " semitones.", "INFO");
    return new_chord;
}

/**
 * @brief Initializes the chord dictionary by populating all transpositions.
 */
void initialize_chord_dictionary()
{
    if (chord_dictionary_initialized)
    {
        logMessage("Chord dictionary already initialized.", "INFO");
        return;
    }

    logMessage("Initializing chord dictionary.", "INFO");
    for (int i = 0; i < NUM_CHORD_TYPES; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            all_chords[i * 12 + j] = transpose_chord(A_root_chords[i], j);
        }
    }
    chord_dictionary_initialized = true;
    logMessage("Chord dictionary initialized successfully.", "INFO");
}

/**
 * @brief Identifies the chord that matches the input notes.
 *
 * @param name_out Output buffer to store the name of the identified chord.
 * @param notes Array of input notes.
 * @param num_notes Number of input notes.
 * @return The length of the identified chord name, or 0 if no match is found.
 * @throws std::invalid_argument if name_out is null or num_notes is less than or equal to 0.
 * @throws std::runtime_error if the best matching chord has an invalid name.
 */
int identify_chord(char *name_out, int notes[], int num_notes)
{
    if (name_out == nullptr)
    {
        logMessage("Output buffer is null.", "ERROR");
        throw std::invalid_argument("Output buffer cannot be null.");
    }

    if (!chord_dictionary_initialized)
    {
        logMessage("Chord dictionary not initialized. Initializing now.", "WARNING");
        initialize_chord_dictionary();
    }

    if (num_notes <= 0)
    {
        logMessage("Invalid number of input notes: " + std::to_string(num_notes), "ERROR");
        throw std::invalid_argument("Number of input notes must be greater than zero.");
    }

    logMessage("Identifying chord for input notes.", "INFO");

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
    {
        logMessage("No matching chord found for input notes.", "WARNING");
        return 0; // No matching chord found
    }

    logMessage("Number of candidates found: " + std::to_string(num_candidates), "INFO");

    // Filter candidates to find the best match
    int best_candidate = -1;
    int min_size = INT_MAX;

    for (int i = 0; i < num_candidates; i++)
    {
        int candidate_index = candidates[i];
        if (all_chords[candidate_index].num_notes < min_size ||
            (all_chords[candidate_index].num_notes == min_size && all_chords[candidate_index].notes[0] == notes[0]))
        {
            best_candidate = candidate_index;
            min_size = all_chords[candidate_index].num_notes;
        }
    }

    if (best_candidate == -1)
    {
        logMessage("No valid candidate found after filtering.", "WARNING");
        return 0; // No valid candidate found
    }

    if (all_chords[best_candidate].name[0] == '\0')
    {
        logMessage("Chord name is empty for the best candidate.", "ERROR");
        throw std::runtime_error("Invalid chord name for the best candidate.");
    }

    // Write the chord name to name_out
    int name_length = 0;
    while (all_chords[best_candidate].name[name_length] != '\0')
    {
        name_out[name_length] = all_chords[best_candidate].name[name_length];
        name_length++;
    }
    name_out[name_length] = '\0';

    logMessage("Identified chord: " + std::string(name_out), "INFO");
    return name_length;
}
