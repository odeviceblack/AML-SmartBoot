cmake_minimum_required(VERSION 3.10)
project(FastBoot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AML_PATH "/data/data/com.termux/files/home/Android/shared/amlmods")

add_library(FastBoot SHARED
	${AML_PATH}/include/mod/config.cpp
	${AML_PATH}/include/mod/logger.cpp

	src/Main.cpp
	src/SocialClub.cpp
	src/StartMode.cpp
)

target_include_directories(FastBoot PRIVATE
	${AML_PATH}/include
)

# Common flags
target_compile_options(FastBoot PRIVATE
	-Wall
	-Wextra
)

set_source_files_properties(
	${AML_PATH}/include/mod/logger.cpp
	${AML_PATH}/include/mod/config.cpp
	PROPERTIES
	COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-sign-compare"
)

# Debug
target_compile_options(FastBoot PRIVATE
	$<$<CONFIG:Debug>:
		-O0
		-g3
		-fno-omit-frame-pointer
	>
)

target_link_options(FastBoot PRIVATE
	$<$<CONFIG:Debug>:
		-g3
	>
)

# Release
target_compile_options(FastBoot PRIVATE
	$<$<CONFIG:Release>:
		-O2
		-flto
		-fvisibility=hidden
		-fomit-frame-pointer
		-fno-exceptions
		-fno-rtti
		-ffunction-sections
		-fdata-sections
	>
)

target_link_options(FastBoot PRIVATE
	$<$<CONFIG:Release>:
		-flto
		-s
		-Wl,--gc-sections
		-Wl,--strip-all
	>
)

target_link_libraries(FastBoot PRIVATE
	log
)
