cmake_minimum_required (VERSION 3.8)

# To enable hot reload on Visual Studio
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("pipeline")

set(CMAKE_CXX_STANDARD 20)

include_directories("include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message (STATUS "This is a Debug build")
    if (MSVC)
		message(STATUS "set debug info flags for MSVC")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /debug")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /debug")
	else()
		message(STATUS "set debug info flags for gcc")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")
	endif()
else()
    message (STATUS "This is a Release build")
endif()

add_library(pipeline_static STATIC "src/pipeline.cpp" "include/pipeline.h" "include/thread_pool.h")
add_library(pipeline_dynamic SHARED "src/pipeline.cpp" "include/pipeline.h" "include/thread_pool.h")

message(STATUS "setting error level")
if (MSVC)
	target_compile_options(pipeline_static PRIVATE /W4 /WX)
	target_compile_options(pipeline_dynamic PRIVATE /W4 /WX)
else()
	target_compile_options(pipeline_static PRIVATE -Wall -Wextra -pedantic -Werror)
	target_compile_options(pipeline_dynamic PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

