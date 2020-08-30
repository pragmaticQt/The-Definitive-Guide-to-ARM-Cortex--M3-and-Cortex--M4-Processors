
set OPTIONS_ARCH=-mthumb -mcpu=cortex-m4
set OPTIONS_OPTS=-Os   
set OPTIONS_COMP=-g -Wall
set OPTIONS_LINK=-Wl,--gc-sections,-Map=map.rpt,-lgcc,-lc,-lnosys -ffunction-sections -fdata-sections
set SEARCH_PATH=CMSIS\Include
set LINKER_SCRIPT=gcc.ld
set LINKER_SEARCH="C:\Program Files (x86)\GNU Tools ARM Embedded\4.7 2012q4\share\gcc-arm-none-eabi\samples\ldscripts"

rem Newlib-nano feature is available for v4.7 and after 
rem set OPTIONS_LINK=%OPTIONS_LINK% --specs=nano.specs

rem Compile the project
arm-none-eabi-gcc                         ^
   %OPTIONS_COMP% %OPTIONS_ARCH%          ^
   %OPTIONS_OPTS%                         ^
   -I %SEARCH_PATH% -T %LINKER_SCRIPT%    ^
   -L %LINKER_SEARCH%            ^
   %OPTIONS_LINK%                ^
   startup_stm32f4xx.S           ^
   blinky.c                      ^
   system_stm32f4xx.c            ^
   -o blinky.elf
if %ERRORLEVEL% NEQ 0 goto end

rem Generate disassembled listing for debug/checking
arm-none-eabi-objdump -S blinky.elf > list.txt
if %ERRORLEVEL% NEQ 0 goto end

rem Generate binary image file
arm-none-eabi-objcopy -O binary blinky.elf blinky.bin
if %ERRORLEVEL% NEQ 0 goto end

rem Generate Hex file (Intel Hex format)
arm-none-eabi-objcopy -O ihex   blinky.elf blinky.hex
if %ERRORLEVEL% NEQ 0 goto end


rem Generate Hex file (Verilog Hex format)
arm-none-eabi-objcopy -O verilog   blinky.elf blinky.vhx
if %ERRORLEVEL% NEQ 0 goto end

:end