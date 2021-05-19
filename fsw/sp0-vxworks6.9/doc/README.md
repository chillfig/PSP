# SP0 implementation of PSP

This is the PSP implementation for Aitch SP0 and SP0s targets flight computers

[[_TOC_]]

___

## VxWorks Kernel Requirements

Kernel must provide the following functions:
- GetWrsKernelTextStart
- GetWrsKernelTextEnd 
- 

Kernel must include the following services:
- FTP
  - Port 21
- Telnet
- User Reserved Memory
  - Why CLEAR_USER_RESERVED_MEMORY_ON_COLD_BOOT in Kernel is set to True?
- Watchdog
- NTP
- Datalight FlashFX (Aitech FLASH Memory)

___
