#include <vxWorks.h>
#include <vsbConfig.h>
#include <logLib.h>
#include <spyLib.h>
#include <cpuset.h>
#include <string.h>
#include <private/kernelLibP.h>
#include <private/kernelLockLibP.h>
#include <private/spyLibP.h>

#ifndef _WRS_CONFIG_LP64
#include <private/symLibP.h>
#include <private/cplusLibP.h>
#endif

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

/* Shorthands for SMP kernel vars acces */ 

#ifdef _WRS_CONFIG_SMP
#define INT_COUNT_CPU(cpu)	_WRS_KERNEL_CPU_GLOBAL_GET (cpu, intCnt)
#define IDLE_TASK_CPU(cpu)	_WRS_KERNEL_CPU_GLOBAL_GET (cpu, idleTaskId)
#endif 	/* _WRS_CONFIG_SMP */
#define TID_CURRENT_CPU(cpu)	_WRS_KERNEL_CPU_GLOBAL_GET (cpu, taskIdCurrent)

/* Typedefs */

typedef struct spyCpuData
    {
    UINT    kernelIncTicks;	/* CPU kernel mode ticks since last report */
    UINT    interruptIncTicks;	/* CPU interrupt mode ticks since last report */
    UINT    idleIncTicks;	/* CPU idle mode ticks since last report */
#ifdef _WRS_CONFIG_SMP
    UINT    taskIncTicks;	    /* CPU task mode ticks since last report */
    UINT    kernelTotalTicks;	    /* CPU knl ticks since spy start */
    UINT    interruptTotalTicks;    /* CPU int ticks since spy start */
    UINT    idleTotalTicks;	    /* CPU idle ticks since spy start */
    UINT    taskTotalTicks;	    /* CPU task ticks since spy start */
#endif	/* _WRS_CONFIG_SMP */
    } SPY_CPU_DATA;

typedef struct spy_info
    {
    int		taskIncTicks;	/* task mode ticks since last report */
    int		taskTotalTicks;	/* task ticks since spy start */
    } SPY_INFO;

/* SpyTask parameters */

TASK_ID spyTaskId	= TASK_ID_ERROR; /* ERROR = spy task not active */
int spyTaskOptions	= VX_UNBREAKABLE;
int spyTaskPriority	= 5;

#if defined (_WRS_SPY_TASK_SIZE)
size_t spyTaskStackSize	= _WRS_SPY_TASK_SIZE;
#else	/* !_WRS_SPY_TASK_SIZE */
#error "Required constant _WRS_SPY_TASK_SIZE not set."
#endif	/* !_WRS_SPY_TASK_SIZE */

/* Global variables */

UINT spyTotalTicks;		/* all ticks since start */
UINT spyIncTicks;		/* all ticks since last report */

UINT spyKernelTotalTicks;	/* sum of knl ticks across CPUs since start */
UINT spyInterruptTotalTicks;	/* sum of int ticks across CPUs since start */
UINT spyIdleTotalTicks;		/* sum of idle ticks across CPUs since start */
BOOL spyNoSymbolPrint = FALSE;	/* Enable symbol lookup & printing in spy */

SPY_CPU_DATA * spyCpuTbl;		/* per-CPU metrics table */

#ifdef _WRS_CONFIG_SMP
cpuset_t    spyActiveCpus;		/* active set of CPUs */
#endif	/* _WRS_CONFIG_SMP */

/* Local variables */

LOCAL BOOL spyClkRunning;		/* TRUE = spyClkStart'ed */
LOCAL BOOL spyInitialized;		/* TRUE = hooks installed */
LOCAL int  spyCreateCount;		/* num tasks created since report */
LOCAL int  spyDeleteCount;		/* num tasks deleted since report */
LOCAL int  maxSpyTasks;			/* Max # of tasks that can be spied */
LOCAL SPY_INFO * pSpyInfo;		/* pointer to spy per-task info array */
LOCAL TASK_ID * pIdList;		/* pointer to task ID array */

LOCAL int  auxClkTicksPerSecond;

/* Display formats */

#ifndef	_WRS_CONFIG_LP64
LOCAL const char * spyFmt1 = \
	"%-12.12s %-12.12s %#-10x  %3d   %3d%% (%8d)  %3d%% (%8d)\n";
