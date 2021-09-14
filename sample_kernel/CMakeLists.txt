cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(sample_kernel)

set(SELF_NAME ":")
set(SELF_EXT ":")
set(SELF_COMPILE_FLAGS "-D__PSP2_KERNEL__")

if("${SELF_NAME}" MATCHES ":")
  set(SELF_NAME "${PROJECT_NAME}")
endif()

if("${SELF_EXT}" MATCHES ":")
  if("${SELF_COMPILE_FLAGS}" MATCHES "__PSP2_USER__")
    set(SELF_EXT "suprx")
  elseif("${SELF_COMPILE_FLAGS}" MATCHES "__PSP2_KERNEL__")
    set(SELF_EXT "skprx")
  endif()
endif()

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -fno-inline -fno-builtin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

enable_language(C ASM)

include_directories(
  ./include/
)

add_executable(${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  SceDebugForDriver_stub
  SceSysmemForDriver_stub
  SceSysclibForDriver_stub
  SceThreadmgrForDriver_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
  COMPILE_FLAGS "${SELF_COMPILE_FLAGS}"
)

vita_create_self(${SELF_NAME}.${SELF_EXT} ${PROJECT_NAME}
  CONFIG exports.yml
  UNSAFE
  # Pack the output velf to the proper size. this is optional.
  STRIPPED
)

if("${SELF_COMPILE_FLAGS}" MATCHES "__PSP2_USER__")
  vita_create_stubs(stubs_${PROJECT_NAME} ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml)
elseif("${SELF_COMPILE_FLAGS}" MATCHES "__PSP2_KERNEL__")
  vita_create_stubs(stubs_${PROJECT_NAME} ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml KERNEL)
endif()
