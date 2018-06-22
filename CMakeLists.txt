# CMake entry point
# Note that this CMakeLists.txt in fully independent from the one in the parent directory
# (since that's the goal of this tutorial !)

# For an introduction to CMake, see
# http://www.cmake.org/cmake/help/cmake_tutorial.html (at least steps 1 and 2)

message(
"
       
           ┌─┐       ┌─┐
        ┌──┘ ┴───────┘ ┴──┐
        │                 │
        │       ───       │
        │  ─┬┘       └┬─  │
        │                 │
        │       ─┴─       │
        │                 │
        └───┐         ┌───┘
            │         │
            │         │
            │         │
            │         └──────────────┐
            │                        │
            │                        ├─┐
            │                        ┌─┘    
            │                        │
            └─┐  ┐  ┌───────┬──┐  ┌──┘         
              │ ─┤ ─┤       │ ─┤ ─┤         
              └──┴──┘       └──┴──┘ 
                  神兽  保佑 
                  代码无BUG! 
  
")

cmake_minimum_required (VERSION 3.10)
set(name "FileSaveSimulator")
project ( ${name} )

#SET(CMAKE_C_COMPILER gcc)
#SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGES "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11")

set(CMAKE_CXX_STANDARD 11)


set(MY_HEADER_FILES
	# C++ header file
    Common.hpp
    INIReader.h
    ControlQueue.h
)

set(MY_SOURCE_FILES 
	# C++ source file
	main.cpp
    ControlQueue.cpp
)


include_directories(
	${PROJECT_SOURCE_DIR}	
)

add_executable(${name}
	${MY_HEADER_FILES}
	${MY_SOURCE_FILES}
)

target_link_libraries(${name}
)
