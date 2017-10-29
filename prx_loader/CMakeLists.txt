cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
project(prx_loader)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall")
link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../common)
target_link_libraries(${PROJECT_NAME}
  SceLibKernel_stub
  SceSysmem_stub
  SceLibc_stub
  SceDisplay_stub
  SceIofilemgr_stub
  SceKernelThreadMgr_stub
  SceCtrl_stub
)
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)
vita_create_vpk(${PROJECT_NAME}.vpk VSDK00012 ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${PROJECT_NAME}
)
