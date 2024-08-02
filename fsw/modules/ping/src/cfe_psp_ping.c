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
*   \file cfe_psp_ping.c
*
*   \brief API for the ping feature, which allow users to send ICMP packets to specific IPs and return 
*   not only about success or failure, but also the structure including some statistics of the packets.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Include Files
**=======================================================================================*/
#include "common_types.h"
#include <cfe_time.h>
#include "cfe_psp_timesync.h"
#include "cfe_psp_ping.h"
#include "cfe_psp_module.h"

/*=======================================================================================
** Static Functions:
** See function definitions for details
**=======================================================================================*/

uint16 CFE_PSP_CalculateICMPChecksum(const uint16 *pData, const uint32 uiLength);

PSP_Ping_Result_t CFE_PSP_GenerateICMPPkt(uint8 *pBuffer, uint16 *pIdent);

PSP_Ping_Result_t CFE_PSP_Process_Packet(uint8 *pBuffer, int iRecvBytes, struct sockaddr_in *pFrom,
                                                const uint16 usIdent, PSP_Ping_Stats_t *pStatPacket);

/*=======================================================================================
** Global Variables
**=======================================================================================*/

/** \brief Track total amount of allocated memory */
static osal_id_t g_uiPingMutex;

/** \brief Tracks identification number for each new ping call created */
uint16 g_usPingIdentNum = 0;

/** \brief Tracks whether mutex was initialized already */
bool g_bInitMutex = false;

CFE_PSP_MODULE_DECLARE_SIMPLE(ping);

/*=======================================================================================
** Static Functions
**=======================================================================================*/

/**
 ** \func Calculate 16 bits CRC from input data
 **
 ** \par Description:
 ** Function used to calculate CRC
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 **
 ** \param[in] pData - Pointer to the input data buffer
 ** \param[in] uiLength  - Total Length of the data
 **
 ** \return Calculated CRC value
 */
