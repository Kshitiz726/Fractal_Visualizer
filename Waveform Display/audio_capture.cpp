#include <cmath>
#include <vector>

// Constants
const int SAMPLE_RATE = 44100;
const int BUFFER_SIZE = 44100; // 1 second buffer size

// Function to generate waveform data based on frequency
std::vector<float> generateWaveformData(float frequency) {
    std::vector<float> waveformData(BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        float t = static_cast<float>(i) / SAMPLE_RATE; // Time in seconds
        waveformData[i] = 0.5f * std::sin(2.0f * 3.14 * frequency * t); // Sine wave
    }
    return waveformData;
}
