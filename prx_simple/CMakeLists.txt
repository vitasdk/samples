cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
project(plugin)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../common)
target_link_libraries(${PROJECT_NAME}
  SceLibc_stub_weak
  SceNet_stub_weak
  SceNetCtl_stub_weak
  SceSysmem_stub_weak
  SceDisplay_stub_weak
  SceSysmodule_stub_weak
  SceIofilemgr_stub_weak
  SceLibKernel_stub_weak
  SceKernelThreadMgr_stub_weak
  SceKernelModulemgr_stub_weak
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -nostdlib")
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSAFE CONFIG ${CMAKE_SOURCE_DIR}/main.yml)

