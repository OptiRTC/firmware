reset_handler = main;
ENTRY( reset_handler );


MEMORY
{
    DCT1_FLASH                 (rx)  : ORIGIN = 0x08004000,     LENGTH = 16K
    DCT2_FLASH                 (rx)  : ORIGIN = 0x08008000,     LENGTH = 16K
    APP_FLASH                  (rx)  : ORIGIN = 0x08040000,     LENGTH = 128K

    /* The SRAM area is offset to include the stack and static ram allocated at the end of the SRAM area.
       This is done so that the build outputs meaningful values for BSS and Data section sizes.

       The value given here is the sum of system_static_ram_size and stack_size
    */
    SRAM      (rwx) : ORIGIN = 0x20020000 - 40K, LENGTH = 40K
}


/* Define the static locations of the part1 module so the dynamic invoke functions have the base address of each library table. */
INCLUDE module_system_part1_export.ld
/* Definitions for the stack size */
INCLUDE module_system_part2_export.ld
/* Definitions for the static parts of the user module */
INCLUDE module_user_export.ld


/* default stack sizes.
 + These are used by the startup in order to allocate stacks for the different modes.
 */

__Stack_Size = 2048 ;

PROVIDE ( _Stack_Size = __Stack_Size ) ;

__Stack_Init = _estack - __Stack_Size ;

/* "PROVIDE" allows to easily override these values from an object file or the commmand line.
 */
PROVIDE ( _Stack_Init = __Stack_Init ) ;

/*
There will be a link error if there is not this amount of RAM free at the end.
*/
_Minimum_Stack_Size = 0x1400;

/* include the memory spaces definitions sub-script */
/* Linker subscript for STM32F20x definitions with 1024K Flash and 128K RAM */

/* Memory Spaces Definitions */

/* higher address of the user mode stack */
_estack = ORIGIN(SRAM)+LENGTH(SRAM);

SECTIONS
{
    INCLUDE module_start.ld

    .vectors :
    {
        link_interrupt_vectors_location = .;
        KEEP(*(.interrupt_vector_table))            /* interrupt vector table */
        link_interrupt_vectors_location_end = .;
    }>APP_FLASH  AT> APP_FLASH

    interrupt_vectors_length = link_interrupt_vectors_location_end - link_interrupt_vectors_location;

    INCLUDE module_info.ld

    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text.irq ))
        *(.text .text.* .gnu.linkonce.t.*)
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);
        _etext = .;
    }>APP_FLASH  AT> APP_FLASH

    /*
     * The .ARM.exidx and .ARM.extab sections are used for C++ exception handling.
     * It is located here for completeness. Bare-metal ARM projects
     * typically cannot afford the overhead associated with C++
     * exceptions handling.
     */
    .ARM.exidx :
    {
        __exidx_start = ALIGN(4);
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > APP_FLASH  AT> APP_FLASH

    .ARM.extab :
    {
        __extab_start = ALIGN(4);
        *(.ARM.extab*)
        __extab_end = .;
    } > APP_FLASH  AT> APP_FLASH

    .ram_vectors : /* Ram VTOR table - must be 512 byte-aligned */
    {
        link_ram_interrupt_vectors_location = .;
        . = . + interrupt_vectors_length;
        link_ram_interrupt_vectors_location_end = .;
    }> SRAM

    .fast : /* This section contains code that is run from RAM after being loaded from flash - functions can be put in this section with the C attribute: __attribute__ ((section (".fast"))) */
    {
        link_run_from_ram_code_flash_location = LOADADDR( .fast ); /* This is the location in flash of the code */
        link_run_from_ram_code_ram_location = .;
        *(.fast .fast.* .text.fastcode)
        link_run_from_ram_code_ram_end = .;
    }> SRAM AT> APP_FLASH

    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >APP_FLASH

    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >APP_FLASH

    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(.fini_array*))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >APP_FLASH

    .data : /* Contains the non-zero initialised global variables */
    {
        _sidata = LOADADDR( .data ); /* This is the location in flash of the initial values of global variables */
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _sdata = . ;
        *(.data*)
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _edata = . ;
    }> SRAM AT> APP_FLASH


    .bss : /* Zero initialised memory used for zero initialised variables */
    {
        _sbss = ALIGN(., 4);
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _ebss = . ;
    }> SRAM AT> SRAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );

    .stack : /* Contains the initial stack */
    {
        link_stack_location = ALIGN(., 4);
        *(.stack)
        . = ALIGN(MAX(link_stack_location + __STACKSIZE__ , .), 4);
        link_stack_end = .;
    }> SRAM AT> SRAM

    /DISCARD/ :
    {
        *(.ARM.attributes*)
        *(.comment)
        *(.init)
        *(.preinit)
        *(.fini)
        *(.fini_array)
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.gnu.linkonce.armextab.*)
        *(.v4_bx)
        *(.vfp11_veneer)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.glue*)
    }

    INCLUDE module_end.ld

}

/* Declare libc Heap to start at end of allocated RAM */

PROVIDE( _heap = link_stack_end );

/* End of the heap is top of RAM, aligned 8 byte */

PROVIDE( _eheap = ALIGN( ORIGIN( SRAM ) + LENGTH( SRAM ) - 8, 8 ) );

/* ThreadX aliases */
PROVIDE( __RAM_segment_used_end__ = link_stack_end );
PROVIDE( __tx_free_memory_start = link_stack_end );
PROVIDE( __tx_vectors = link_interrupt_vectors_location );

PROVIDE( dct1_start_addr_loc     = ORIGIN( DCT1_FLASH ));
PROVIDE( dct1_size_loc           = LENGTH( DCT1_FLASH ));
PROVIDE( dct2_start_addr_loc     = ORIGIN( DCT2_FLASH ));
PROVIDE( dct2_size_loc           = LENGTH( DCT2_FLASH ));

PROVIDE( VTOR_Length = interrupt_vectors_length );

ASSERT ( VTOR_Length==388, "Interrupt Vector Table incorrect size" );
ASSERT ( link_interrupt_vectors_location == ORIGIN(APP_FLASH), "Interrupt Vector Table not at ORIGIN(APP_FLASH)" );

ASSERT( link_ram_interrupt_vectors_location == ORIGIN( SRAM ), "RAM Interrupt table should be at start of RAM" );
ASSERT( ( link_ram_interrupt_vectors_location_end - link_ram_interrupt_vectors_location ) == VTOR_Length, "Expected RAM VTOR table to be same length as VTOR Flash" );

ASSERT ( link_module_info_start < link_module_info_end, "module info not linked" );
ASSERT ( link_module_info_suffix_start < link_module_info_suffix_end, "module info suffix not linked" );
ASSERT ( link_module_info_crc_start < link_module_info_crc_end, "module info crc not linked" );

INCLUDE module_asserts.ld
ASSERT ( link_module_info_start - ORIGIN(APP_FLASH) == 388,  "module info not where expected" )