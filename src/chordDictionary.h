#ifndef CHORD_DICTIONARY_H
#define CHORD_DICTIONARY_H

#include <vector>
#include <string>

#define CHORD_NAME_SIZE 15
#define CHORD_MAX_NOTES 5
#define NUM_CHORD_TYPES 11

/// Represents a chord with notes, number of notes, and its name.
struct chord
{
    int num_notes;
    int notes[CHORD_MAX_NOTES]; // Root position
    char name[CHORD_NAME_SIZE];

    /// Checks if a chord contains all input notes
    bool contains(int notes_in[], int num_notes_in);
};

/// Chord Transposer
chord transpose_chord(chord old_chord, int semitones_up);

static bool chord_dictionary_initialized = false;
/// Initialize chord dictionary
void initialize_chord_dictionary();

/// Determine the chord based on input notes
int identify_chord(char *name_out, int notes[], int num_notes);

#endif // CHORD_DICTIONARY_H
