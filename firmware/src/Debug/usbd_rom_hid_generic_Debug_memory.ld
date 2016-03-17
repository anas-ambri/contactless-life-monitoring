/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC4330
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.0.0 [Build 526] [2015-11-23]  on Mar 15, 2016 3:42:01 PM
 */

MEMORY
{
  /* Define each memory region */
  RamLoc128 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x20000 /* 128K bytes (alias RAM) */  
  RamLoc72 (rwx) : ORIGIN = 0x10080000, LENGTH = 0x12000 /* 72K bytes (alias RAM2) */  
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes (alias RAM3) */  
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16K bytes (alias RAM4) */  
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16K bytes (alias RAM5) */  
  SPIFI (rx) : ORIGIN = 0x14000000, LENGTH = 0x400000 /* 4M bytes (alias Flash) */  
}

  /* Define a symbol for the top of each memory region */
  __base_RamLoc128 = 0x10000000  ; /* RamLoc128 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc128 = 0x10000000 + 0x20000 ; /* 128K bytes */  
  __top_RAM = 0x10000000 + 0x20000 ; /* 128K bytes */  
  __base_RamLoc72 = 0x10080000  ; /* RamLoc72 */  
  __base_RAM2 = 0x10080000 ; /* RAM2 */  
  __top_RamLoc72 = 0x10080000 + 0x12000 ; /* 72K bytes */  
  __top_RAM2 = 0x10080000 + 0x12000 ; /* 72K bytes */  
  __base_RamAHB32 = 0x20000000  ; /* RamAHB32 */  
  __base_RAM3 = 0x20000000 ; /* RAM3 */  
  __top_RamAHB32 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM3 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __base_RamAHB16 = 0x20008000  ; /* RamAHB16 */  
  __base_RAM4 = 0x20008000 ; /* RAM4 */  
  __top_RamAHB16 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __top_RAM4 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __base_RamAHB_ETB16 = 0x2000c000  ; /* RamAHB_ETB16 */  
  __base_RAM5 = 0x2000c000 ; /* RAM5 */  
  __top_RamAHB_ETB16 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
  __top_RAM5 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
  __base_SPIFI = 0x14000000  ; /* SPIFI */  
  __base_Flash = 0x14000000 ; /* Flash */  
  __top_SPIFI = 0x14000000 + 0x400000 ; /* 4M bytes */  
  __top_Flash = 0x14000000 + 0x400000 ; /* 4M bytes */  
