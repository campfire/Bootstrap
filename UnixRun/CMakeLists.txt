INCLUDE_DIRECTORIES(../Environment)
FIND_LIBRARY(../Environment)

link_libraries(dl)
link_libraries(Environment)

add_executable(Run Run.cpp)