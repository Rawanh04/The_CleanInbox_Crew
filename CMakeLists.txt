cmake_minimum_required(VERSION 3.31)
project(The_CleanInbox_Crew LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(The_CleanInbox_Crew
        src/main.cpp
        src/ml/NaiveBayes.cpp
        src/core/CsvReader.cpp
        src/ml/Rocchio.cpp
        # src/ml/Rocchio.cpp
)

target_include_directories(The_CleanInbox_Crew PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

 add_executable(test_csv
     apps/test_csv.cpp
     src/core/CsvReader.cpp
         include/ml/Rocchio.h
         apps/test_rocchio.cpp
 )

add_executable(test_rocchio
        apps/test_rocchio.cpp
        src/ml/Rocchio.cpp
        src/core/CsvReader.cpp
)

target_include_directories(test_rocchio PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# target_include_directories(test_csv PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
