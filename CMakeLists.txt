cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(CommandLineToArgv LANGUAGES CXX)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		include/
)

target_sources(${PROJECT_NAME}
	PRIVATE
		main.cpp
		include/CommandLineToArgv.h
)

