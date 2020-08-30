
set OPTIONS_ARCH=-mthumb -mcpu=cortex-m4
set OPTIONS_OPTS=-Os   
set OPTIONS_COMP=-g -Wall
set OPTIONS_LINK=--gc-sections -Map=map.rpt 
set SEARCH_PATH_1=CMSIS\Include
set SEARCH_PATH_2=CMSIS\ST\STM32F4xx\Include
set LINKER_SCRIPT=gcc.ld
set LINKER_SEARCH="C:\Program Files (x86)\GNU Tools ARM Embedded\4.7 2012q4\share\gcc-arm-none-eabi\samples\ldscripts"

rem Newlib-nano feature is available for v4.7 and after 
rem set OPTIONS_LINK=%OPTIONS_LINK% --specs=nano.specs

arm-none-eabi-as %OPTIONS_ARCH% startup_stm32f4xx.s -o startup_stm32f4xx.o
if %ERRORLEVEL% NEQ 0 goto end

arm-none-eabi-as %OPTIONS_ARCH% simple_example.s -o simple_example.o
if %ERRORLEVEL% NEQ 0 goto end

rem Compile the SystemInit
arm-none-eabi-gcc                         ^
   %OPTIONS_COMP% %OPTIONS_ARCH%          ^
   %OPTIONS_OPTS% -c                      ^
   -I %SEARCH_PATH_1% -I %SEARCH_PATH_2%  ^
   system_stm32f4xx.c                     ^
   -o system_stm32f4xx.o
if %ERRORLEVEL% NEQ 0 goto end

rem Link
arm-none-eabi-ld -T %LINKER_SCRIPT% -L %LINKER_SEARCH% ^
   %OPTIONS_LINK% -o simple_example.elf                ^
   system_stm32f4xx.o simple_example.o startup_stm32f4xx.o
if %ERRORLEVEL% NEQ 0 goto end

rem Generate disassembled listing for debug/checking
arm-none-eabi-objdump -S simple_example.elf > list.txt
if %ERRORLEVEL% NEQ 0 goto end

rem Generate binary image file
arm-none-eabi-objcopy -O binary simple_example.elf simple_example.bin
if %ERRORLEVEL% NEQ 0 goto end

rem Generate Hex file (Intel Hex format)
arm-none-eabi-objcopy -O ihex   simple_example.elf simple_example.hex
if %ERRORLEVEL% NEQ 0 goto end


rem Generate Hex file (Verilog Hex format)
arm-none-eabi-objcopy -O verilog simple_example.elf simple_example.vhx
if %ERRORLEVEL% NEQ 0 goto end

:end