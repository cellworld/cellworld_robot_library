cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( robot_lib
        VERSION 2021.1.0
        DESCRIPTION "Cellworld vr utilities"
        LANGUAGES CXX)

####
#### DEPENDENCIES
####

find_package (Dependencies QUIET)

if (NOT ${Dependencies_FOUND})
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
        file(DOWNLOAD https://raw.githubusercontent.com/germanespinosa/dependencies/main/DependenciesConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
    endif()
    set(Dependencies_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    find_package (Dependencies REQUIRED)
endif()

####
#### END DEPENDENCIES
####

string(APPEND CMAKE_CXX_FLAGS " -pthread ")
install_git_dependency( Gamepad_lib
        https://github.com/cellworld/dependency_gamepad_lib
        CMAKE_PROJECT
        IMPORT_TARGETS gamepad_lib)
install_git_dependency(Cellworld
        https://github.com/cellworld/cellworld
        CMAKE_PROJECT
        IMPORT_TARGETS cellworld requests json-cpp)
install_git_dependency(Params-cpp https://github.com/cellworld/dependency_params-cpp
        CMAKE_PROJECT
        IMPORT_TARGETS params-cpp)
install_git_dependency(Easy-tcp https://github.com/cellworld/dependency_easy-tcp
        CMAKE_PROJECT
        IMPORT_TARGETS easy-tcp)
install_git_dependency(Tcp_messages https://github.com/cellworld/dependency_tcp-messages
        CMAKE_PROJECT
        IMPORT_TARGETS tcp_messages)
install_git_dependency(Agent_tracking https://github.com/cellworld/cellworld_agent_tracking
        CMAKE_PROJECT
        IMPORT_TARGETS agent_tracking)
install_git_dependency(Experiment_service https://github.com/cellworld/cellworld_experiment_service
        CMAKE_PROJECT
        IMPORT_TARGETS experiment_service)
install_git_dependency(Controller https://github.com/germanespinosa/cellworld_pid_controller
        CMAKE_PROJECT
        IMPORT_TARGETS controller)

###
### MAIN LIBRARY SETUP
###

set(robot_lib_files
        src/robot.cpp
        src/robot_simulator.cpp
        src/tracking_simulator.cpp
        src/gamepad_wrapper.cpp
        src/robot_agent.cpp
        src/robot_agent_new.cpp
        src/prey_simulator.cpp)

include_directories(include)

add_library(robot_lib ${robot_lib_files})

set_target_properties(robot_lib
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED On
        CXX_EXTENSIONS Off
        VERSION ${PROJECT_VERSION})

target_include_directories(robot_lib
        SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_include_directories(robot_lib
        PRIVATE
        include )

target_link_libraries( robot_lib
        LINK_PUBLIC
        agent_tracking
        params-cpp
        gamepad_lib
        experiment_service
        controller)

###
### UTILITIES SETUP
###

include_directories( include )

add_executable(robot src/tool/fake_robot.cpp)
target_link_libraries( robot PUBLIC robot_lib)

add_executable(stop_fake_robot src/tool/stop_fake_robot.cpp)
target_link_libraries( stop_fake_robot PUBLIC robot_lib)

add_executable(robot_controller src/tool/controller.cpp)
target_link_libraries( robot_controller PUBLIC robot_lib)

add_executable(gamepad src/tool/gamepad.cpp)
target_link_libraries( gamepad PUBLIC robot_lib)

add_executable(hybrid_gamepad src/tool/hybrid_gamepad.cpp)
target_link_libraries( hybrid_gamepad PUBLIC robot_lib)

#add_executable(hybrid_gamepad2 src/tool/hybrid_gamepad2.cpp)
#target_link_libraries( hybrid_gamepad2 PUBLIC robot_lib)

add_executable(robot_paths src/tool/robot_paths.cpp)
target_link_libraries(robot_paths PUBLIC robot_lib)

add_executable(robot_worlds src/tool/robot_worlds.cpp)
target_link_libraries(robot_worlds PUBLIC robot_lib)
