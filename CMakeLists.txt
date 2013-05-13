cmake_minimum_required(VERSION 2.8)

project(image-server)

set(SOURCE_EXE main.cpp base64.cpp UrlHelper.cpp webserver.cpp response.cpp)

set(HEADER base64.h UrlHelper.h webserver.h)

add_executable(main ${SOURCE_EXE})
