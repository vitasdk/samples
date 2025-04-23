cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(json)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Json Example")
set(VITA_TITLEID  "VSDK00022")
set(VITA_VERSION  "01.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
# Building and linking with SceLibJson requires the compiler option -fno-rtti
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++11")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

include_directories(
	../common
)

add_executable(${PROJECT_NAME}.elf
  src/main.cpp
  ../common/debugScreen.c
)

target_link_libraries(${PROJECT_NAME}.elf
  SceLibJson_stub
  SceSysmodule_stub
  SceDisplay_stub
  SceCtrl_stub
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME}.elf)
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE test.json test.json
)
