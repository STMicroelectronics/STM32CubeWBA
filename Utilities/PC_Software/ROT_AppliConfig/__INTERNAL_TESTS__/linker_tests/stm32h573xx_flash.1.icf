/*###ICF### Section handled by ICF editor, don't touch! ****/
/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_0.xml" */
/*-Specials-*/

define symbol SECTOR_SIZE = 0x2000;                     /* Size of a FLASH sector */
define symbol CODE_OFFSET = 0x37*SECTOR_SIZE;           /* This define is updated automatically by prebuild command */
define symbol CODE_SIZE = 0x49*SECTOR_SIZE;             /* This define is updated automatically by prebuild command */
define symbol RESERVED_AREA_OFFSET = 0x80*SECTOR_SIZE;  /* This define is updated automatically by prebuild command */
define symbol RESERVED_AREA_SIZE = 0x00*SECTOR_SIZE;    /* This define is updated automatically by prebuild command */
define symbol IMAGE_HEADER_SIZE = 0x400;                /* mcuboot header size */
define symbol TRAILER_MAX_SIZE = 0x800;                 /* max size TLV and trailer */
define symbol UNIT_SIZE = 0x400;                        /* Size of a SRAM unit */
define symbol RAM_S_NS_IF_SIZE = 0x10*UNIT_SIZE;        /* This define is updated automatically by prebuild command */
define symbol RAM_S_END = 0x20068fff;                       /* This define is updated automatically by prebuild command */

define  symbol __ICFEDIT_intvec_start__        = 0x08000000 + CODE_OFFSET + IMAGE_HEADER_SIZE;

/*-Memory Regions-*/
define exported symbol __ICFEDIT_region_ROM_start__               = 0x08000000 + CODE_OFFSET + IMAGE_HEADER_SIZE;
define exported symbol __ICFEDIT_region_ROM_end__                 = 0x08000000 + CODE_OFFSET + CODE_SIZE - TRAILER_MAX_SIZE - 1;
define exported symbol __ICFEDIT_region_ROM_reserved_area_start__ = 0x08000000 + RESERVED_AREA_OFFSET;
define exported symbol __ICFEDIT_region_ROM_reserved_area_end__   = 0x08000000 + RESERVED_AREA_OFFSET + RESERVED_AREA_SIZE;
define exported symbol __ICFEDIT_region_RAM_start__               = 0x20000000;
define exported symbol __ICFEDIT_region_RAM_end__                 = 0x20040000 - RAM_S_NS_IF_SIZE - 1;
define exported symbol __ICFEDIT_region_RAM_2_start__             = RAM_S_END + 1;
define exported symbol __ICFEDIT_region_RAM_2_end__               = 0x2009FFFF;

/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x1000;
define symbol __ICFEDIT_size_heap__   = 0x2000;
/**** End of ICF editor section. ###ICF###*/


define memory mem with size = 4G;
define region ROM_region      = mem:[from __ICFEDIT_region_ROM_start__   to __ICFEDIT_region_ROM_end__];
define region RAM_region      = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];

define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

initialize by copy { readwrite };
do not initialize  { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };

place in ROM_region   { readonly };
place in RAM_region   { readwrite,
                        block CSTACK, block HEAP };
                        