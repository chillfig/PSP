/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_ping.h
*
*   \brief Function declarations for cfe_psp_ping.c
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef _PSP_PING_H_
#define _PSP_PING_H_

/*=======================================================================================
** Include Files
**=======================================================================================*/

#ifdef _VXWORKS_OS_
/* For vxworks, include selectLib for ioctl suport */
#include <selectLib.h>
#include <errnoLib.h>
#else
#include <sys/select.h>
#define __USE_MISC 1
#endif

#include <errno.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
 
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

/*
** cFE includes
*/
#include "common_types.h"
#include <cfe_time_extern_typedefs.h>

/*=======================================================================================
** Macro Definition
**=======================================================================================*/

/**
 ** \brief Define all ICMP Packet Types 
 **/
#define ICMP_ECHOREPLY       0 /* Echo Reply */
#define ICMP_DEST_UNREACH    3 /* Destination Unreachable */
#define ICMP_SOURCE_QUENCH   4 /* Source Quench */
#define ICMP_REDIRECT        5 /* Redirect (change route) */
#define ICMP_ECHO            8 /* Echo Request */
#define ICMP_TIME_EXCEEDED  11 /* Time Exceeded */
#define ICMP_PARAMETERPROB  12 /* Parameter Problem */
#define ICMP_TIMESTAMP      13 /* Timestamp Request */
#define ICMP_TIMESTAMPREPLY 14 /* Timestamp Reply */
#define ICMP_INFO_REQUEST   15 /* Information Request */
#define ICMP_INFO_REPLY     16 /* Information Reply */
#define ICMP_ADDRESS        17 /* Address Mask Request */
#define ICMP_ADDRESSREPLY   18 /* Address Mask Reply	*/

/**
 ** \brief Define ICMP Packet Size 
 **/
#define ICMP_HEADER_SIZE       8
#define ICMP_PACKET_SIZE      64
#define ICMP_RTN_PACKET_SIZE 256
#define IP_MAX_ADDRESS_LEN    64

/**
 ** \brief Define ICMP minimum wait-time for packet in MicroSeconds
 ** Equivalent of 1 mili-second
 **/
#define ICMP_WAIT_TIME_MICRO_SEC 1000

/*=======================================================================================
** Structure Definition
**=======================================================================================*/

/**
 * \brief icmphdr packet structure
 *
 * \par Name:
 *      icmphdr
 * 
 * \par Description:
 * This is a generic structure of an icmp packet. It is modeled after this structure
 * https://sites.uclouvain.be/SystInfo/usr/include/netinet/ip_icmp.h.html
 * 
 */
struct icmphdr
{
    uint8 ucType; /* message type */
    uint8 ucCode; /* type sub-code */
    uint16 usChecksum;
    union
    {
        struct
        {
            uint16	usID;
            uint16	usSequence;
        } echo; /* echo datagram */
        uint32	uiGateway; /* gateway address */
        struct
        {
            uint16	usGlibcReserved;
            uint16	usMTU;
        } frag; /* path mtu discovery */
    } un;
};

/**
 * \brief \tlm_msg Telemetry Structure for Ping Statistics. 
 *
 * \par Name:
 *      PSP_Ping_Stats_t
 * 
 */
typedef struct
{
    char               IPAddrName[IP_MAX_ADDRESS_LEN];
    CFE_TIME_SysTime_t roundTripTime;
    uint16             usIdent;
    uint8              usType;
    uint8              ucPadding;
} PSP_Ping_Stats_t;

/*=======================================================================================
** Type Definitions
**=======================================================================================*/

/**
 * \brief General Command Response Enumeration
 *
 * \par This contains the enumerations for General Command Response code. The source of
 *      definition for these values is in section "Command Response and Log Entries" from GP10098.
 */
typedef enum
{
    PSP_PING_IGNORE               =  2,
    PSP_PING_SUCCESS              =  0,
    PSP_PING_INVALID_PKT_TYPE_ERR = -2,
    PSP_PING_UNKNOWN_HOST_ERR     = -3,
    PSP_PING_SOCKET_ERR           = -4,
    PSP_PING_CRT_PKT_ERR          = -5,
    PSP_PING_SND_PKT_ERR          = -6,
    PSP_PING_SELECT_ERR           = -7,
    PSP_PING_TIMEOUT_ERR          = -8,
    PSP_PING_RCV_PKT_ERR          = -9,
    PSP_PING_INVALID_ARG_ERR      = -10
} PSP_Ping_Result_t;

/*=======================================================================================
** API Definitions
**=======================================================================================*/
/**
 ** \brief Return the Packet Type
 **
 ** \par Description:
 ** Function used to get the Packet Type
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] ucIcmpType - the enumerated value of ucIcmpType
 **
 ** \return A character array of the packet type.
 */
const char *CFE_PSP_GetPacketTypeName(const uint8 ucIcmpType);

/**
 * \brief PSP Single Ping API
 *
 * \par Description: Sends and waits for a singular response from an ICMP packet.
 * The function will also calculate the round trip time that it took the packet
 * to return.
 *
 * \par Assumptions, External Events, and Notes:
 * This function uses raw sockets, which interacts directly with the lowest level
 * network protocol. Furthermore each raw socket receives its own copy of the ICMP Packets,
 * so even if multiple raw sockets are created the user does not have to worry about other sockets
 * ingesting and dropping a ICMP packet designated for another socket.
 * 
 * \param[in] pHostIP - The destination IP that we are pinging
 * \param[in] usTimeoutMS - Timeout value in miliseconds
 * \param[in,out] pPingStats - Pointer to Ping Statistic Packet
 *  
 * \retval PSP_PING_SUCCESS | OS_SUCCESS
 * \retval PSP_PING_INVALID_ARG_ERR
 * \return PSP_PING_UNKNOWN_HOST_ERR
 * \return PSP_PING_SOCKET_ERR
 * \return PSP_PING_CRT_PKT_ERR
 * \return PSP_PING_SND_PKT_ERR
 * \return PSP_PING_SELECT_ERR
 * \return PSP_PING_TIMEOUT_ERR
 * \return PSP_PING_RCV_PKT_ERR
 */
PSP_Ping_Result_t CFE_PSP_SinglePing(const char *pHostIP, const uint16 usTimeoutMS,
                                     PSP_Ping_Stats_t *pPingStats);

#endif /* _PSP_PING_H_ */
