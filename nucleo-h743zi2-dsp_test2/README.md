Using STM32 DSP Core
====================

Benchmark
---------

### Saturday, July 2, 2022, Yong Da
- I have a vector\*vector dot product speed test working on the MCU using the arm_math.h and hardware accelerated functions
 
- parameters:
    - generate 2 vectors
    - size  50,000 elements
    - type  float_32
    - elem  values between 0.00 - 0.99

- Note that it's the RNG process that's actually taking up most of the compute time
- takes about 25s (manually counted) for the MCU to do this computation 100x

- laptop_benchmark.c takes about 0.085s to do the same thing.... ripppp

Setup
-----
There are included ARM libraries provided as part of STM32 DSP that can accelerate matrix operations
Based on CMSIS
The Common Microcontroller Software Interface Standard (CMSIS) is a vendor-independent abstraction layer for microcontrollers that are based on Arm Cortex processors.
CMSIS source files are included in the FW package
- C:\Users\yongd\STM32Cube\Repository\STM32Cube_FW_H7_V1.10.0\Drivers\CMSIS\DSP

This is the contents of the `DSP` folder.
```
├── DSP_Lib_TestSuite
│   ├── Common
│   ├── DspLibTest_FVP
│   ├── DspLibTest_FVP_A5
│   ├── DspLibTest_MPS2
│   ├── DspLibTest_SV_FVP
│   ├── DspLibTest_SV_MPS2
│   ├── DspLibTest_SV_Simulator
│   ├── DspLibTest_Simulator
│   └── RefLibs
├── Examples
│   └── ARM
├── Include
├── Lib
│   ├── ARM
│   ├── GCC
│   └── IAR
├── Projects
│   ├── ARM
│   ├── GCC
│   └── IAR
├── PythonWrapper
│   └── cmsisdsp_pkg
└── Source
    ├── BasicMathFunctions
    ├── CommonTables
    ├── ComplexMathFunctions
    ├── ControllerFunctions
    ├── FastMathFunctions
    ├── FilteringFunctions
    ├── MatrixFunctions
    ├── StatisticsFunctions
    ├── SupportFunctions
    └── TransformFunctions
```

Setup
-----
Follow here: https://community.st.com/s/article/configuring-dsp-libraries-on-stm32cubeide


1. You have to copy the `Include` and `Lib` folder into your new STM32CubeIDE project directory.
2. update the GCC compile settings to have the `Include` folder in the include path
3. update the GCC linker setting to include the `Lib\GCC` folder.
4. add the debug symbol for the ARM processor you're using.

See the pictures folder.
![pictures](pictures/gcc_compiler_include.png)
![pictures](pictures/gcc_compiler_preprocess.png)
![pictures](pictures/gcc_linker.png)


Deviation from setup
--------------------
You have to rename the `Lib\GCC\libarm_cortexM7lfsp_math.a` file to `arm_cortexM7lfsp_math.lib`. The linker is expecting to link .lib file. So you'll have to explicitly re-name it. Also you do not need to pre-fix "lib" to the file (aka `libarm....`). It seems that STM32CubeIDE does this itself. 

![pictures](pictures/rename_library.png)



Printing floats
---------------
- https://forum.digikey.com/t/easily-use-printf-on-stm32/20157
- need to enable "Use float with printf..." from project properties > C/C++ build > settings > MCU settings.
- `printf("result: %.1f\n", result);`
- output: `result: 14.0`



```
.
├── Core
│   ├── Inc
│   ├── Src
│   └── Startup
├── Debug
│   ├── Core
│   │   ├── Src
│   │   └── Startup
│   └── Drivers
│       └── STM32H7xx_HAL_Driver
└── Drivers
    ├── CMSIS
    │   ├── DSP
    │   ├── Device
    │   ├── Include
    │   └── Lib
    └── STM32H7xx_HAL_Driver
        ├── Inc
        └── Src

19 directories
```