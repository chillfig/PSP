# Sleep for a bit to let target servers come up
msleep 500

# Attach to target1
# Connecting to Target PSP_FT
if [catch "wtxToolAttach PSP_FT psp_ft" target_attach_error] {
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
set psp_options     0x00000003
set psp_stack_addr  0
set psp_stack_size  16384
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
set task_info [wtxObjModuleInfoGet psp_ft.exe]
set moduleID [lindex $task_info 0]

# Resume task
wtxContextResume CONTEXT_TASK $task_id

puts stdout "Waiting for Functional Test to complete"

# Set maximum number of seconds to wait to 100 sec
set max_wait 100
set waiting_for 0
# Wait until done by listening to a special USER event
while {([set event [wtxEventGet]] == "") && ($waiting_for < $max_wait)} {
    msleep 1000
    incr waiting_for
}

# If we received an event message, print it
if {($event != "")} {
    puts stdout $event
}

# If we waited for max_wait seconds, kill process
if {($waiting_for == $max_wait)} {
    puts stdout "Timeout waiting for Functional Test"
    puts stdout "Killing Task"
    wtxContextKill $moduleID
}

# If we waited less than max_wait seconds, nothing to do?
if {($waiting_for < $max_wait)} {
    puts stdout "Functional Test run for $waiting_for seconds"
    puts stdout "Closing"
    # Give it another second for printing on target
    msleep 1
    return "OK"
}

# Disconnect from Target PSP_FT_FC
wtxToolDetach
