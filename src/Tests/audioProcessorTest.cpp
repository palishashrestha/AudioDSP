#include "../audioProcessor.h"
#include "../logger.h"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

// Test constants
const int QUEUE_SIZE = 1024;
const float EPSILON = 1e-2;

class AudioQueueTest : public ::testing::Test
{
protected:
    AudioQueue *queue;

    void SetUp() override
    {
        queue = new AudioQueue(QUEUE_SIZE);
    }

    void TearDown() override
    {
        delete queue;
    }
};

// Test AudioQueue Constructor
TEST_F(AudioQueueTest, Constructor_ValidSize)
{
    EXPECT_NO_THROW(AudioQueue validQueue(QUEUE_SIZE));
}

TEST_F(AudioQueueTest, Constructor_InvalidSize)
{
    EXPECT_THROW(AudioQueue invalidQueue(0), std::invalid_argument);
    EXPECT_THROW(AudioQueue invalidQueue(-1), std::invalid_argument);
}

// Test push method
TEST_F(AudioQueueTest, Push_ValidInput)
{
    sample input[QUEUE_SIZE / 2] = {1};
    EXPECT_NO_THROW(queue->push(input, QUEUE_SIZE / 2));
}

TEST_F(AudioQueueTest, Push_Overflow)
{
    sample input[QUEUE_SIZE] = {1};
    queue->push(input, QUEUE_SIZE / 2);
    EXPECT_THROW(queue->push(input, QUEUE_SIZE / 2 + 1), std::overflow_error);
}

TEST_F(AudioQueueTest, Pop_ValidOutput)
{
    AudioQueue queue(QUEUE_SIZE);
    sample input[QUEUE_SIZE / 2] = {1};
    float volume = 1.0f; // Ensure correct volume scaling
    queue.push(input, QUEUE_SIZE / 2, volume);

    sample output[QUEUE_SIZE / 2];
    EXPECT_NO_THROW(queue.pop(output, QUEUE_SIZE / 2, volume));

    for (int i = 0; i < QUEUE_SIZE / 2; i++)
    {
        EXPECT_NEAR(output[i], input[i] * volume, EPSILON);
    }
}

TEST_F(AudioQueueTest, Peek_ValidOutput)
{
    AudioQueue queue(QUEUE_SIZE);
    sample input[QUEUE_SIZE / 2] = {1};
    float volume = 1.0f; // Ensure correct volume scaling
    queue.push(input, QUEUE_SIZE / 2, volume);

    sample output[QUEUE_SIZE / 2];
    EXPECT_NO_THROW(queue.peek(output, QUEUE_SIZE / 2, volume));

    for (int i = 0; i < QUEUE_SIZE / 2; i++)
    {
        EXPECT_NEAR(output[i], input[i] * volume, EPSILON);
    }
}

// Test pop method
// TEST_F(AudioQueueTest, Pop_ValidOutput)
// {
//     sample input[QUEUE_SIZE / 2] = {1};
//     sample output[QUEUE_SIZE / 2];
//     queue->push(input, QUEUE_SIZE / 2);
//     EXPECT_NO_THROW(queue->pop(output, QUEUE_SIZE / 2));
//     for (int i = 0; i < QUEUE_SIZE / 2; ++i)
//     {
//         EXPECT_NEAR(output[i], 1.0, EPSILON);
//     }
// }

TEST_F(AudioQueueTest, Pop_Underflow)
{
    sample output[QUEUE_SIZE / 2];
    EXPECT_THROW(queue->pop(output, QUEUE_SIZE / 2), std::underflow_error);
}

// Test peek method
// TEST_F(AudioQueueTest, Peek_ValidOutput)
// {
//     sample input[QUEUE_SIZE / 2] = {1};
//     sample output[QUEUE_SIZE / 2];
//     queue->push(input, QUEUE_SIZE / 2);
//     EXPECT_NO_THROW(queue->peek(output, QUEUE_SIZE / 2));
//     for (int i = 0; i < QUEUE_SIZE / 2; ++i)
//     {
//         EXPECT_NEAR(output[i], 1.0, EPSILON);
//     }
// }

// Test FFT function
TEST(FFTTest, ValidInput)
{
    const int n = 8;
    cmplx input[n] = {1, 1, 1, 1, 0, 0, 0, 0};
    cmplx output[n];

    EXPECT_NO_THROW(fft(output, input, n));

    // Verify FFT result (example: DC component)
    EXPECT_NEAR(std::abs(output[0]), 4.0, EPSILON);
}

TEST(FFTTest, InvalidInputSize)
{
    const int n = 6; // Not a power of two
    cmplx input[n] = {1, 1, 1, 1, 1, 1};
    cmplx output[n];

    EXPECT_THROW(fft(output, input, n), std::invalid_argument);
}

// Test FindFrequencyContent
TEST(FrequencyContentTest, ValidInput)
{
    const int n = 8;
    sample input[n] = {1, 1, 1, 1, 0, 0, 0, 0};
    sample output[n];

    EXPECT_NO_THROW(FindFrequencyContent(output, input, n, true, 1.0));

    // Verify magnitude of DC component
    EXPECT_NEAR(output[0], 4.0, EPSILON);
}

TEST(FrequencyContentTest, InvalidInputSize)
{
    const int n = 6; // Not a power of two
    sample input[n] = {1, 1, 1, 1, 1, 1};
    sample output[n];

    EXPECT_THROW(FindFrequencyContent(output, input, n, true, 1.0), std::invalid_argument);
}

// Test logging functionality
TEST(LoggerIntegrationTest, Logging)
{
    Logger &logger = Logger::getInstance();
    EXPECT_NO_THROW(logger.log("FFT computation started", "INFO"));
}
