cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(prx_auto_export)
include("${VITASDK}/share/vita.cmake" REQUIRED)

# Add `-fvisibility=hidden` to temporarily unexport all functions. (But doesn't this apply to *.s files?)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -fvisibility=hidden -O2 -fno-inline -fno-builtin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fno-rtti -fno-exceptions -Wl,-q -Wall -fvisibility=hidden -O2 -fno-inline -fno-builtin")

set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS} -a 0x2F00000000000101")

enable_language(C ASM)

add_executable(${PROJECT_NAME}
  src/main.c
  src/math.s
  src/math.cpp
)

target_link_libraries(${PROJECT_NAME}
  SceLibc_stub
  SceLibKernel_stub
  SceRtabi_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
  COMPILE_FLAGS "-D__PSP2_USER__"
)

#  GEN_EXPORTS for Library
# MODULE_ENTRY for module entry

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME}
  GEN_EXPORTS ${CMAKE_CURRENT_BINARY_DIR}/exports.yml
  STRIPPED
  REL_OPTIMIZE
  MODULE_ENTRY vitasdk_sample_module_start,vitasdk_sample_module_stop,vitasdk_sample_module_exit
)

vita_create_stubs(stub_${PROJECT_NAME} ${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/exports.yml GEN_EXPORTS)
