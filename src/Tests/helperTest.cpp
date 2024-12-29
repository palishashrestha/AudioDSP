#include "../helper.h"
#include "../logger.h"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include <stdexcept>

/// Test show_bargraph function
TEST(HelperTest, ShowBarGraph)
{
    int bars[] = {5, 10, 15};
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(show_bargraph(bars, 3, true, 10, 1, 1.0, '#'));
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_FALSE(output.empty()); // Ensure graph is printed
}

TEST(HelperTest, IndexToFreq)
{
    int index = 5;
    float freq = index2freq(index, false);
    EXPECT_NEAR(freq, 2 * 5 * 44100.0 / 65536.0, 1e-3) << "Frequency calculation is incorrect.";
}

// Test for an array containing only one input
TEST(HelperTest, ApproximateHCF_SingleInput)
{
    float inputs[] = {15.0f};
    int num_inputs = 1;

    try
    {
        approx_hcf(inputs, num_inputs, true, 100, 1e-3);
        FAIL() << "Expected std::invalid_argument for single input.";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_STREQ("At least two inputs are required to compute HCF.", e.what());
    }
    catch (...)
    {
        FAIL() << "Expected std::invalid_argument, but got a different exception.";
    }
}

// Test for invalid inputs that exceed accuracy threshold
TEST(HelperTest, ApproximateHCF_ExceedsAccuracyThreshold)
{
    float inputs[] = {15.0f, 10.0f, 4.5f};
    int num_inputs = 3;

    float hcf = approx_hcf(inputs, num_inputs, true, 100, 1.0);
    ASSERT_EQ(hcf, 0) << "HCF should be 0 for inputs exceeding the accuracy threshold.";
}

/// Test freq2index function
TEST(HelperTest, FreqToIndex)
{
    float index = freq2index(440.0, true);
    ASSERT_FLOAT_EQ(index, 0.5 * 440.0 * FFTLEN / RATE);
}

/// Test mapLin2Log function
TEST(HelperTest, MapLinToLog)
{
    ASSERT_NO_THROW(mapLin2Log(0, 100, 1, 10, 50, true));
    ASSERT_THROW(mapLin2Log(0, 100, 1, 10, -10, true), std::out_of_range);
}

/// Test Find_n_Largest function
TEST(HelperTest, FindNLargest)
{
    sample input[] = {5, 2, 9, 1, 7};
    int output[3];
    Find_n_Largest(output, input, 3, 5, true, false);

    std::vector<int> expected = {2, 4, 0}; // Indices of largest elements
    ASSERT_EQ(std::vector<int>(output, output + 3), expected);
}

/// Test pitchNumber function
TEST(HelperTest, PitchNumber)
{
    float centsSharp = 0;
    int pitchNum = pitchNumber(440.0, true, &centsSharp);
    ASSERT_EQ(pitchNum, 1); // A4 (440 Hz)
    ASSERT_NEAR(centsSharp, 0, 1e-2);
}

/// Test pitchName function
TEST(HelperTest, PitchName)
{
    char name[3];
    int length = pitchName(name, 1, true);
    ASSERT_EQ(std::string(name, length), "A");
}
