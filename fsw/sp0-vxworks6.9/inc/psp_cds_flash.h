/**
 ** \file  psp_cds_flash.h
 **
 ** \brief API header to save and restore CDS in FLASH memory
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved.\n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** None
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef _PSP_CDS_FLASH_H_
#define _PSP_CDS_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
** \func Set the CDS reading method
**
** \par Description:
** This function set the CDS reading method(use CRC, always read from Flash,
** or trust the CDS reserved memory in RAM is correct.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] ucMethod - Reading method
**
** \return None
*/
void CFE_PSP_SetReadCDSMethod(uint8 ucMethod);

/**
** \func Get the CDS reading method
**
** \par Description:
** This function get the CDS reading method(use CRC, always read from Flash,
** or trust the CDS reserved memory in RAM is correct.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return #CFE_PSP_CDS_READ_METHOD_RAM - Trust the CDS data on RAM (no CRC or read from Flash)
** \return #CFE_PSP_CDS_READ_METHOD_CRC - Check the CRC first then read from Flash if CRC mis-matched
** \return #CFE_PSP_CDS_READ_METHOD_FLASH - Always read from Flash
*/
uint8 CFE_PSP_GetReadCDSMethod();

/**
** \func Change the previous calculated CRC value to new provided value
**
** \par Description:
** This function change the previous calculated CRC value to new provided value.
** This function is just for testing purpose by forcing the CRC mismatched and 
** read CDS data from Flash.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param uiNewCRC - New CRC
**
** \return None
*/
void CFE_PSP_SetStaticCRC(uint32 uiNewCRC);

/**
** \func Get the previous calculated CRC value
**
** \par Description:
** None
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return Calculated CRC value
*/
uint32 CFE_PSP_GetStaticCRC(void);

/**
 ** \func Calculate 16 bits CRC from input data
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** InputCRC allows the user to calculate the CRC of non-contiguous blocks as a
 ** single value. Nominally, the user should set this value to zero.
 **
 ** CFE now includes a function to calculate the CRC.
 ** uint32 CFE_ES_CalculateCRC(void *pData, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);
 ** Only CFE_MISSION_ES_CRC_16 is implemented as the TypeCRC
 **
 ** \param[inout] DataPtr  - Pointer to the input data buffer
 ** \param[inout] DataLength  - Data buffer length
 ** \param[inout] InputCRC  - A starting value for use in the CRC calculation.
 **
 ** \return Calculated CRC value
 */
uint32 CFE_PSP_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC);

/**
** \func Read the whole CDS data from Flash
**
** \par Description:
** This function read the whole CDS data on Flash to reserved memory on RAM.
**
** \warning It took about 117ms to read 131072 bytes (128KB) whole CDS area from Flash. 
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param puiReadBytes - Number of read bytes
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_ReadCDSFromFlash(uint32 *puiReadBytes);

/**
** \func Write the whole CDS data on Flash
**
** \par Description:
** This function write the whole CDS data from reserved memory on RAM to Flash.
**
** \par Assumptions, External Events, and Notes:
** It took about 117ms to write 131072 bytes (128KB) whole CDS data to Flash.
**
** \param puiWroteBytes - Number of written bytes
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_WriteCDSToFlash(uint32 *puiWroteBytes);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_CDS_FLASH_H_ */