#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <vector>

// Function to initialize the audio system (if needed)
bool initializeAudioCapture();

// Function to generate waveform data based on a frequency
std::vector<float> generateWaveformData(float frequency);

#endif // AUDIO_CAPTURE_H
