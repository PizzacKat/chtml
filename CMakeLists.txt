cmake_minimum_required(VERSION 3.20)
project(chtml)

set(CMAKE_C_STANDARD 17)

include_directories(include)
add_executable(${PROJECT_NAME} src/main.c src/compiler.c src/codegen.c src/parser.c src/node.c src/tokenizer.c src/token.c)
