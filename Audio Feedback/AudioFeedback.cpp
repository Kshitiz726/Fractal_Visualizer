#include <iostream>
#include <SDL.h>
#include <cmath>

void playFractalSound(int iterationCount, float frequency) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_AudioSpec wavSpec;
    SDL_zero(wavSpec);
    wavSpec.freq = 44100;
    wavSpec.format = AUDIO_S16SYS;
    wavSpec.channels = 1;
    wavSpec.samples = 2048;
    wavSpec.callback = NULL;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if (deviceId == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    const int duration = 200; // Duration of the sound in milliseconds
    const int sampleRate = 44100;
    int length = (sampleRate * duration) / 1000;

    // Generate audible sound based on iteration count and frequency
    int16_t* buffer = new int16_t[length];

    // Create a sine wave with a sharp attack and exponential decay envelope
    const float initialAmplitude = 30000.0f;
    const float attackDuration = 0.02f; // Attack duration in seconds
    const float decayFactor = 0.01f;    // Exponential decay factor

    for (int i = 0; i < length; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float envelope = 1.0f;

        // Apply sharp attack
        if (t < attackDuration) {
            envelope *= t / attackDuration;
        }

        // Apply exponential decay
        envelope *= exp(-decayFactor * t);

        // Generate sine wave
        float wave = initialAmplitude * envelope * sinf(2.0f * M_PI * frequency * t);

        buffer[i] = static_cast<int16_t>(wave);
    }

    SDL_QueueAudio(deviceId, buffer, length * sizeof(int16_t));
    SDL_PauseAudioDevice(deviceId, 0);

    SDL_Delay(duration); // Wait for the sound to finish

    SDL_CloseAudioDevice(deviceId);
    delete[] buffer;
    SDL_Quit();
}