The Definitive Guide to the ARM Cortex-M3 and Cortex-M4 Processor, 3rd Edition
Joseph Yiu

---------------------------------------------
        Example projects 
---------------------------------------------
Note:
Assume the files are installed as C:\Book\ch_*
Project examples for chapter 22 might need adjustment to CMSIS-DSP library path 
and ST library file locations.


Chapter 10
--------------
ch_10_context_switching_1   1st Context switching example for section 10.5,
                            no FPU support 
ch_10_context_switching_2   2nd Context switching example for section 10.5,
                            with FPU support 


Chapter 11
--------------
ch_11_mpu_simple_eg_1       Section 11.3, Simple example of setting up MPU
ch_11_mpu_simple_eg_2       Section 11.3, MPU region number merged into RBAR
ch_11_mpu_loop_unroll       Section 11.3, Unroll MPU configuration loop for faster setup time
ch_11_mpu_alias             Section 11.3, MPU configuration using alias register
ch_11_mpu_flexible_mpu_functions
                            Section 11.3, MPU configuration using generic MPU functions

Chapter 12
--------------
ch_12_hardfault_handler     Section 12.8, HardFault Handler example 
ch_12_stack_switch_1        Section 12.9.1, 1st example of running system with two stacks
                            Stack setup in reset handler.
ch_12_stack_switch_2        Section 12.9.1, 2nd example of running system with two stacks
                            Stack swtich in C

Chapter 15
--------------
ch_15_blinky_1_stm32f4      Section 15.3, getting started with Keil MDK
ch_15_blinky_2_lpc4300      Section 15.7, running program from SRAM

Chapter 16
--------------
ch_16_blinky_stm32_1        Section 16.3, getting started with IAR EWARM

Chapter 17
--------------
ch_17_blinky_stm32_1        Section 17.3, Example project with gcc
ch_17_blinky_stm32_2_MDK    Section 17.6, Example of compiling with gcc within Keil MDK
ch_17_blinky_stm32_3_CoIDE  Section 17.7, Example of compiling with gcc using Coocox CoIDE

Chapter 18
--------------
ch_18_hello_world_1_MDK     Section 18.2.2, example of using ITM to generate printf message with Keil MDK
ch_18_hello_world_2_MDK     Section 18.2.2, example of bidirectional data transfer using ITM
ch_18_hello_world_3_IAR     Section 18.2.3, example of bidirectional data transfer using ITM with IAR EWARM
ch_18_hello_world_4_semihosting_IAR
                            Section 18.3, example of using semi-hosting with IAR EWARM for printf messages
ch_18_hello_world_5_CoIDE   Section 18.3, example of using semi-hosting with CoIDE for printf messages
ch_18_uart_demo_6_MDK       Section 18.4, example of using UART for bidirectional message transfers
ch_18_lcd_demo_7_MDK        Section 18.4, example of using character LCD module for printf message (KEIL MDK)
ch_18_lcd_demo_8_IAR        Section 18.4, example of using character LCD module for printf message (IAR EWARM)

Chapter 19
--------------
ch_19_rtx_eg_1_simple_two_threads
                            Section 19.3.1, simple CMSIS-RTOS example with two threads
ch_19_rtx_eg_2_signal_event
                            Section 19.3.3, CMSIS-RTOS event communication example
ch_19_rtx_eg_3_mutex        Section 19.3.4, CMSIS-RTOS mutex example
ch_19_rtx_eg_4_semaphore    Section 19.3.5, CMSIS-RTOS semaphore example
ch_19_rtx_eg_5_message_queue
                            Section 19.3.6, CMSIS-RTOS message queue example
ch_19_rtx_eg_6_mail_queue
                            Section 19.3.7, CMSIS-RTOS mail queue example
ch_19_rtx_eg_7_memory_pool_w_msg_q
                            Section 19.3.8, CMSIS-RTOS message queue and memory pool example
ch_19_rtx_eg_8_timer        Section 19.3.10, CMSIS-RTOS timer example
ch_19_rtx_eg_9_user_defined_svc
                            Section 19.3.11, example of expanding CMSIS-RTOS SVC services

Chapter 20
--------------
ch_20_asm_eg_1_mdk          Section 20.4.1, Simple assembly project with Keil MDK
ch_20_asm_eg_2_ds5          Section 20.4.1, Simple assembly project with ARM DS-5
ch_20_asm_eg_3_gcc          Section 20.4.2, Simple assembly project with gcc
ch_20_asm_eg_4_hello_mdk    Section 20.4.5, Hello world in assembly
ch_20_asm_eg_5_print_values_mdk
                            Section 20.4.6, various helper functions to display messages
ch_20_asm_eg_6_nvic_ctrl_mdk
                            Section 20.4.7, NVIC helper functions
ch_20_embedded_asm_1_MDK    Section 20.4.8, 20.5.3
                            Square root in embedded assembler in Keil MDK
ch_20_inline_asm_1_MDK      Section 20.5.4, example of inline assembler in Keil MDK

Chapter 22
--------------
ch_22_dtfm                  Section 22.5 Example 1 - DTMF Demodulation
ch_22_ls                    Section 22.6 Example 2 - Least squares motion tracking

Chapter 23
--------------
ch_23_eg_1_conditional_branches
                            Section 23.1.1, various optimized branch condition decisioning
ch_23_eg_2_nonbase_thread_enable
                            Section 23.5, non-base thread enable example
ch_23_eg_3_reentrant_handler
                            Section 23.6, re-entrant handler example
ch_23_eg_4_fpb
                            Section 23.10, Flash Patch example using FPB
                            (Flash Patch and Breakpoint unit)
