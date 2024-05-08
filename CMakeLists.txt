cmake_minimum_required(VERSION 3.5)
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif(POLICY CMP0048)
# project name version languages
project(autoland_gui VERSION 1.0.3 LANGUAGES CXX)

# set C++ standard
set(CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

set(Qt5_DIR /opt/homebrew/Cellar/qt@5/5.15.13_1/lib/cmake/Qt5)

# find package
find_package(Qt5 COMPONENTS 
    Widgets
    Gui
    Core
)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# include dir
include_directories(
    include
)

add_executable(${PROJECT_NAME} include/mathassist.h include/myopengl.h include/MainWindow.h src/mathassist.cpp src/myopengl.cpp src/main.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})