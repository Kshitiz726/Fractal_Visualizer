#include <iostream>
#include <vector>
#include <cmath>
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <imgui.h>
#include "C:\Users\Kshitiz\source\repos\Fractal app\Audio Feedback\AudioFeedback.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "C:\Users\Kshitiz\source\repos\Fractal app\include\stb_image.h"
#include <Waveform_display.h>
#include <audio_capture.h>
#include <portaudio.h>
const int width = 1800;
const int height = 600;
bool displayWaveformFlag = true;
float frequency = 0.0f;
// Shader sources
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 coord;

    void main() {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        coord = aPos * 0.5 + 0.5; // Transform coordinates to range [0, 1]
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec2 coord;
    out vec4 FragColor;

    uniform vec2 offset;
    uniform float zoom;
    uniform int maxIter;
    uniform int fractalType; // Uniform to select fractal type
    uniform int colorScheme; 

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

    vec3 oceanSunsetColor(float i, float maxIter, vec2 c) {
        float hue = 200.0 + mod(i * 10.0 + length(c) * 50.0, 100.0);
        float saturation = 0.8;
        float brightness = 0.9;
        return hsv2rgb(vec3(hue / 360.0, saturation, brightness));
    }

    vec3 electricNeonColor(float i, float maxIter, vec2 c) {
        float hue = mod(i * 10.0 + length(c) * 50.0, 360.0);
        float saturation = 1.0;
        float brightness = 1.0;
        return hsv2rgb(vec3(hue / 360.0, saturation, brightness));
    }
vec3 mixedColor(float i, float maxIter, vec2 c) {
    float hue = mod(i * 10.0 + length(c) * 50.0, 360.0); // Calculate hue based on iteration and position
    return hsv2rgb(vec3(hue, 0.8, 0.9)); // Convert HSV to RGB
}


vec3 getColor(float i, float maxIter, vec2 c) {
    vec3 color;
    if (colorScheme == 0) {
        color = mixedColor(i, maxIter, c);
    } else if (colorScheme == 1) {
        color = oceanSunsetColor(i, maxIter, c);
    } else if (colorScheme == 2) {
        color = electricNeonColor(i, maxIter, c);
    } else {
        color = oceanSunsetColor(i, maxIter, c); // Default to ocean sunset
    }
    return color;
}


    void main() {
        float x = coord.x;
        float y = coord.y;

        // Grid and axes
        float gridSpacing = 0.05 * zoom;
        float gridWidth = 0.002;
        float gridX = smoothstep(gridWidth, 0.0, abs(mod(x + offset.x / zoom, gridSpacing) - gridSpacing * 0.5)) * 0.3;
        float gridY = smoothstep(gridWidth, 0.0, abs(mod(y + offset.y / zoom, gridSpacing) - gridSpacing * 0.5)) * 0.3;
        vec3 grid = vec3(gridX + gridY);

        float axisWidth = 0.003;
        float axisX = smoothstep(axisWidth, 0.0, abs(x + offset.x / zoom)) * 0.6;
        float axisY = smoothstep(axisWidth, 0.0, abs(y + offset.y / zoom)) * 0.6;
        vec3 axes = vec3(axisX + axisY);

        vec3 background = grid + axes;

        float cx, cy;
        if (fractalType == 1) { // Julia set
            cx = -0.7; // You can adjust these values
            cy = 0.27015;
        } else { // Mandelbrot set
            cx = (x * 3.5 - 2.5 + offset.x) / zoom;
            cy = (y * 2.0 - 1.0 + offset.y) / zoom;
        }

        float zx, zy;
        if (fractalType == 1) { // Julia set
            zx = (x * 3.5 - 2.5 + offset.x) / zoom;
            zy = (y * 2.0 - 1.0 + offset.y) / zoom;
        } else if (fractalType == 2) { // Burning Ship set
            zx = (x * 3.5 - 2.5 + offset.x) / zoom;
            zy = (y * 2.0 - 1.0 + offset.y) / zoom;
        } else if (fractalType == 3) { // Tricorn set
            zx = (x * 3.5 - 2.5 + offset.x) / zoom;
            zy = (y * 2.0 - 1.0 + offset.y) / zoom;
        } else if (fractalType == 4) { // Multibrot set
            zx = (x * 3.5 - 2.5 + offset.x) / zoom;
            zy = (y * 2.0 - 1.0 + offset.y) / zoom;
        } else { // Mandelbrot set
            zx = 0.0;
            zy = 0.0;
        }

        float i = 0.0;
        for (i = 0.0; i < float(maxIter) && (zx * zx + zy * zy) < 4.0; i += 1.0) {
            float temp;
            if (fractalType == 2) { // Burning Ship set
                temp = zx * zx - zy * zy + cx;
                zy = abs(2.0 * zx * zy) + cy;
                zx = abs(temp);
            } else if (fractalType == 3) { // Tricorn set
                temp = zx * zx - zy * zy + cx;
                zy = -2.0 * zx * zy + cy;
                zx = temp;
            } else if (fractalType == 4) { // Multibrot set
                temp = zx * zx * zx - 3.0 * zx * zy * zy + cx;
                zy = 3.0 * zx * zx * zy - zy * zy * zy + cy;
                zx = temp;
            } else { // Julia and Mandelbrot sets
                temp = zx * zx - zy * zy + cx;
                zy = 2.0 * zx * zy + cy;
                zx = temp;
            }
        }

        vec3 color = getColor(i, float(maxIter), vec2(cx, cy));
        vec3 finalColor = mix(background, color, smoothstep(0.0, 0.5, i / float(maxIter)));

        FragColor = vec4(finalColor, 1.0);
    }
)glsl";





