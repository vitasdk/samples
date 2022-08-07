cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(hello_plugin_world)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

add_subdirectory(main_app)
add_subdirectory(plugin)

vita_create_vpk(${PROJECT_NAME}.vpk VSDK00021 ${CMAKE_BINARY_DIR}/main_app/main_app.self
  FILE ${CMAKE_BINARY_DIR}/plugin/plugin.suprx plugin.suprx
  VERSION ${VITA_VERSION}
  NAME ${PROJECT_NAME}
)
add_dependencies(${PROJECT_NAME}.vpk main_app.self plugin.suprx)
