# SP0 implementation of PSP

This is the PSP implementation for Aitch SP0 and SP0s targets flight computers

> **NOTE:** Aitech SP0 and SP0s do not support User Reserved Memory nor Persistente Memory. 

[[_TOC_]]

___

## VxWorks Kernel Requirements


### Functions

Kernel must provide the following functions:
- GetWrsKernelTextStart()
- GetWrsKernelTextEnd()

#### Not included - from MCP750
- startCfeCore() - _"This function is used to unzip, load, and run CFE core"_

> **Note:** This function is not necessary during development time, but could be useful in production. The _startCfeCore_ function could load CFE on startup.

- CFE_PSP_InitFlashDisk() - _"Initialize the Compact flash disk in vxWorks"_

### Components
Kernel must include the following services:
- FTP
  - Port 21
- Telnet
- User Reserved Memory (Not Supported by Aitech)
  - Why CLEAR_USER_RESERVED_MEMORY_ON_COLD_BOOT in Kernel is set to True?
- Watchdog
- NTP
- Datalight FlashFX (Aitech FLASH Memory)

___
