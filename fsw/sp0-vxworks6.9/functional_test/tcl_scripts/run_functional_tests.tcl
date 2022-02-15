# Sleep for a bit to let target servers come up
msleep 500

# Attach to target1
# Connecting to Target PSP_FT
if [catch "wtxToolAttach PSP_UT psp_ft" target_attach_error] {
    puts stdout "Could not connect to Target"
    puts $target_attach_error
    return "ERROR"
}

# Load and run tests (call PSP_FT_Start())
# Loading PSP_FT
if [catch "wtxObjModuleLoad LOAD_ALL_SYMBOLS ../payload/psp_ft.exe" module_load_error] {
    puts stdout "Could not load psp_ft"
    puts stdout $module_load_error
    return "ERROR"
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

# Create a suspended context (process) with above options
if [ catch "set task_id [wtxContextCreate CONTEXT_TASK psp_ft $psp_priority $psp_options $psp_stack_addr $psp_stack_size $coverageTestAddress $redirIn $redirOut $delay]" context_create_error ] {
    puts stdout "Could not Create Context"
    puts stdout $context_create_error
    return "ERROR"
}

#Register to receive all events
puts stdout "Registering events"
wtxRegisterForEvent USER.*

puts stdout [wtxEventpointListGet]

# Find Module ID so we can kill it later
set task_info [wtxObjModuleInfoGet psp_ut.exe]
set moduleID [lindex $task_info 0]

# Resume task
wtxContextResume CONTEXT_TASK $task_id

# Sleep to let test run, then detach
# Waiting for 15 seconds until it ends
msleep 15000

# Disconnect from Target PSP_FT_FC
wtxToolDetach
