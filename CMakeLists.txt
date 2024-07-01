cmake_minimum_required(VERSION 3.16)

# Set the vcpkg toolchain file
# set(CMAKE_TOOLCHAIN_FILE ./vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

project(r-type)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif (WIN32)

find_package(asio CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(Boost COMPONENTS system filesystem serialization thread REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME}_server ./Network/server/server.cpp ./Network/server/sender.cpp)
add_executable(${PROJECT_NAME}_game)
add_executable(${PROJECT_NAME}_test ./Test/main.cpp)

target_compile_options(${PROJECT_NAME}_game PRIVATE -g3)
target_compile_options(${PROJECT_NAME}_test PRIVATE -g3)

# Add all source files
add_library(${PROJECT_NAME}_engine SHARED
    ./Engine/src/Core/Engine.cpp
    ./Engine/src/Core/Scene.cpp
    ./Engine/src/Core/Asset.cpp
    ./Engine/src/Core/Camera.cpp
    ./Engine/src/Systems/PhysicsSystem.cpp
    ./Engine/src/Systems/RenderingSystem.cpp
    ./Engine/src/Systems/InputSystem.cpp
    ./Engine/src/Systems/CollisionSystem.cpp
    ./Engine/src/Systems/UserInterfaceSystem.cpp
    ./Engine/src/Systems/AudioSystem.cpp
    ./Engine/src/Systems/EntityDataSystem.cpp
    ./Engine/src/Components/Physics.hpp
    ./Engine/src/Components/Rendering.hpp
    ./Engine/src/Core/Entity.hpp
    ./Engine/src/Core/Engine.hpp
    ./Engine/src/Core/Component.hpp
    ./Engine/src/Core/Coordinator.hpp
    ./Engine/src/Core/Scene.hpp
    ./Engine/src/Core/Asset.hpp
    ./Engine/src/Core/Camera.hpp
    ./Engine/src/Core/System.hpp
    ./Engine/src/Systems/PhysicsSystem.hpp
    ./Engine/src/Systems/RenderingSystem.hpp
    ./Engine/src/Systems/InputSystem.hpp
    ./Engine/src/Systems/CollisionSystem.hpp
    ./Engine/src/Systems/UserInterfaceSystem.hpp
    ./Engine/src/Systems/AudioSystem.hpp
    ./Engine/src/Systems/EntityDataSystem.hpp
)

target_include_directories(${PROJECT_NAME}_engine PRIVATE ./Engine/src)

# Add the main.cpp file and the Engine source files
target_sources(
    ${PROJECT_NAME}_game
    PRIVATE
    ./Games/R-Type/main.cpp
    ./Games/R-Type/include/Game.cpp
    ./Games/R-Type/include/Game.hpp
)

# Linkage des librairies
target_link_libraries(${PROJECT_NAME}_engine PRIVATE raylib asio::asio Boost::system Boost::filesystem Boost::serialization Boost::thread)
target_link_libraries(${PROJECT_NAME}_game PRIVATE ${PROJECT_NAME}_engine raylib asio::asio Boost::system Boost::filesystem Boost::serialization Boost::thread)
target_link_libraries(${PROJECT_NAME}_server PRIVATE asio::asio Boost::system Boost::filesystem Boost::serialization)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}_engine raylib asio::asio Boost::system Boost::filesystem Boost::serialization)

# Set C++ version to C++20
target_compile_features(${PROJECT_NAME}_server PRIVATE cxx_std_20)
target_compile_features(${PROJECT_NAME}_engine PRIVATE cxx_std_20)
target_compile_features(${PROJECT_NAME}_game PRIVATE cxx_std_20)
target_compile_features(${PROJECT_NAME}_test PRIVATE cxx_std_20)

# Include the "include" directory for both targets
target_include_directories(${PROJECT_NAME}_game PRIVATE ./Game/include)

add_custom_target(run
    COMMAND ${PROJECT_NAME}_game
    DEPENDS ${PROJECT_NAME}_game
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)