# Shader Playground 🎮💡

A lightweight real-time OpenGL shader editor built with **GLFW**, **ImGui**, and **GLAD** using **C++** and **CMake**.

## 🚀 Features

- Live shader editing and compilation
- Real-time rendering window
- Minimal UI with ImGui integration
- Easily extensible for experimentation

## 🧰 Tech Stack

- C++
- OpenGL
- ImGui
- GLFW
- GLAD
- CMake

## 🛠️ Setup Instructions

### 1. Clone the repo
```bash
git clone https://github.com/Apostrphe/shader-playground.git
cd shader-playground
```
### 2. Build the project
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
### 3. Project Structure
```
shader-playground/
├── libs/                 # External libraries (GLFW, GLAD, ImGui)
├── shaders/              # GLSL shader files
├── main.cpp              # Entry point
├── CMakeLists.txt        # Build configuration