// Global variables for zoom, offset, and mouse interaction
float zoom = 1.0f;
glm::vec2 offset(0.0f, 0.0f);
glm::vec2 lastCursorPos(0.0f, 0.0f);
bool leftMouseButtonPressed = false;
int maxIter = 1024; // Adding global variable for maximum iterations

// Function to handle resizing of the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Global variable to track initial cursor position for zooming
glm::vec2 initialCursorPos(0.0f, 0.0f);
bool initialZoomDone = false;

// Function to handle mouse scroll events for zooming
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Get cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Convert cursor position to Mandelbrot set coordinates
    float x = static_cast<float>(xpos) / width * 2.0f - 1.0f;
    float y = 1.0f - static_cast<float>(ypos) / height * 2.0f;

    // Adjust zoom based on mouse wheel scroll
    float zoomFactor = pow(1.1f, static_cast<float>(yoffset));
    zoom *= zoomFactor;
    zoom = std::max(zoom, 1.0e-6f); // Clamp zoom to avoid going to zero or negative

    // Calculate new offset to keep the section centered on cursor position
    offset.x = x - (x - offset.x) * zoomFactor;
    offset.y = y - (y - offset.y) * zoomFactor;
}

// Function to handle mouse button events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMouseButtonPressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            displayWaveformFlag = true;
            // Correct type for xpos and ypos
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastCursorPos.x = static_cast<float>(xpos);
            lastCursorPos.y = static_cast<float>(ypos);

            float x_ndc = static_cast<float>(xpos) / width * 2.0f - 1.0f;
            float y_ndc = 1.0f - static_cast<float>(ypos) / height * 2.0f;

            // Check if the click is within the fractal area (in NDC, it's between -1 and 1)
            if (x_ndc >= -1.0f && x_ndc <= 1.0f && y_ndc >= -1.0f && y_ndc <= 1.0f) {
                // Play sound if click is within fractal area
                float frequency = 500.0f + (lastCursorPos.x) * 100.0f;
                int iterationCount = maxIter;
                playFractalSound(iterationCount, frequency);
                
                
            }
        }
        else if (action == GLFW_RELEASE) {
            leftMouseButtonPressed = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

// Function to handle mouse cursor position for panning
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    // Calculate cursor movement since last position
    double deltaX = xpos - lastX;
    double deltaY = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    // Adjust offset based on cursor movement
    if (leftMouseButtonPressed) {
        float sensitivity = 0.003f; // Adjust sensitivity to control the panning speed
        offset.x -= static_cast<float>(deltaX) * sensitivity;
        offset.y += static_cast<float>(deltaY) * sensitivity; // Invert y due to NDC y-axis orientation
    }
}


