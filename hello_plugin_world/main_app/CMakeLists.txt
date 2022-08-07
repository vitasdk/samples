project(main_app)
add_executable(${PROJECT_NAME}
	main.c
	# Add every source file common to whole application here, even if it's intended to be used by plugins
	../../common/debugScreen.c
	)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall")
link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../../common ../common)

# These two directive make sure all our code is bundled even if we don't use it in the main binary
# These must come before other target_link_libraries to ensure libraries don't get wholly linked
# This may not be what we want
target_link_options(${PROJECT_NAME} PRIVATE
	-Wl,--whole-archive
)
target_link_libraries(${PROJECT_NAME}
	-Wl,--no-whole-archive
)

target_link_libraries(${PROJECT_NAME}
  SceLibKernel_stub
  SceSysmem_stub
  SceDisplay_stub
  SceIofilemgr_stub
  SceKernelThreadMgr_stub
  SceCtrl_stub
)

vita_create_self(${PROJECT_NAME}.tmp.self ${PROJECT_NAME})
add_custom_target(${PROJECT_NAME}.self
	DEPENDS ${PROJECT_NAME}.tmp.self
	COMMAND psp2rela -static_mode -fetch_base -src=${PROJECT_NAME}.tmp.self -dst=${PROJECT_NAME}.self)
