#include "../chordDictionary.h"
#include "gtest/gtest.h"
#include <stdexcept>

// Test initialize_chord_dictionary
TEST(ChordDictionaryTest, InitializeChordDictionary)
{
    initialize_chord_dictionary();
    EXPECT_NO_THROW(initialize_chord_dictionary());
}

// Test contains
TEST(ChordDictionaryTest, ContainsNotes)
{
    chord A_major = {3, {1, 5, 8}, "A Maj"};
    int notes1[] = {1, 5};
    EXPECT_TRUE(A_major.contains(notes1, 2));

    int notes2[] = {1, 6};
    EXPECT_FALSE(A_major.contains(notes2, 2));
}

// Test transpose_chord function
TEST(ChordDictionaryTest, TransposeChord)
{
    chord A_major = {3, {1, 5, 8}, "A Maj"};

    // Test transposition by 2 semitones
    chord transposed = transpose_chord(A_major, 2);
    EXPECT_EQ(transposed.notes[0], 3);  // C
    EXPECT_EQ(transposed.notes[1], 7);  // E
    EXPECT_EQ(transposed.notes[2], 10); // G#
    EXPECT_STREQ(transposed.name, "B Maj");

    // Test no transposition
    transposed = transpose_chord(A_major, 0);
    EXPECT_EQ(transposed.notes[0], 1);
    EXPECT_EQ(transposed.notes[1], 5);
    EXPECT_EQ(transposed.notes[2], 8);
    EXPECT_STREQ(transposed.name, "A Maj");

    // Test edge case: Transpose by -11 semitones
    transposed = transpose_chord(A_major, -11);
    EXPECT_EQ(transposed.notes[0], 2);
    EXPECT_EQ(transposed.notes[1], 6);
    EXPECT_EQ(transposed.notes[2], 9);
    EXPECT_STREQ(transposed.name, "A#Maj");

    // Test invalid transposition
    EXPECT_THROW(transpose_chord(A_major, 12), std::invalid_argument);
    EXPECT_THROW(transpose_chord(A_major, -12), std::invalid_argument);
}

// Test identify_chord function
TEST(ChordDictionaryTest, IdentifyChord)
{
    char chord_name[20];
    int notes1[] = {1, 5, 8};
    int notes2[] = {1, 5, 8, 12};
    int notes_no_match[] = {2, 4, 7};

    // Initialize the chord dictionary
    initialize_chord_dictionary();

    // Test identifying a basic chord
    int len = identify_chord(chord_name, notes1, 3);
    EXPECT_STREQ(chord_name, "A Maj");
    EXPECT_EQ(len, 5); // Length of "A Maj"

    // Test identifying a more complex chord
    len = identify_chord(chord_name, notes2, 4);
    EXPECT_STREQ(chord_name, "A Maj7");
    EXPECT_EQ(len, 6); // Length of "A Maj7"
}

// Test edge cases
TEST(ChordDictionaryTest, EdgeCases)
{
    chord A_major = {3, {1, 5, 8}, "A Maj"};
    chord transposed = transpose_chord(A_major, 0);
    EXPECT_EQ(transposed.notes[0], 1);
    EXPECT_EQ(transposed.notes[1], 5);
    EXPECT_EQ(transposed.notes[2], 8);
}
