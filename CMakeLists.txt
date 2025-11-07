cmake_minimum_required(VERSION 3.31)
project(The_CleanInbox_Crew)

set(CMAKE_CXX_STANDARD 20)

add_executable(The_CleanInbox_Crew
        src/main.cpp
        src/ml/NaiveBayes.cpp
)
