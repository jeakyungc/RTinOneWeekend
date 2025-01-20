# 최소 CMake 버전 설정
cmake_minimum_required(VERSION 3.10)
# 프로젝트 이름 및 언어 설정
project(RTinOneWeekend VERSION 1.0 LANGUAGES CXX)
# C++ 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Windows 스택 크기 설정
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
endif()


# 실행 파일 생성
add_executable(RTinOneWeekend src/main.cc)