// Utility function to compile shader
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }

    return shader;
}

// Utility function to create shader program
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
    }

    // Clean up shaders as they're now linked into our program and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Function to set up vertex data
GLuint setupVertexData() {
    // Set up vertex data (position) and buffers and configure vertex attributes
    float vertices[] = {
        // Positions
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

// Function to initialize ImGui
void initImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    // Increase global font scale
    io.FontGlobalScale = 2.5f;
    // Set ImGui window size and position

}
const char* fractalNames[] = { "Mandelbrot", "Julia", "Burning Ship", "Tricorn", "Multibrot" };


const char* colorSchemes[] = { "Mixed", "Ocean Sunset", "Electric Neon" };

int currentFractal = 0; // Index of the selected fractal
int currentColorScheme = 0; // Index of the selected color scheme

GLuint loadTexture(const char* path) {
    GLuint textureID = 0;
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // Flip loading vertically because OpenGL's texture coordinates are flipped
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // Set texture wrap and filter modes if needed
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

void renderImGui(GLFWwindow* window) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, 400));
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - 450));

    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::SliderFloat("Zoom", &zoom, 0.1f, 10.0f);
    ImGui::SliderFloat2("Offset", &offset.x, -2.0f, 2.0f);
    ImGui::Combo("Fractal Type", &currentFractal, fractalNames, IM_ARRAYSIZE(fractalNames), -1);
    ImGui::SliderInt("Max Iterations", &maxIter, 24, 2048);




    // Combo box for color schemes
    ImGui::Combo("Color Scheme", &currentColorScheme, colorSchemes, IM_ARRAYSIZE(colorSchemes), -1);
  
    ImGui::SetCursorPosX(2000);
    ImGui::SetCursorPosY(38);


    const char* logoPath = "C:/Users/Kshitiz/Pictures/Screenshots/logo_final.png";
    GLuint your_logo_texture_id = loadTexture(logoPath); // Replace with your actual image loading function

    // Display the loaded image using its texture ID
    ImVec2 imageSize(200.0f, 200.0f); // Increase size as needed
    ImGui::Image((void*)(intptr_t)your_logo_texture_id, imageSize);

    ImGui::SetCursorPosX(1950);
    ImGui::SetCursorPosY(200); // Adjust Y position as needed
    ImGui::Text("Fractal Visualiser : \n Kshitiz Raj Paudyal \n Ashim Tiwari");
    ImGui::End();

    // Waveform visualizer window


    // Call the waveform visualizer function
    ImVec2 cursorPos = io.MousePos;
    float frequency = 500.0f + (cursorPos.x) * 100.0f;

    // Call the waveform visualizer function with the calculated frequency
    if (displayWaveformFlag) {
        displayWaveform(frequency); // Call the waveform visualizer function with the frequency parameter
    }

    

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




// Main rendering function
void renderScene(GLFWwindow* window, GLuint shaderProgram, GLuint VAO) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glUniform2f(glGetUniformLocation(shaderProgram, "offset"), offset.x, offset.y);
    glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), zoom);
    glUniform1i(glGetUniformLocation(shaderProgram, "maxIter"), maxIter);
    glUniform1i(glGetUniformLocation(shaderProgram, "fractalType"), currentFractal);

    // Ensure currentColorScheme is within valid range
    if (currentColorScheme < 0) currentColorScheme = 0;
    if (currentColorScheme >= IM_ARRAYSIZE(colorSchemes)) currentColorScheme = IM_ARRAYSIZE(colorSchemes) - 1;

    // Pass currentColorScheme to shader
    glUniform1i(glGetUniformLocation(shaderProgram, "colorScheme"), currentColorScheme);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    renderImGui(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
}





int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    const int width = mode->width;
    const int height = mode->height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Fractal Visualiser", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    initImGui(window);

    GLuint shaderProgram = createShaderProgram();
    GLuint VAO = setupVertexData();

    while (!glfwWindowShouldClose(window)) {
        renderScene(window, shaderProgram, VAO);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}