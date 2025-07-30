#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <C:\Users\Athul Raghav\CLionProjects\shader-playground\libs\imgui\imgui.h>
#include <C:\Users\Athul Raghav\CLionProjects\shader-playground\libs\imgui\backends\imgui_impl_glfw.h>
#include <C:\Users\Athul Raghav\CLionProjects\shader-playground\libs\imgui\backends\imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <cstring>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int compileShader(unsigned int type, const char* source);
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource);
bool checkShaderCompilation(unsigned int shader, const std::string& type);
bool checkProgramLinking(unsigned int program);

const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 800;

// Default vertex shader (full-screen quad)
const char* defaultVertexShader =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"}\n";

// Default fragment shader (animated rainbow)
std::string defaultFragmentShader =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n"
"uniform vec2 iMouse;\n"
"\n"
"void main() {\n"
"    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
"    vec3 color = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";

// Example shaders
std::string simpleGradient =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec2 iResolution;\n"
"\n"
"void main() {\n"
"    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
"    FragColor = vec4(uv, 0.5, 1.0);\n"
"}\n";

std::string pulsingCircle =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n"
"\n"
"void main() {\n"
"    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;\n"
"    float d = length(uv);\n"
"    float pulse = sin(iTime * 2.0) * 0.1 + 0.3;\n"
"    vec3 color = vec3(1.0 - smoothstep(pulse - 0.02, pulse + 0.02, d));\n"
"    color *= vec3(0.2 + 0.8 * sin(iTime), 0.5, 1.0);\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";

std::string fractalMandelbrot =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n"
"\n"
"void main() {\n"
"    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;\n"
"    vec2 c = uv * 2.0;\n"
"    vec2 z = vec2(0.0);\n"
"    int iter = 0;\n"
"    for(int i = 0; i < 100; i++) {\n"
"        if(dot(z, z) > 4.0) break;\n"
"        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;\n"
"        iter++;\n"
"    }\n"
"    float t = float(iter) / 100.0;\n"
"    vec3 color = 0.5 + 0.5 * cos(3.0 + t*4.0 + vec3(0,0.6,1.0));\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";

