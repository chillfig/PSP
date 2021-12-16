# Sleep for a bit to let target servers come up
msleep 500

# Attach to target1
# Connecting to Target PSP_FT
wtxToolAttach PSP_FT psp_ft

# Load and run tests (call PSP_FT_Start())
# Loading PSP_FT
if [catch "wtxObjModuleLoad LOAD_ALL_SYMBOLS ../payload/psp_ft.exe" sym] {
    puts stdout "Could not load psp_ft"
    error $sym
}

set symbol_info [wtxSymFind -name PSP_FT_Run]
set coverageTestAddress [lindex $symbol_info 1]

set psp_priority    100
# VX_SPE_TASK = 0x04000000
# 
set psp_options     0x04000003
set psp_stack_addr  0
set psp_stack_size  8192
set redirIn         0
set redirOut        0
set delay           100

# Running OS_BSPMain() entry point
#if [catch "wtxFuncCall $coverageTestAddress" sym] {
#    error $sym
#}

if [ catch "set task_id [wtxContextCreate CONTEXT_TASK psp_ft $psp_priority $psp_options $psp_stack_addr $psp_stack_size $coverageTestAddress $redirIn $redirOut $delay]" sym ] {
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
msleep 5000
# Disconnect from Target PSP_FT_FC
wtxToolDetach
