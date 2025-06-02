add_library(doublemetaphone STATIC double_metaphone.cpp)
target_include_directories(doublemetaphone PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
