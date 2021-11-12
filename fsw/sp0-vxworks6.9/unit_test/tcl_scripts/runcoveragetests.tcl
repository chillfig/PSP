
# Sleep for a bit to let target servers come up
msleep 500

# Attach to target1
# Connecting to Target PSP_UT_FC
wtxToolAttach PSP_UT_FC psp_ut_codecoverage

# Load and run tests (call OS_BSPMain())
# Loading PSP_UT
# wtxObjModuleLoad LOAD_ALL_SYMBOLS scripts/ut_on_target/payload/psp_ut.exe
if [catch "wtxObjModuleLoad LOAD_ALL_SYMBOLS ../payload/psp_ut.exe" sym] {
    puts stdout "Could not load psp_ut"
    error $sym
}

set symbol_info [wtxSymFind -name OS_BSPMain]
set coverageTestAddress [lindex $symbol_info 1]

set psp_priority    100
# VX_SPE_TASK = 0x04000000
# 
set psp_options     0x04000003
set psp_stack_addr  0
set psp_stack_size  2048
set redirIn         0
set redirOut        0
set delay           100

# Running OS_BSPMain() entry point
#if [catch "wtxFuncCall $coverageTestAddress" sym] {
#    error $sym
#}

if [ catch "set task_id [wtxContextCreate CONTEXT_TASK psp_ut $psp_priority $psp_options $psp_stack_addr $psp_stack_size $coverageTestAddress $redirIn $redirOut $delay]" sym ] {
    puts stdout "Could not Create Context"
    error $sym
}

# Add Event notifications
#wtxEventpointAdd tacc [symbol OS_BSPMain] task "$ctxId $ctxSubId"
#wtxEventpointAdd cexit $coverageTestAddress task "$ctxId $ctxSubId"

#Register to receive all events
#wtxRegisterForEvent .*

#proc listEvents {} {while {[set event [wtxEventGet]] != ""} {puts $event}}
#listEvents

# Resume task
wtxContextResume CONTEXT_TASK $task_id

#listEvents

# Sleep to let test run, then detach
# Waiting for 5 seconds until it ends
msleep 30000
# Disconnect from Target PSP_UT_FC
wtxToolDetach
