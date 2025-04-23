cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(HttpsSample)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Https Sample")
set(VITA_TITLEID  "VSDK00023")
set(VITA_VERSION  "01.00")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS} -a 0x2F00000000000001")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -fno-inline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

enable_language(C ASM)

add_executable(${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  ScePaf_stub
  SceNet_stub
  SceNetCtl_stub
  SceHttp_stub
  SceSsl_stub
  SceKernelThreadMgr_stub
  SceLibKernel_stub
  SceIofilemgr_stub
  SceVshBridge_stub
  SceSysmodule_stub
  SceSysmem_stub
)

vita_create_self(http_test.self ${PROJECT_NAME}
  UNSAFE
)

vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} http_test.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
)
