cmake_minimum_required(VERSION 3.20)
project(fices VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE sources sources/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${sources})

find_package(SDL3 REQUIRED)
find_package(EnTT REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)

# target link libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        EnTT::EnTT
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        SDL3::SDL3
        glm::glm
)