LOCAL const char * spyFmt2 = \
	"%-12.12s %-12.12s %-10s  %3s  %4d%% (%8d) %4d%% (%8d)\n";
#else	/* _WRS_CONFIG_LP64 */
LOCAL const char * spyFmt1 = \
	"%-12.12s %#-18p  %3d   %3d%% (%10d)  %3d%% (%10d)\n";
LOCAL const char * spyFmt2 = \
	"%-12.12s %-18s  %3s   %3d%% (%10d)  %3d%% (%10d)\n";
#endif	/* _WRS_CONFIG_LP64 */

#ifdef _WRS_CONFIG_SMP
LOCAL const char * spySmpFmt = \
	"%-3u %3u%% (%8u) %3u%% (%8u) %3u%% (%8u) %3u%% (%8u)  %3u%%\n";
#endif	/* _WRS_CONFIG_SMP */

/* Forward static functions */

LOCAL void	spyCreateHook		(WIND_TCB *pTcb);
LOCAL void	spyDeleteHook		(WIND_TCB *pTcb);
LOCAL void	spyClkInt		(void);
LOCAL void	spyTaskIdListSort	(TASK_ID idList [], int nTasks);

CFE_PSP_MODULE_DECLARE_SIMPLE(spylib_fpga);

/**
 ** \func Initialize spylib_fpga module
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] PspModuleId - Not used
 **
 ** \return None
 */
void spylib_fpga_Init(uint32 PspModuleId)
{

}

/*******************************************************************************
*
* spyLibInit - initialize task cpu utilization tool package
*
* This routine initializes the task cpu utilization tool package.
* If the configuration macro INCLUDE_SPY is defined, it is called by the root
* task, usrRoot(), in usrConfig.c.
*
* RETURNS: N/A
*
* SEE ALSO: usrLib
*/

void spyLibInit
    (
    int	spyTasksMax	/* Maximun number of tasks that can be spied */
    )
    {
    _func_spy		= spyCommon;
    _func_spy2		= spyCommon2;
    _func_spyStop	= spyStopCommon;
    _func_spyClkStart 	= spyClkStartCommon;
    _func_spyClkStop 	= spyClkStopCommon;
    _func_spyTask 	= spyComTask;
    _func_spyReport	= spyReportCommon;
    _func_spyReport2	= spyReportCommon2;

    /*
     * Allocate CPU metrics table for all hardcoded CPUs.
     * Allocate arrays used during spying.
     */

    if (((spyCpuTbl = calloc ((size_t)vxCpuConfiguredGet (),
			      sizeof (SPY_CPU_DATA))) == NULL) ||
	((pIdList = calloc ((size_t)spyTasksMax, sizeof (TASK_ID))) == NULL) ||
	((pSpyInfo = calloc ((size_t)spyTasksMax, sizeof (SPY_INFO))) == NULL))
	{
	/* calloc() sets the errno */
	printErr ("spyLib: cannot initialize data collection arrays!\n");
	}

    /* Set the maximun number of tasks that can be spied */

    maxSpyTasks = spyTasksMax;
    }

/*******************************************************************************
*
* spyCreateHook - initialize task tick counters
*
* This routine is installed as a task create hook so that the task tick
* counters can be initialized and spyReport() can indicate when new tasks
* appear between reports.
*
* RETURNS: N/A
*/

LOCAL void spyCreateHook
    (
    WIND_TCB *pTcb      /* WIND_TCB of new task */
    )
    {
    pTcb->taskTicks    = 0;
    pTcb->taskIncTicks = 0;
    spyCreateCount++;
    }

/*******************************************************************************
*
* spyDeleteHook - notify spyLib of task deletion
*
* This routine is installed as a task delete hook so that spyReport()
* can indicate when tasks disappear between reports.
*
* RETURNS: N/A
*
*/

LOCAL void spyDeleteHook
    (
    WIND_TCB *pTcb      /* WIND_TCB of new task */
    )
    {
    spyDeleteCount++;
    }

/*******************************************************************************
*
* spyClkInt - spyLib interrupt service routine
*
* This routine is called at each tick of the auxiliary clock.  When at
* interrupt level the interrupt tick count is incremented.  If there is no
* active task then the idle tick count is incremented, otherwise increment
* the active task's tick counter.
*
* RETURNS: N/A
*/