uint16 CFE_PSP_CalculateICMPChecksum(const uint16 *pData, const uint32 uiLength)
{
    const uint16 *pDataPtr = pData;
    uint16 usLeftOverSum   = 0;
    uint16 usCRCValue      = 0;
    int iSum               = 0;
    int iLeft              = uiLength;

    if (pData == NULL)
    {
        goto CFE_PSP_CalculateICMPChecksum_Exit_Tag;
    }

    while ( iLeft > 1 )
    {
        iSum += *pDataPtr++;
        iLeft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if ( iLeft == 1 )
    {
        usLeftOverSum = 0;
        *(uint8 *)(&usLeftOverSum) = *(uint8 *)pDataPtr ;
        iSum += usLeftOverSum;
    }

    iSum = (iSum >> 16) + (iSum & 0xffff); /* add hi 16 to low 16 */
    iSum += (iSum >> 16);                  /* add carry */
    usCRCValue = ~iSum;                    /* truncate to 16 bits */

CFE_PSP_CalculateICMPChecksum_Exit_Tag:
    return (usCRCValue);
}

/**
 ** \func Generate the ICMP Packet to return
 **
 ** \par Description:
 ** Function to generate and return the icmp packet's header
 **
 ** \par Assumptions, External Events, and Notes:
 ** We are doing memcpy() operations instead of type casting of pBuffer
 ** causes cast align warning.
 **
 ** \param[in] pBuffer - Pointer to the out_going packet
 ** \param[in] pIdent - Pointer to identification value
 **
 * \retval PSP_PING_SUCCESS
 * \retval PSP_PING_CRT_PKT_ERR
 */
PSP_Ping_Result_t CFE_PSP_GenerateICMPPkt(uint8 *pBuffer, uint16 *pIdent)
{
    PSP_Ping_Result_t retVal     = PSP_PING_CRT_PKT_ERR;
    struct icmphdr *pIcmpHdr     = NULL;
    CFE_TIME_SysTime_t TimeStamp = {0};

    /* We need a copy of the pBuffer pointer to do arithmetic without affecting main packet */
    uint8 *pPacket               = pBuffer;

    /* Check if pBuffer is NULL */
    if (pBuffer == NULL)
    {
        /* Invalid NULL Argument passed */
        OS_printf("CFE_PSP_GenerateICMPPkt(): Invalid Argument.\n");
        goto CFE_PSP_GenerateICMPPkt_Exit_Tag;
    }

    /*
    ** NOTE: The reason we are doing memcpy() operations over direct
    ** type casting of pBuffer is because the compiler complains about a cast align
    ** warning. However, the operation of memcpy is safe because pointers are the same
    ** sizes.
    */

    /* Get access to the icmp header */
    memcpy(&pIcmpHdr, &pBuffer, sizeof(pBuffer));

    /* Modify the icmp header*/
    pIcmpHdr->ucType = ICMP_ECHO;
    pIcmpHdr->ucCode = 0;
    pIcmpHdr->un.echo.usSequence = 0;

    /* Lock the Mutex before assigning identification number */
    if (OS_MutSemTake(g_uiPingMutex) != OS_SUCCESS)
    {
        /* Failed to take Sempahore */
        OS_printf("CFE_PSP_GenerateICMPPkt(): Failed to take semaphore.\n");
        goto CFE_PSP_GenerateICMPPkt_Exit_Tag;
    }

    /* Set the ID value to returned argument, if not NULL */
    if (pIdent != NULL)
    {
        *pIdent = g_usPingIdentNum;
    }

    /* Update identification number and increment count */
    pIcmpHdr->un.echo.usID = g_usPingIdentNum;
    /* Increment the ping ID */
    g_usPingIdentNum++;

    /* Unlock the mutex */
    if (OS_MutSemGive(g_uiPingMutex) != OS_SUCCESS)
    {
        /* Failed to give Semaphore*/
        OS_printf("CFE_PSP_GenerateICMPPkt(): Failed to give semaphore.\n");
        goto CFE_PSP_GenerateICMPPkt_Exit_Tag;
    }

    /* Call the GetOSTime and save it in TimeStamp */
    if (CFE_PSP_GetOSTime(&TimeStamp) != CFE_PSP_SUCCESS)
    {
        /* If we fail getting time, we are returning a NULL to end the call. */
        OS_printf("CFE_PSP_GenerateICMPPkt(): Failed to get time.\n");
        goto CFE_PSP_GenerateICMPPkt_Exit_Tag;
    }

    /* Now copy the time into the payload section of the pPacket */
    pPacket = pPacket + ICMP_HEADER_SIZE;
    memcpy(pPacket, &TimeStamp, sizeof(TimeStamp));

    /* Calculate the Checksum here */
    pIcmpHdr->usChecksum = CFE_PSP_CalculateICMPChecksum((uint16 *)pIcmpHdr, ICMP_PACKET_SIZE);

    retVal = PSP_PING_SUCCESS;

CFE_PSP_GenerateICMPPkt_Exit_Tag:
    return retVal;
}

/**
 * \brief PSP Ping Initialization
 *
 * \par Description:
 *      Initializes the mutex for the Ping API
 *
 * \par Assumptions, External Events, and Notes:
 *      None
 *
 * \param[in] uiPspModuleId - Used when psp module is initializing
 *
 * \retval OS_SUCCESS
 * \return OS_INVALID_POINTER
 * \return OS_ERR_NAME_TOO_LONG
 * \return OS_ERR_NO_FREE_IDS
 * \return OS_ERR_NAME_TAKEN
 * \return OS_SEM_FAILURE
 */
void ping_Init(uint32 uiPspModuleId)
{
    int32 iRetVal = -1;

    /* Check if mutex is already defined */
    if (g_bInitMutex == true)
    {
        goto ping_Init_Exit_Tag;
    }

    iRetVal = OS_MutSemCreate(&g_uiPingMutex, "PING Mutex", 0);
    if (iRetVal != OS_SUCCESS)
    {
        OS_printf("ping_Init() - OS_MutSemCreate() failed: 0x%08X\n", (int)iRetVal);
        goto ping_Init_Exit_Tag;
    }

    /* Successful at initializing mutex */
    g_bInitMutex = true;

ping_Init_Exit_Tag:
    return;
}

/**
 * \brief PSP Process ICMP Packet
 *
 * \par Description:
 *      Process the ICMP packet that was received out of the socket
 *
 * \par Assumptions, External Events, and Notes:
 *  We are doing memcpy() operations instead of type casting of pBuffer
 *  causes cast align warning.
 *
 * \param[in] pBuffer - A packet that was found in one of the sockets.
 * \param[in] from - The sender's address.
 * \param[in] usIdent - Used to Identify which packet ID we are looking for.
 * \param[in,out] pStatPacket - The Statistic Packet that we are sending back to user.
 * 
 * \retval PSP_PING_SUCCESS
 * \retval PSP_PING_INVALID_ARG_ERR
 * \return PSP_PING_INVALID_PKT_TYPE_ERR
 * \return PSP_PING_IGNORE
 * \return CFE_PSP_SUCCESS
 * \return CFE_PSP_ERROR
 */
PSP_Ping_Result_t CFE_PSP_Process_Packet(uint8 *pBuffer, int iRecvBytes, struct sockaddr_in *pFrom,
                                                const uint16 usIdent, PSP_Ping_Stats_t *pStatPacket)
{   
    PSP_Ping_Result_t retVal       = PSP_PING_INVALID_PKT_TYPE_ERR;
    struct ip ipReplyHdr           = {0};
    struct icmphdr icmpReplyHdr    = {0};
    CFE_TIME_SysTime_t pktSentTime = {0};
    CFE_TIME_SysTime_t pktRecvTime = {0};
    uint8    ucIpHeaderLength      = 0;
    uint8    ucIcmpType            = 0;
    uint16   usId                  = 0;

    /* Access the memory for the ip_header */
    memcpy(&ipReplyHdr, pBuffer, sizeof(struct ip));
    ucIpHeaderLength = ipReplyHdr.ip_hl << 2;

    if ((iRecvBytes - ucIpHeaderLength) != ICMP_PACKET_SIZE)
    {
        retVal = PSP_PING_IGNORE;
        goto CFE_PSP_Process_Packet_Exit_Tag;
    }

    /* Access the memory for the icmp packet's header */
    memcpy(&icmpReplyHdr, pBuffer + ucIpHeaderLength, sizeof(struct icmphdr));

    ucIcmpType = icmpReplyHdr.ucType;
    usId = icmpReplyHdr.un.echo.usID;
    /* Check if icmp packet is the right type */
    if (ucIcmpType != ICMP_ECHOREPLY) 
    {
        goto CFE_PSP_Process_Packet_Exit_Tag;
    }
    
    /* Check if the icmp packet has the correct id value */
    if (usId != usIdent)
    {
        retVal = PSP_PING_IGNORE;
        goto CFE_PSP_Process_Packet_Exit_Tag;
    }

    /* Get the time stamp that was set in the icmp packet */
    memcpy(&pktSentTime, (pBuffer + ucIpHeaderLength + ICMP_HEADER_SIZE), sizeof(CFE_TIME_SysTime_t));

    /* Calculate the difference in time */
    if (CFE_PSP_GetOSTime(&pktRecvTime) != CFE_PSP_SUCCESS)
    {
        retVal = PSP_PING_IGNORE;
        goto CFE_PSP_Process_Packet_Exit_Tag;
    }

    pStatPacket->roundTripTime = CFE_TIME_Subtract(pktRecvTime, pktSentTime);
    pStatPacket->usIdent = usId;
    pStatPacket->usType = ucIcmpType;
    snprintf(pStatPacket->IPAddrName, sizeof(pStatPacket->IPAddrName), "%s", inet_ntoa(pFrom->sin_addr));

    retVal = PSP_PING_SUCCESS;

CFE_PSP_Process_Packet_Exit_Tag:
    return retVal;  
}

/*=======================================================================================
** Public PSP API
**=======================================================================================*/
/*----------------------------------------------------------------
 *
 *  Purpose: Implemented per public PSP API
 *           See description in API and header file for detail
 *
 *-----------------------------------------------------------------*/
const char *CFE_PSP_GetPacketTypeName(const uint8 ucIcmpType)
{
    uint8 ucIcmpTypeReturn = ucIcmpType;
    const char *pIcmpNameMap[20] = 
    {
        "Echo Reply",
        "ICMP 1",
        "ICMP 2",
        "Dest Unreachable",
        "Source Quench",
        "Redirect",
        "ICMP 6",
        "ICMP 7",
        "Echo",
        "ICMP 9",
        "ICMP 10",
        "Time Exceeded",
        "Parameter Problem",
        "Timestamp",
        "Timestamp Reply",
        "Info Request",
        "Info Reply",
        "Address",
        "Address Reply",
        "OUT-OF-RANGE"
    };

    /* Check index range */
    if (ucIcmpTypeReturn > 18)
    {
        /* Invalid Value */
        ucIcmpTypeReturn = 19;
    }

    return(pIcmpNameMap[ucIcmpTypeReturn]);
}

/*----------------------------------------------------------------
 *
 *  Purpose: Implemented per public PSP API
 *           See description in API and header file for detail
 *
 *-----------------------------------------------------------------*/
PSP_Ping_Result_t CFE_PSP_SinglePing(const char *pHostIP, const uint16 usTimeoutMS, PSP_Ping_Stats_t *pPingStats)
{
    PSP_Ping_Result_t retVal = PSP_PING_TIMEOUT_ERR;
    uint8 outIcmpPkt[ICMP_PACKET_SIZE] = {0};
    uint8 inIcmpPkt[ICMP_RTN_PACKET_SIZE] = {0};
    struct sockaddr_in dest = {0};
    struct sockaddr_in from = {0};
    struct timeval timeoutValue = {0};
    socklen_t fromLength = sizeof(from);
    fd_set set = {{0}};
    uint16 usPktID = 0;
    int iSockFd = -1;
    int iRecvBytes = 0;
    int iSendBytes = 0;

    /* Variable used to monitor how long we are waiting for a message */
    uint16 usElaspedTimeMS = 0;

    /* Set the timeout value */
    timeoutValue.tv_usec = ICMP_WAIT_TIME_MICRO_SEC;

    if ((pHostIP == NULL) || (pPingStats == NULL))
    {
        retVal = PSP_PING_INVALID_ARG_ERR;
        goto CFE_PSP_SinglePing_Exit_Tag;
    }

    /* Define the destination socket */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr((char *)pHostIP);
    if (dest.sin_addr.s_addr == ((unsigned int) -1))
    {
        retVal = PSP_PING_UNKNOWN_HOST_ERR;
        OS_printf("CFE_PSP_SinglePing() - Unknown host %s\n", pHostIP); 
        goto CFE_PSP_SinglePing_Exit_Tag;
    }

    /*
    ** NOTE: As future work, once OSAL can handle raw sockets and icmp protocols,
    ** we will replace all network library calls with OSAL library functions.
    */

    /* Create the ICMP socket */
    iSockFd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (iSockFd < 0)
    {
        retVal = PSP_PING_SOCKET_ERR;
        OS_printf("CFE_PSP_SinglePing() - Error creating socket\n");
        goto CFE_PSP_SinglePing_Exit_Tag;
    }

    /* Create the ICMP packet */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, &usPktID);
    if (retVal != PSP_PING_SUCCESS)
    {
        close(iSockFd);
        OS_printf("CFE_PSP_SinglePing() - Failed to create the packet\n");
        goto CFE_PSP_SinglePing_Exit_Tag;
    }

    /* Send the ICMP packet */
    iSendBytes = (int)sendto(iSockFd, outIcmpPkt, ICMP_PACKET_SIZE, 0,
                             (struct sockaddr *)&dest, sizeof(struct sockaddr));
    if (iSendBytes < 0)
    {
        retVal = PSP_PING_SND_PKT_ERR;
        close(iSockFd);
        OS_printf("CFE_PSP_SinglePing() - Unable to send ICMP packet to socket. Return Value of %d.\n", iSendBytes);
        goto CFE_PSP_SinglePing_Exit_Tag;
    }
    
    if (iSendBytes != ICMP_PACKET_SIZE)
    {
        retVal = PSP_PING_SND_PKT_ERR;
        close(iSockFd);
        OS_printf("CFE_PSP_SinglePing() - Something is wrong, sent %d bytes instead 64 bytes\n", iSendBytes);
        goto CFE_PSP_SinglePing_Exit_Tag;
    }

    /* This is where we create a blocking function */
    while (usElaspedTimeMS < usTimeoutMS)
    {
        /* Reset Set before calling Select */
        FD_ZERO(&set);
        FD_SET(iSockFd, &set);

        /* Monitor the sockets for new messages */
        retVal = (int)select(iSockFd+1, &set, NULL, NULL, &timeoutValue);
        if (retVal < 0)
        {
            /* There was an Error with the Select Call - Exit Loop */
            retVal = PSP_PING_SELECT_ERR;
            break;
        } 
        else if (retVal == 0)
        {
            /* There were no packets in the socket */
            retVal = PSP_PING_TIMEOUT_ERR;
            usElaspedTimeMS++;
        }
        else
        {
            /* The socket has a message that you can ingest */
            iRecvBytes = (int)recvfrom(iSockFd, inIcmpPkt, ICMP_RTN_PACKET_SIZE, 0,
                                       (struct sockaddr *)&from, &fromLength);
            if (iRecvBytes < 0)
            {
                /* There was an error when trying to receive the icmp packet */
                retVal = PSP_PING_RCV_PKT_ERR;
                break;
            }
            else if (iRecvBytes >= ICMP_PACKET_SIZE)
            {
                /* If you have a successful ping, then you can break the loop */
                retVal = CFE_PSP_Process_Packet(inIcmpPkt, iRecvBytes, &from, usPktID, pPingStats);
                if (retVal == PSP_PING_SUCCESS)
                {
                    break;
                }
            }
            else
            {
                /* The bytes received were lower than the initial icmp packet size */
                retVal = PSP_PING_IGNORE;
            }
        }
    }

    close(iSockFd);

CFE_PSP_SinglePing_Exit_Tag:
    return retVal;
}