std::string noisyPlasma =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform float iTime;\n"
"uniform vec2 iResolution;\n"
"\n"
"float noise(vec2 p) {\n"
"    return sin(p.x) * sin(p.y);\n"
"}\n"
"\n"
"void main() {\n"
"    vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
"    float t = iTime * 0.5;\n"
"    float n1 = noise(uv * 4.0 + t);\n"
"    float n2 = noise(uv * 8.0 - t * 0.7);\n"
"    float n3 = noise(uv * 16.0 + t * 0.3);\n"
"    float plasma = sin(uv.x * 10.0 + n1 + t) + sin(uv.y * 10.0 + n2 + t) + sin((uv.x + uv.y) * 5.0 + n3 + t);\n"
"    vec3 color = 0.5 + 0.5 * cos(plasma + vec3(0, 2, 4));\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                         "🎨 Shader Playground - Live Editor", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.FontGlobalScale = 1.1f; // Slightly larger fonts

    ImGui::StyleColorsDark();

    // Customize ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.WindowPadding = ImVec2(10, 10);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "🎮 Shader Playground initialized successfully!" << std::endl;

    // Full-screen quad vertices
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,  // bottom left
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f,  1.0f, 0.0f,  // top left
         1.0f,  1.0f, 0.0f   // top right
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        1, 2, 3   // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create initial shader program
    unsigned int shaderProgram = createShaderProgram(defaultVertexShader, defaultFragmentShader.c_str());

    // Shader editor variables
    char shaderBuffer[16384]; // Increased buffer size
    std::strcpy(shaderBuffer, defaultFragmentShader.c_str());
    bool shaderChanged = false;
    std::string compileError = "✅ Shader compiled successfully!";
    bool autoCompile = true;
    float timeSpeed = 1.0f;

    // Mouse tracking
    double mouseX = 0.0, mouseY = 0.0;

    // Performance tracking
    float frameTime = 0.0f;
    int frameCount = 0;
    float totalTime = 0.0f;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Track performance
        float currentTime = (float)glfwGetTime();
        frameCount++;

        // Get mouse position
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = SCR_HEIGHT - mouseY; // Flip Y coordinate

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main shader editor window
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("🎨 Shader Editor", nullptr, ImGuiWindowFlags_MenuBar);

        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Reset to Default")) {
                    std::strcpy(shaderBuffer, defaultFragmentShader.c_str());
                    shaderChanged = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Auto Compile", nullptr, &autoCompile);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("Fragment Shader Editor");
        ImGui::Separator();

        // Large text editor for shader code
        ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AllowTabInput;
        // Large text editor for shader code
        if (ImGui::InputTextMultiline("##shader", shaderBuffer, sizeof(shaderBuffer),
                                     ImVec2(-1, 400), ImGuiInputTextFlags_AllowTabInput)) {
            if (autoCompile) {
                shaderChanged = true;
            }
                                     }

        // Manual compile button
        ImGui::Separator();
        if (ImGui::Button("🔨 Compile Shader") || (shaderChanged && autoCompile)) {
            unsigned int newProgram = createShaderProgram(defaultVertexShader, shaderBuffer);
            if (newProgram != 0) {
                glDeleteProgram(shaderProgram);
                shaderProgram = newProgram;
                compileError = "✅ Shader compiled successfully!";
            } else {
                compileError = "❌ Shader compilation failed! Check console for details.";
            }
            shaderChanged = false;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto Compile", &autoCompile);

        // Compile status
        ImVec4 statusColor = (compileError.find("✅") != std::string::npos) ?
                            ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImGui::TextColored(statusColor, "%s", compileError.c_str());

        ImGui::End();

        // Controls window
        ImGui::SetNextWindowPos(ImVec2(620, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("🎛️ Controls");

        ImGui::Text("Uniforms");
        ImGui::Separator();
        ImGui::Text("Time: %.2f", currentTime * timeSpeed);
        ImGui::SliderFloat("Time Speed", &timeSpeed, 0.0f, 3.0f);
        ImGui::Text("Resolution: %dx%d", SCR_WIDTH, SCR_HEIGHT);
        ImGui::Text("Mouse: (%.0f, %.0f)", mouseX, mouseY);

        ImGui::Separator();
        ImGui::Text("Performance");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);

        ImGui::End();

        // Example shaders window
        ImGui::SetNextWindowPos(ImVec2(620, 420), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_FirstUseEver);
        ImGui::Begin("🎪 Example Shaders");

        ImGui::Text("Click to load:");
        ImGui::Separator();

        if (ImGui::Button("🌈 Rainbow Waves", ImVec2(-1, 30))) {
            std::strcpy(shaderBuffer, defaultFragmentShader.c_str());
            shaderChanged = true;
        }

        if (ImGui::Button("🎨 Simple Gradient", ImVec2(-1, 30))) {
            std::strcpy(shaderBuffer, simpleGradient.c_str());
            shaderChanged = true;
        }

        if (ImGui::Button("⭕ Pulsing Circle", ImVec2(-1, 30))) {
            std::strcpy(shaderBuffer, pulsingCircle.c_str());
            shaderChanged = true;
        }

        if (ImGui::Button("🌀 Mandelbrot Fractal", ImVec2(-1, 30))) {
            std::strcpy(shaderBuffer, fractalMandelbrot.c_str());
            shaderChanged = true;
        }

        if (ImGui::Button("⚡ Plasma Effect", ImVec2(-1, 30))) {
            std::strcpy(shaderBuffer, noisyPlasma.c_str());
            shaderChanged = true;
        }

        ImGui::Separator();
        ImGui::Text("💡 Tips:");
        ImGui::BulletText("Use iTime for animation");
        ImGui::BulletText("Use iResolution for aspect ratio");
        ImGui::BulletText("Use iMouse for interaction");
        ImGui::BulletText("Press ESC to exit");

        ImGui::End();

        // Render the shader output
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Clear with dark background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw full-screen quad with shader
        glUseProgram(shaderProgram);

        // Set uniforms
        int timeLocation = glGetUniformLocation(shaderProgram, "iTime");
        int resolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
        int mouseLocation = glGetUniformLocation(shaderProgram, "iMouse");

        if (timeLocation != -1)
            glUniform1f(timeLocation, currentTime * timeSpeed);
        if (resolutionLocation != -1)
            glUniform2f(resolutionLocation, (float)display_w, (float)display_h);
        if (mouseLocation != -1)
            glUniform2f(mouseLocation, (float)mouseX, (float)mouseY);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Render ImGui on top
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    std::cout << "👋 Shader Playground closed. Happy coding!" << std::endl;
    return 0;
}

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    if (!checkShaderCompilation(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    bool success = checkProgramLinking(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success) {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

bool checkShaderCompilation(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "🚫 " << type << " shader compilation failed:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

bool checkProgramLinking(unsigned int program) {
    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "🚫 Program linking failed:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}