/**
 ** \file
 **
 ** \brief Main PSP Configuration File for GRP740
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** This file includes most of the PSP configuration
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef CFE_PSP_TIMER_H
#define CFE_PSP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_gr740vx69_platcfg PSP Platform Configurations - GR740-VxWorks6.9
** \{
*/

/**
 ** \brief Default Timer pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Timer API.
*/
#define TIMER_PRINT_SCOPE       "PSP Timer: "

/**
 ** \brief Initializes the vxWorks Aux and TimeStamp timers
 **
 ** \par Description:
 ** Function sets up a 64 bit timer. The timer value will be used for the 
 ** function #vxTimeBaseGet.
 **  
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_InitLocalTime(void);

/**
 ** \brief Reads value of VxWorks timer
 **
 ** \par Description:
 ** Function replicates the behaviour of the VxWorks function vxTimeBaseGet,
 ** which is missing from the GR740 BSP.
 **  
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 ** \param Tbu - Upper part of the 64 bit timer 
 ** \param Tbl - Lower part of the 64 bit timer
 **
 ** \return None
 */
void vxTimeBaseGet(uint32 *Tbu, uint32 *Tbl);

/**
** \} <!-- End of group "psp_gr740vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_TIMER_ */