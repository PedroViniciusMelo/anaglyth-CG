cmake_minimum_required(VERSION 3.24)
project(Projeto)

set(CMAKE_CXX_STANDARD 14)
set(OpenGlLinkers -lglut -lGLU -lGL) # setting all the Glut libraries as one variable.


add_executable( Projeto Classes/Model.cpp main.cpp)

target_link_libraries(Projeto ${OpenGlLinkers}) # linking opengl libraries to the project


