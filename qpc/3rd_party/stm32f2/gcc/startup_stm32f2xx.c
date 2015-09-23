//***************************************************************************
// startup.c - Boot code for STM32F10x Arm Cortex-M3 microcontrollers
// Copyright (c) 2007-2008 Bilkon Bilgisayar Kontrollu Cihazlar Ltd.Sti
// Ahmet Cihan AKINCA
//***************************************************************************
#include <stddef.h>

//***************************************************************************
// Forward declaration of the default fault handlers.
//***************************************************************************
static void ResetISR( void );
static void NmISR( void );
static void FaultISR( void );
static void IntDefaultHandler( void );

//***************************************************************************
// The entry point for the application.
//***************************************************************************
extern int main( void );
extern void SysTick_Handler(void);

//***************************************************************************
// Reserve space for the system stack.
//***************************************************************************
#ifndef STACK_SIZE
#define STACK_SIZE                              (( size_t )128 )
#endif

__attribute__ ((section(".stack"))) unsigned long pulStack[STACK_SIZE];

//***************************************************************************
// The minimal vector table for STM32F103x Cortex-M3.
//***************************************************************************
__attribute__ ((section(".vector_table"))) void (* const stmVectors[])(void) = {
    ( void (*)( void ))(( unsigned int )pulStack + sizeof( pulStack )),	// ()				The pop address of the STACK
    ResetISR,              			// Reset Handler
    NmISR,                			// NMI Handler
    FaultISR,          				// Hard Fault Handler
    IntDefaultHandler,          	// MPU Fault Handler
    IntDefaultHandler,          	// Bus Fault Handler
    IntDefaultHandler,         		// Usage Fault Handler
    0,                         		// Reserved
    0,                          	// Reserved
    0,                          	// Reserved
    0,                          	// Reserved
    IntDefaultHandler,             	// SVCall Handler
    IntDefaultHandler,           	// Debug Monitor Handler
    0,                          	// Reserved
    IntDefaultHandler,             	// PendSV Handler
    SysTick_Handler,            	// SysTick Handler
    IntDefaultHandler,             	// Window WatchDog
    IntDefaultHandler,             	// PVD through EXTI Line detection
    IntDefaultHandler,             	// Tamper and TimeStamps through the EXTI line
    IntDefaultHandler,             	// RTC Wakeup through the EXTI line
    IntDefaultHandler,             	// FLASH
    IntDefaultHandler,             	// RCC
    IntDefaultHandler,             	// EXTI Line0
    IntDefaultHandler,             	// EXTI Line1
    IntDefaultHandler,             	// EXTI Line2
    IntDefaultHandler,             	// EXTI Line3
    IntDefaultHandler,             	// EXTI Line4
    IntDefaultHandler,           	// DMA1 Stream 0
    IntDefaultHandler,           	// DMA1 Stream 1
    IntDefaultHandler,           	// DMA1 Stream 2
    IntDefaultHandler,           	// DMA1 Stream 3
    IntDefaultHandler,           	// DMA1 Stream 4
    IntDefaultHandler,           	// DMA1 Stream 5
    IntDefaultHandler,           	// DMA1 Stream 6
    IntDefaultHandler,             	// ADC1, ADC2 and ADC3s
    IntDefaultHandler,             	// CAN1 TX
    IntDefaultHandler,             	// CAN1 RX0
    IntDefaultHandler,             	// CAN1 RX1
    IntDefaultHandler,             	// CAN1 SCE
    IntDefaultHandler,             	// External Line[9:5]s
    IntDefaultHandler,          	// TIM1 Break and TIM9
    IntDefaultHandler,          	// TIM1 Update and TIM10
    IntDefaultHandler,     			// TIM1 Trigger and Commutation and TIM11
    IntDefaultHandler,          	// TIM1 Capture Compare
    IntDefaultHandler,             	// TIM2
    IntDefaultHandler,             	// TIM3
    IntDefaultHandler,             	// TIM4
    IntDefaultHandler,             	// I2C1 Event
    IntDefaultHandler,             	// I2C1 Error
    IntDefaultHandler,             	// I2C2 Event
    IntDefaultHandler,             	// I2C2 Error
    IntDefaultHandler,             	// SPI1
    IntDefaultHandler,             	// SPI2
    IntDefaultHandler,             	// USART1
    IntDefaultHandler,             	// USART2
    IntDefaultHandler,             	// USART3
    IntDefaultHandler,             	// External Line[15:10]s
    IntDefaultHandler,             	// RTC Alarm (A and B) through EXTI Line
    IntDefaultHandler,            	// USB OTG FS Wakeup through EXTI line
    IntDefaultHandler,         		// TIM8 Break and TIM12
    IntDefaultHandler,          	// TIM8 Update and TIM13
    IntDefaultHandler,     			// TIM8 Trigger and Commutation and TIM14
    IntDefaultHandler,             	// TIM8 Capture Compare
    IntDefaultHandler,           	// DMA1 Stream7
    IntDefaultHandler,             	// FSMC
    IntDefaultHandler,             	// SDIO
    IntDefaultHandler,             	// TIM5
    IntDefaultHandler,             	// SPI3
    IntDefaultHandler,             	// UART4
    IntDefaultHandler,             	// UART5
    IntDefaultHandler,             	// TIM6 and DAC1&2 underrun errors
    IntDefaultHandler,             	// TIM7
    IntDefaultHandler,           	// DMA2 Stream 0
    IntDefaultHandler,           	// DMA2 Stream 1
    IntDefaultHandler,           	// DMA2 Stream 2
    IntDefaultHandler,           	// DMA2 Stream 3
    IntDefaultHandler,           	// DMA2 Stream 4
    IntDefaultHandler,             	// Ethernet
    IntDefaultHandler,             	// Ethernet Wakeup through EXTI line
    IntDefaultHandler,             	// CAN2 TX
    IntDefaultHandler,             	// CAN2 RX0
    IntDefaultHandler,             	// CAN2 RX1
    IntDefaultHandler,             	// CAN2 SCE
    IntDefaultHandler,             	// USB OTG FS
    IntDefaultHandler,           	// DMA2 Stream 5
    IntDefaultHandler,           	// DMA2 Stream 6
    IntDefaultHandler,           	// DMA2 Stream 7
    IntDefaultHandler,             	// USART6
    IntDefaultHandler,             	// I2C3 event
    IntDefaultHandler,             	// I2C3 error
    IntDefaultHandler,         		// USB OTG HS End Point 1 Out
    IntDefaultHandler,          	// USB OTG HS End Point 1 In
    IntDefaultHandler,            	// USB OTG HS Wakeup through EXTI
    IntDefaultHandler,             	// USB OTG HS
    IntDefaultHandler,             	// DCMI
    IntDefaultHandler,             	// CRYP crypto
    IntDefaultHandler              	// Hash and Rng
};
//***************************************************************************
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// ".data" segment resides immediately following the ".text" segment.
//***************************************************************************
extern unsigned int _etext;
extern unsigned int _sdata;
extern unsigned int _edata;
extern unsigned int _sbss;
extern unsigned int _ebss;

//***************************************************************************
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied main() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//*****************************************************************************
static void ResetISR( void ) {
    unsigned int *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_etext;
    for(pulDest = &_sdata; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &_sbss; pulDest < &_ebss; )
    {
        *pulDest++ = 0;
    }

    //
    // Call the application's entry point.
    //
    main();
}

//***************************************************************************
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//***************************************************************************
static void NmISR( void ) {
    while( 1 );
}

//***************************************************************************
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//***************************************************************************
static void FaultISR( void ) {
    while( 1 );
}

//***************************************************************************
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//***************************************************************************
static void IntDefaultHandler( void ) {
    while( 1 );
}

//***************************************************************************
//	End of startup code for STM32F103x ARM Cortex-M3 microprocessor
//***************************************************************************