LOCAL void spyClkInt (void)
    {
    char cMsg1[] = "spyClkInt ERROR: taskIdCurrent is not ready, while "
		    "VxWorks is not in kernel state, not in idle state, not "
		    "handling a nested interrupt!\n";
#ifndef _WRS_CONFIG_SMP
    if (intCount () > 1)
	{
	/* We interrupted an interrupt */

        spyCpuTbl[vxCpuIndexGet ()].interruptIncTicks++;
	}
    else if (IS_KERNEL_IDLE ())
	{
	/* We interrupted the idle state */

        spyCpuTbl[vxCpuIndexGet ()].idleIncTicks++;
	}
    else if (KERNEL_ENTERED_ME ())
	{
	/* We interrupted the kernel */

        spyCpuTbl[vxCpuIndexGet ()].kernelIncTicks++;
	}
    else
        {
        if (taskIsReady ((TASK_ID) TID_CURRENT_CPU (_WRS_CPU_INDEX_GET)))
            taskIdCurrent->taskIncTicks++;
        else
            logMsg (cMsg1,
		    (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0,
		    (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0);
        }

#else /* !_WRS_CONFIG_SMP*/
    UINT    cpuNb;
    UINT    knlCpuIdx;
    int	    cpuIndex;

    /* 
     * Kernel state can be entered by one CPU at a time only so we test this
     * outside the CPU loop. If no CPU owns the lock, the returned index will be
     * -1 and the associated tests will be skipped.
     */

    knlCpuIdx = KERNEL_LOCK_OWNER_GET ();

    /* 
     * Iterate over the CPUs and take note of their state. Note that the
     * measurement could be flawed if task migrate during the measurement (a
     * task execution could be counted twice).
     * 
     * Kernel state is an exclusive state, which is why we exit the loop
     * prematurely. However, we still have to test for the ISR state since it
     * can interrupt the kernel one.
     */

    spyActiveCpus = vxCpuEnabledGet ();

    /* Get current CPU index */

    cpuIndex = _WRS_CPU_INDEX_GET();

    for (cpuNb = 0; cpuNb < vxCpuConfiguredGet (); cpuNb++)
        {
	if (!CPUSET_ISSET (spyActiveCpus, cpuNb))
	    continue;	/* ignore inactive CPUs */

	if (((cpuNb == cpuIndex) && (INT_COUNT_CPU (cpuNb) > 1)) ||
	    ((cpuNb != cpuIndex) && (INT_COUNT_CPU (cpuNb) > 0)))
    	    {
	    spyCpuTbl[cpuNb].interruptIncTicks++;

	    /* Was the kernel preempted by an ISR? */

	    if (cpuNb == knlCpuIdx)
		continue;
	    }
	else if (cpuNb == knlCpuIdx)
	    {
	    /* Kernel state */

	    spyCpuTbl[cpuNb].kernelIncTicks++;
	    continue;
	    }
	else if (kernelIsCpuIdle (cpuNb))
	    {
	    /* Show idle task ticks in the task counters */

	    (IDLE_TASK_CPU (cpuNb))->taskIncTicks++;
	    spyCpuTbl[cpuNb].idleIncTicks++;
	    }
	else
	    {
	    if (taskIsReady ((TASK_ID) TID_CURRENT_CPU (cpuNb)))
		{
		(TID_CURRENT_CPU (cpuNb))->taskIncTicks++;
		spyCpuTbl[cpuNb].taskIncTicks++; /* task mode */
		}
	    else
		{
		/*
		 * In SMP mode, since spy do not lock the kernel to reduce
		 * Spy intrusiveness, we can reach this point:
		 * While current CPU is busy handling an auxiliary clock
		 * interrupt, other CPUs are running uninterrupted, therefore
		 * task states and kernel lock ownership can change
		 * asynchronously.
		 *
		 * This means that taskIdCurrent was probably ready right
		 * before taskIsReady(), but its state just changed...
		 * We simply ignore this specific case.
		 */
		}
	    }
	}
#endif /* _WRS_CONFIG_SMP */

    spyIncTicks++;
    }

/*******************************************************************************
*
* spyClkStartCommon - start collecting task activity data
*
* This routine begins data collection by enabling the auxiliary clock
* interrupts at a frequency of <intsPerSec> interrupts per second.  If
* <intsPerSec> is omitted or zero, the frequency will be 100.  Data from
* previous collections is cleared.
*
* RETURNS:
* OK, or ERROR if the CPU has no auxiliary clock, or if task create and
* delete hooks cannot be installed.
*
* SEE ALSO: sysAuxClkConnect()
*
* NOMANUAL
*/

STATUS spyClkStartCommon
    (
    int		intsPerSec,	/* timer interrupt freq, 0 = default of 100 */
    FUNCPTR	printRtn	/* routine to display result */
    )
    {
    FAST int ix;
    FAST int nTasks;
    FAST WIND_TCB *pTcb;

    if (spyClkRunning)
	return (ERROR);		/* We're already going */

    if ((_func_taskCreateHookAdd != NULL) && (_func_taskDeleteHookAdd != NULL))
	{
	if (!spyInitialized &&
	    ((* _func_taskCreateHookAdd) (spyCreateHook) == ERROR ||
	     (* _func_taskDeleteHookAdd) (spyDeleteHook) == ERROR))
	    {
	    if (printRtn != NULL)
		(* printRtn) ("Unable to add create/delete hooks.\n");

	    return (ERROR);
	    }
	}

    spyInitialized = TRUE;

    if (intsPerSec == 0)
	intsPerSec = 100;

    /* Reset counters */

    spyDeleteCount  = 0;
    spyCreateCount  = 0;
    spyTotalTicks = 0;
    spyIncTicks = 0;
    
    spyKernelTotalTicks = 0;
    spyInterruptTotalTicks = 0;
    spyIdleTotalTicks = 0;

    /* Clear persistent counters */

    memset (spyCpuTbl, 0, vxCpuConfiguredGet () * sizeof (SPY_CPU_DATA));

    /* Initialize tick counters of tasks already running */

    nTasks = taskIdListGet (pIdList, maxSpyTasks);

    for (ix = 0; ix < nTasks; ++ix)
	{
	pTcb = taskTcb (pIdList [ix]);

	/*
	 * Need to make sure pTcb is a valid pointer
	 */

	if (pTcb == NULL)
	    continue;

	pTcb->taskIncTicks = pTcb->taskTicks = 0;
	}

    if (fpgaAuxClkConnect ((FUNCPTR)spyClkInt, (_Vx_usr_arg_t)0, CFE_PSP_SPYLIB_FPGA_TIMER_ID) != OK)
	{
	if (printRtn != NULL)
	    (* printRtn) ("No auxiliary clock on CPU.\n");
	return (ERROR);
	}

    auxClkTicksPerSecond = fpgaAuxClkRateGet(CFE_PSP_SPYLIB_FPGA_TIMER_ID);

    (void) fpgaAuxClkRateSet (intsPerSec, CFE_PSP_SPYLIB_FPGA_TIMER_ID);
    fpgaAuxClkEnable (CFE_PSP_SPYLIB_FPGA_TIMER_ID);

    spyClkRunning = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* spyClkStopCommon - stop collecting task activity data
*
* This routine disables the auxiliary clock interrupts.
* Data collected remains valid until the next spyClkStart() call.
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStart()
*
* NOMANUAL
*/
void spyClkStopCommon (void)
    {
    (void) fpgaAuxClkRateSet(auxClkTicksPerSecond, CFE_PSP_SPYLIB_FPGA_TIMER_ID);
    fpgaAuxClkDisable(CFE_PSP_SPYLIB_FPGA_TIMER_ID);
    spyClkRunning  = FALSE;
    }

/*******************************************************************************
*
* spyReportCommon - display task activity data
*
* This routine reports on data gathered at interrupt level for the amount of
* CPU time utilized by each task, the amount of time spent at interrupt level,
* the amount of time spent in the kernel, and the amount of idle time.  Time
* is displayed in ticks and as a percentage, and the data is shown since both
* the last call to spyClkStart() and the last spyReport().  If no interrupts
* have occurred since the last spyReport(), nothing is displayed.
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStart()
*
* NOMANUAL
*/

void spyReportCommon
    (
    FUNCPTR printRtn		/* routine to display result */
    )
    {
    spyReportCommon2 (printRtn, spyNoSymbolPrint);
    }

/*******************************************************************************
*
* spyReportCommon2 - display task activity data
*
* This routine is similar to spyReportCommon() but takes an additional
* parameter which allows to disable symbol lookup & display.
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStart()
*
* NOMANUAL
*/

void spyReportCommon2
    (
    FUNCPTR printRtn,		/* routine to display result */
    BOOL noSym			/* symbol lookup & display */
    )
    {
    FAST WIND_TCB * pTcb;
    FAST int 	   ix;
    int 	   taskPriority;
#ifndef _WRS_CONFIG_LP64
    char	   demangled [MAX_SYS_SYM_LEN + 1];
    char *	   nameToPrint = NULL;
    SYMBOL_DESC	   symbolDesc;	/* symFind() descriptor */
#endif	/* !_WRS_CONFIG_LP64 */
    FAST int	   nTasks;
    UINT	   cpuNb;
#ifdef _WRS_CONFIG_SMP
    UINT	   cpuTotal;
    UINT	   itPct = 0;
    UINT	   idlPct = 0;
    UINT	   knlPct = 0;
    UINT	   tskPct = 0;
    UINT	   tmpTaskIncTicks = 0;
#endif	/* _WRS_CONFIG_SMP */
    UINT 	   tmpIdleIncTicks = 0;
    UINT 	   tmpKernelIncTicks = 0;
    UINT 	   tmpInterruptIncTicks = 0;

    int 	   tmpIncTicks;			/* incremental snap shot */
    int 	   totalPerCent;
    int 	   incPerCent;
    int 	   sumTotalPerCent = 0;
    int 	   sumIncPerCent   = 0;

    /* If there have been no ticks, there is nothing to report */

    if (spyIncTicks == 0)
	return;

    spyNoSymbolPrint = noSym;

    /* Snapshot and clear task statistics */

    nTasks = taskIdListGet (pIdList, maxSpyTasks);

    spyTaskIdListSort (pIdList, nTasks);

    for (ix = 0; ix < nTasks; ++ix)
	{
	pTcb = taskTcb (pIdList [ix]);

	/* Need to make sure pTcb is a valid pointer */

	if (pTcb == NULL)
	    continue;

	/* Order is important: save and clear incremental, then update total */

	pSpyInfo[ix].taskIncTicks	= pTcb->taskIncTicks;
	pTcb->taskIncTicks		= 0;

	pTcb->taskTicks			+= pSpyInfo[ix].taskIncTicks;
	pSpyInfo[ix].taskTotalTicks	= pTcb->taskTicks;
	}

    /* 
     * Save and clear incremental counts and accumulate totals. We have to use
     * temporary variables because the counters are still modified by the
     * measurement interrupt routine.
     */

    tmpIncTicks          = spyIncTicks;
    spyIncTicks		 = 0;
    spyTotalTicks       += tmpIncTicks;

    for (cpuNb = 0; cpuNb < vxCpuConfiguredGet (); cpuNb++)
	{
	/* 
	 * Compute deltas totals. These contain the kernel, idle, interrupt and
	 * task ticks spent by all CPUs since the last report. They are used to
	 * show deltas totals in the first report (task-based).
	 */

	tmpKernelIncTicks += spyCpuTbl[cpuNb].kernelIncTicks;
	tmpIdleIncTicks += spyCpuTbl[cpuNb].idleIncTicks;
	tmpInterruptIncTicks += spyCpuTbl[cpuNb].interruptIncTicks;

#ifdef _WRS_CONFIG_SMP
	tmpTaskIncTicks += spyCpuTbl[cpuNb].taskIncTicks;

	/* 
	 * Compute totals per CPU. These contain the total number of ticks spent
	 * by each CPU in one of the four states (knl, idle, it and tsk). They
	 * are used for the second report (per-CPU SMP stats) since the
	 * incremental counters are cleared between reports.
	 */ 
    
	spyCpuTbl[cpuNb].kernelTotalTicks += spyCpuTbl[cpuNb].kernelIncTicks;
	spyCpuTbl[cpuNb].interruptTotalTicks += 
					    spyCpuTbl[cpuNb].interruptIncTicks;
	spyCpuTbl[cpuNb].idleTotalTicks += spyCpuTbl[cpuNb].idleIncTicks;
	spyCpuTbl[cpuNb].taskTotalTicks += spyCpuTbl[cpuNb].taskIncTicks;
#endif	/* _WRS_CONFIG_SMP */

	/* Reset delta counters */

	spyCpuTbl[cpuNb].kernelIncTicks = 0;
	spyCpuTbl[cpuNb].interruptIncTicks = 0;
	spyCpuTbl[cpuNb].idleIncTicks = 0;
#ifdef _WRS_CONFIG_SMP
	spyCpuTbl[cpuNb].taskIncTicks = 0;
#endif	/* _WRS_CONFIG_SMP */
	}

    /* 
     * Compute totals since spy start. These contain the sum of ticks spent by
     * all CPUs in each state since spy start. They are used to compute
     * percentages in the first report.
     */

    spyKernelTotalTicks += tmpKernelIncTicks;
    spyInterruptTotalTicks += tmpInterruptIncTicks;
    spyIdleTotalTicks += tmpIdleIncTicks;

    if (printRtn == NULL)	/* for host browser don't display result */
	return;

    /* Print info */

    (* printRtn) ("\n");
#ifndef	_WRS_CONFIG_LP64
    (* printRtn) (
"    NAME         ENTRY       TID      PRI   total %% (ticks)  delta %% (ticks)\n");
    (* printRtn) (
"------------ ------------ ----------  ---   ---------------  ---------------\n");
#else	/* _WRS_CONFIG_LP64 */
    (* printRtn) (
"    NAME            TID          PRI   total %%   (ticks)  delta %%   (ticks)\n");
    (* printRtn) (
"------------ ------------------  ---   -----------------  -----------------\n");

#endif	/* _WRS_CONFIG_LP64 */

    /* Deal with tasks */

    for (ix = 0; ix < nTasks; ++ix)
	{
#ifndef _WRS_CONFIG_LP64
	char addrStr[2 + sizeof(pTcb->entry)*2 + 1];	/* for hexa address */
	nameToPrint = NULL;
#endif	/* !_WRS_CONFIG_LP64 */
	/* Find task entry point symbol name in symbol table */

	pTcb = taskTcb (pIdList [ix]);

	/* Need to make sure pTcb is a valid pointer */

	if (pTcb == NULL)
	    continue;

#ifndef _WRS_CONFIG_LP64
	if (!spyNoSymbolPrint)
	    {
	memset (&symbolDesc, 0, sizeof (SYMBOL_DESC));
	symbolDesc.mask		= SYM_FIND_BY_VALUE;
	symbolDesc.value	= (SYM_VALUE)pTcb->entry;
	symbolDesc.name		= NULL;

	if ((_func_symFind != NULL) && (sysSymTbl != NULL) &&
	    (_func_symFind (sysSymTbl, &symbolDesc) == OK) &&
	    (symbolDesc.value == (SYM_VALUE)pTcb->entry))
	    {
	    nameToPrint = cplusDemangle (symbolDesc.name, demangled,
					 sizeof (demangled));
	    }
	    }

	if (nameToPrint == NULL)
	    {
	    /*
	     * The entry point does not exactly match a symbol, or symbol
	     * lookup is disabled using SPY_SYMBOL_PRINT parameter: Display
	     * the entry point as a numerical value.
	     */

	    nameToPrint = addrStr;
	    sprintf (nameToPrint, "%p", pTcb->entry);
	    }
	    
	if (!spyNoSymbolPrint)
	free (symbolDesc.name);
#endif	/* !_WRS_CONFIG_LP64 */

        (void) taskPriorityGet (pIdList [ix], &taskPriority);

	/* Print line for this task */

	totalPerCent     = (pSpyInfo[ix].taskTotalTicks * 100) / spyTotalTicks;
	incPerCent       = (pSpyInfo[ix].taskIncTicks * 100) / tmpIncTicks;
	sumTotalPerCent += totalPerCent;
	sumIncPerCent   += incPerCent;

	(* printRtn) (spyFmt1, objNamePtrGet ((OBJ_ID) pTcb),
#ifndef _WRS_CONFIG_LP64
		      nameToPrint,		/* No entry field in LP64 */
#endif	/* !_WRS_CONFIG_LP64 */
		      pIdList [ix], taskPriority, totalPerCent,
		      pSpyInfo[ix].taskTotalTicks, incPerCent,
		      pSpyInfo[ix].taskIncTicks);
	}

    /* Kernel totals */

    totalPerCent     = (spyKernelTotalTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpKernelIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    (* printRtn) (spyFmt2, "KERNEL",
#ifndef _WRS_CONFIG_LP64
		  "",				/* No entry field in LP64 */
#endif	/* !_WRS_CONFIG_LP64 */
		  "", "", totalPerCent,
		  spyKernelTotalTicks, incPerCent, tmpKernelIncTicks);

    /* Interrupt totals */

    totalPerCent     = (spyInterruptTotalTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpInterruptIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    (* printRtn) (spyFmt2, "INTERRUPT",
#ifndef _WRS_CONFIG_LP64
		  "",				/* No entry field in LP64 */
#endif	/* !_WRS_CONFIG_LP64 */
		  "", "", totalPerCent,
		  spyInterruptTotalTicks, incPerCent, tmpInterruptIncTicks);

#ifndef _WRS_CONFIG_SMP
    /* 
     * Idle totals. On SMP, they are already counted since we incremented the
     * idle tasks counters.
     */

    totalPerCent     = (spyIdleTotalTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpIdleIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    (* printRtn) (spyFmt2, "IDLE",
#ifndef _WRS_CONFIG_LP64
		  "",				/* No entry field in LP64 */
#endif	/* !_WRS_CONFIG_LP64 */
		  "", "", totalPerCent, spyIdleTotalTicks,
		  incPerCent, tmpIdleIncTicks);
#endif	/* _WRS_CONFIG_SMP */

    /* Grand total */

    (* printRtn) (spyFmt2, "TOTAL",
#ifndef _WRS_CONFIG_LP64
		  "",				/* No entry field in LP64 */
#endif	/* !_WRS_CONFIG_LP64 */
		  "", "", sumTotalPerCent, spyTotalTicks,
		  sumIncPerCent, tmpIncTicks);

    (* printRtn) ("\n");

    /* Mark deletions and creations */

    if (spyCreateCount > 0)
	{
	(* printRtn) ("%d task%s created.\n", spyCreateCount,
		spyCreateCount == 1 ? " was" : "s were");
	spyCreateCount = 0;
	}

    if (spyDeleteCount > 0)
	{
	(* printRtn) ("%d task%s deleted.\n", spyDeleteCount,
		spyDeleteCount == 1 ? " was" : "s were");
	spyDeleteCount = 0;
	}

#ifdef _WRS_CONFIG_SMP
    (* printRtn) ("\n");
    (* printRtn) (
    "CPU      KERNEL        INTERRUPT          IDLE           TASK       TOTAL\n");
    (* printRtn) (
    "--- --------------- --------------- --------------- --------------- -----\n");

    for (cpuNb = 0; cpuNb < vxCpuConfiguredGet (); cpuNb++)
	{
	/* Compute percentages */

	cpuTotal = spyCpuTbl[cpuNb].interruptTotalTicks + 
		   spyCpuTbl[cpuNb].kernelTotalTicks +
		   spyCpuTbl[cpuNb].idleTotalTicks + 
		   spyCpuTbl[cpuNb].taskTotalTicks;

	/* Avoid inactive CPUs division by zero */

	if (cpuTotal == 0)
	    cpuTotal = 1;

	itPct = spyCpuTbl[cpuNb].interruptTotalTicks * 100 / cpuTotal;
	idlPct = spyCpuTbl[cpuNb].idleTotalTicks * 100 / cpuTotal;
	knlPct = spyCpuTbl[cpuNb].kernelTotalTicks * 100 / cpuTotal;
	tskPct = spyCpuTbl[cpuNb].taskTotalTicks * 100 / cpuTotal;

	/* Print CPU line */

	(* printRtn) (spySmpFmt, cpuNb, 
		      knlPct, spyCpuTbl[cpuNb].kernelTotalTicks,
		      itPct, spyCpuTbl[cpuNb].interruptTotalTicks, 
		      idlPct, spyCpuTbl[cpuNb].idleTotalTicks,
		      tskPct, spyCpuTbl[cpuNb].taskTotalTicks,
		      itPct + idlPct + knlPct + tskPct);
	}
#endif	/* _WRS_CONFIG_SMP */
    }

/*******************************************************************************
*
* spyComTask - run periodic task activity reports
*
* This routine is spawned as a task by spy() to provide periodic task
* activity reports.  It prints a report, delays for the specified number of
* seconds, and repeats.
*
* RETURNS: N/A
*
* SEE ALSO: spy()
*
* NOMANUAL
*/

void spyComTask
    (
    int		freq,		/* reporting frequency, in seconds */
    FUNCPTR	printRtn	/* routine to display results */
    )
    {
    int delay = freq * sysClkRateGet ();

    while (TRUE)
	{
	spyReportCommon (printRtn);
	taskDelay (delay);
	}
    }

/*******************************************************************************
*
* spyStopCommon - stop spying and reporting
*
* This routine calls spyClkStop().  Any periodic reporting by spyTask()
* is terminated.
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStop(), spyTask()
*
* NOMANUAL
*/

void spyStopCommon (void)
    {
    spyClkStopCommon ();
    if (spyTaskId != TASK_ID_ERROR)
	{
	taskDelete (spyTaskId);
	spyTaskId = TASK_ID_ERROR;
	}
    }

/*******************************************************************************
*
* spyCommon - begin periodic task activity reports
*
* This routine collects task activity data and periodically runs spyReport().
* Data is gathered <ticksPerSec> times per second, and a report is made every
* <freq> seconds.  If <freq> is zero, it defaults to 5 seconds.  If
* <ticksPerSec> is omitted or zero, it defaults to 100.
*
* This routine spawns spyTask() to do the actual reporting.
*
* It is not necessary to call spyClkStart() before running spy().
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStart(), spyTask()
*
* NOMANUAL
*/

void spyCommon
    (
    int		freq,		/* reporting freq in sec, 0 = default of 5 */
    int		ticksPerSec,	/* interrupt clock freq, 0 = default of 100 */
    FUNCPTR	printRtn	/* routine to use to display results */
    )
    {
    spyCommon2 (freq, ticksPerSec, spyNoSymbolPrint, printRtn);
    }

/*******************************************************************************
*
* spyCommon2 - begin periodic task activity reports
*
* This routine is similar to spyCommon() but takes an additional
* parameter which allows to disable symbol lookup & display.
*
* RETURNS: N/A
*
* SEE ALSO: spyClkStart(), spyTask()
*
* NOMANUAL
*/

void spyCommon2
    (
    int		freq,		/* reporting freq in sec, 0 = default of 5 */
    int		ticksPerSec,	/* interrupt clock freq, 0 = default of 100 */
    BOOL	noSym,		/* Disable symbol lookup and display */
    FUNCPTR	printRtn	/* routine to use to display results */
    )
    {
    char cTaskName[] = "tSpyTask";
    if (freq == 0)
	freq = 5;	/* default frequency is 5 secs */

    spyNoSymbolPrint = noSym;

    if (spyClkStartCommon (ticksPerSec, printRtn) == OK)
	{
	spyTaskId = taskSpawn (cTaskName, spyTaskPriority,
			       spyTaskOptions, spyTaskStackSize,
			       (FUNCPTR)spyComTask,
			       (_Vx_usr_arg_t)freq, (_Vx_usr_arg_t) printRtn,
			       (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0,
			       (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0,
			       (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0,
			       (_Vx_usr_arg_t)0, (_Vx_usr_arg_t)0);

	if ((spyTaskId == TASK_ID_ERROR) && (printRtn != NULL))
	    (* printRtn) ("Unable to spawn spyTask.\n");
	}
    }

/*******************************************************************************
*
* spyTaskIdListSort - sort the ID list by priority
*
* This routine sorts the task ID list <idList> by task priority.
*
* RETURNS: N/A
*/

LOCAL void spyTaskIdListSort
    (
    TASK_ID idList[],
    int nTasks
    )
    {
    FAST TASK_ID temp;
    int prevPri;
    int curPri;
    FAST TASK_ID * pCurId;
    BOOL change = TRUE;
    FAST TASK_ID * pEndId = &idList [nTasks];

    if (nTasks == 0)
        return;

    while (change)
        {
        change = FALSE;

        (void) taskPriorityGet (idList[0], &prevPri);

        for (pCurId = &idList[1]; pCurId < pEndId; ++pCurId, prevPri = curPri)
            {
            (void) taskPriorityGet (*pCurId, &curPri);

            if (prevPri > curPri)
                {
                temp = *pCurId;
                *pCurId = *(pCurId - 1);
                *(pCurId - 1) = temp;
                change = TRUE;
                }
            }
        }
    }
