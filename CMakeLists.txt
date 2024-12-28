cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME embeded_stmepic_base_project)




############################################################################################################
# C++ Configuration  [1/4]
set(CMAKE_CXX_STANDARD 17)

# aditional compiler flags
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u_printf_float -Wreturn-type")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}  -u_printf_float -Wreturn-type")

# aditional linker flags
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}  -u_printf_float -Wreturn-type")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}  -u_printf_float -Wreturn-type")


## enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(NOT CCACHE_PROGRAM)
    message(STATUS "ccache not found")
else()
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

############################################################################################################









# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


############################################################################################################
# C++ Configuration  [2/4]

# Enable CMake support for ASM and C languages
enable_language(CXX C ASM)

############################################################################################################


# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)


############################################################################################################
# Add sources subdirectory and include directories link libriries  [3/4]
# congigure the project here
set(STMEPIC_DIR ${CMAKE_SOURCE_DIR}/../../StmEpic)
add_subdirectory(${STMEPIC_DIR} ${CMAKE_BINARY_DIR}/StmEpic)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  # Add user sources here
  "src/main_prog.cpp"
)



# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  # Add user defined include paths
  "src"
)




# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)



# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
  stm32cubemx
  etl::etl
    # Add user defined libraries
)

############################################################################################################




############################################################################################################
# Configure nice task to make life easy  [4/4]

add_custom_command(
	TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf firmware.bin
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf firmware.hex
	COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.elf firmware.elf
  # Show ccache statistics
 )


add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
)

if(CCACHE_PROGRAM)
  add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CCACHE_PROGRAM} -s
  )
endif()