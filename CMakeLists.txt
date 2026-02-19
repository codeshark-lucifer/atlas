cmake_minimum_required(VERSION 4.2) 
project(Atlas LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(vendor/glad) 

add_executable(app 
    src/main.cpp
    src/core/helper.cpp
    src/core/win32.cpp
)

target_include_directories(app PRIVATE 
    include
)

target_link_libraries(app PRIVATE 
    user32  # window creation, message loop, input handling
    gdi32   # basic graphics output(HDC, BitBlt, etc.)
    winmm   # multimedia timer for frame timing, sound output (PlaySound, etc.)
    glad    # OpenGL function loader
    dwmapi  # DWM for window composition, transparency, etc.
)