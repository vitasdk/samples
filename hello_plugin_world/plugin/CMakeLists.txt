project(plugin)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

# Depend on main application to get its symbols
add_dependencies(${PROJECT_NAME} main_app)

link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../../common ../common)

# No target link libraries: we only call main_app functions

# Make sure that all calls that could go to our main binary are properly encoded
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -mlong-calls -mword-relocations")
# If using C++, this will allow to call constructors and destructors of plugin at load/unload time
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fuse-cxa-atexit -mlong-calls -mword-relocations")

# Here we create a plugin binary taking all its symbols from main binary and not using any libc on its own
# Only libgcc is linked inside it if needed
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -nodefaultlibs -Wl,--just-symbols,${CMAKE_BINARY_DIR}/main_app/main_app -lgcc")

# We don't use system libraries so make vita-elf-create happy
set(VITA_ELF_CREATE_FLAGS "-n")

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/main.yml)
