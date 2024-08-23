#include "Waveform_display.h"
#include "audio_capture.h"
#include <imgui.h>
#include <vector>

void displayWaveform(float frequency) {
    
    ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Once); 
    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once); 

    ImGui::Begin("Waveform Visualizer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    
    std::vector<float> waveformData = generateWaveformData(frequency);

    
    if (!waveformData.empty()) {
        ImGui::PlotLines("Waveform", waveformData.data(), waveformData.size(), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 300)); // Adjust the plot size if needed
    }

    ImGui::End();
}
