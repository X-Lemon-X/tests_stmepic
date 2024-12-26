# Example repository on how to use the StmEpic library with ModuCard system, Nomad or Ariadna.

## Introduction
The repository shows how to start project using [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
The example project is configured for the ***STM32F446RETx*** microcontroller, however it can be quiclkly reconfigured for other stm32 microcontroler.

## Requirements

### Compiler:
Building the project requires GNU Arm Embedded Toolchain to be installed.
1. Download it from this site [GNU Arm Embedded Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) or directly from this [link](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz) 
2. Move the extracted files in some directory for example "$HOME/.local/share/gccarm", 
3. Add this in your ___.profile___ file 
```bash
# Add the arm-none-aebi to the path
if [ -d "$HOME/.local/share/gccarm/bin" ]; then
    PATH="$HOME/.local/share/gccarm/bin:$PATH" 
fi
```
4. restart the pc or run source ~/.profile
### Software:
1. You will most likely need to install the following packages:
```bash
sudo apt-get install -y cmake ninja-build clang ccache build-essential dfu-util stlink-tools python3-pip python3-venv
``` 
2. Install StmCubeIDE from this [link](https://www.st.com/en/development-tools/stm32cubeide.html)
3. Install StmCubeMX from this [link](https://www.st.com/en/development-tools/stm32cubemx.html)

## Generating the project from scratch
1. Open the StmCubeMX and File->New Project
2. Select appropriate microcontroller, in this case [for this example repo] it is ***STM32F446RET***
3. After that you can configure the peripherals and ther pinout in Pinout & Configuration tab
4. The clock configuration can be done in Clock Configuration tab
### Important settings In Project Manager tab
5. set the Toolchain/IDE to ***CMake***
6. set the Project Name to ***repo_name***
7. set the Project location to ***The path where you repo is***. The cube ide doesn't allow relative paths, so you set something incorrectly have to copy the project ___*.ioc___ file to the repo and open it from there so the project location will be set correctly automatically.
8. Save the project and generate the code using the GENERATE CODE button
9. In you repo folder there should be bunch of new folders and files like: 
- CMakeLists.txt 
- cmake/ 
- Core/ 
- Drivers/
10. Now the project is configured to be built with CMake, However the C++ language is not enabled
and we still have to add our ilbraries and source files to the project.

## Adding C++ support
1. In cmake/stm32cubemx/CMakeLists.txt find main.c and replace it with main.cpp
2. in main.cpp add the following code:
```cpp
// in include section
#include "src/main_prog.hpp"

// in main function bofore while(1)
main_prog();
```
3. the CMakeLists.txt file is configured for the C++, copy of this file is CMakeListsC++_copy.cmake in case you overwrite it by mistake
4. Since the StmEpic library requires interface provided by the HAl library. if you generate minimial projecty you might not be able to build it, since not all HAL library components are enabled. To fix this you have to enable the HAL library in the ***Core/Inc/stm32f4xx_hal_conf.h*** file (or somethin similar). The file should look like this:
```c
  /* #define HAL_CRYP_MODULE_ENABLED */
/* #define HAL_ADC_MODULE_ENABLED */
#define HAL_CAN_MODULE_ENABLED
/* #define HAL_CRC_MODULE_ENABLED */
/* #define HAL_CAN_LEGACY_MODULE_ENABLED */
/* #define HAL_HASH_MODULE_ENABLED */
#define HAL_I2C_MODULE_ENABLED   // This might need to be enabled manually
/* #define HAL_I2S_MODULE_ENABLED */
/* #define HAL_IWDG_MODULE_ENABLED */
/* #define HAL_LTDC_MODULE_ENABLED */
/* #define HAL_RNG_MODULE_ENABLED */
/* #define HAL_RTC_MODULE_ENABLED */
/* #define HAL_SAI_MODULE_ENABLED */
/* #define HAL_SD_MODULE_ENABLED */
/* #define HAL_MMC_MODULE_ENABLED */
#define HAL_SPI_MODULE_ENABLED // This might need to be enabled manually
#define HAL_TIM_MODULE_ENABLED // This might need to be enabled manually
#define HAL_UART_MODULE_ENABLED // This might need to be enabled manually
//...

```
5. Now you should be able to build the project with CMake
```bash
cmake -B build -G Ninja
cmake --build build
```

## Adding source files and libraries
there are section in the CMakeLists.txt file where you can add your source files and libraries.

## CAN communication with Nomad or Ariadna
To generate the code for the CAN communication check this guide
[CAN communication guide](src/can_constants/README.md). \\
Quick gide is to run the following command.
```bash
src/can_constants/./generate-files.sh
```

and add directory and source file to the CMakeLists.txt file.
```bash
src/can_constants/can_messages/output
src/can_constants/can_messages/output/can.c
```






