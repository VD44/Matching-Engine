cmake_minimum_required(VERSION 3.15)
project(Match)

set(CMAKE_CXX_STANDARD 14)

add_executable(Match main.cpp MatchingEngine.cpp MatchingEngine.h)