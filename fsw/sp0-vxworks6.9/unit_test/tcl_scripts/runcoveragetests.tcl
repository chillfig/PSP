# Sleep for a bit to let target servers come up
msleep 500

# Attach to target1
# Connecting to Target FC2
wtxToolAttach PSP_UT_FC psp_ut_codecoverage

# Load and run tests (call OS_BSPMain())
# Loading PSP_UT
wtxObjModuleLoad LOAD_ALL_SYMBOLS ../payload/psp_ut.exe
set coverageTestAddress [lindex [wtxSymFind -name OS_BSPMain] 1]
# Running OS_BSPMain() entry point
wtxFuncCall $coverageTestAddress

# Sleep to let test run, then detach
# Waiting for 5 seconds until it ends
msleep 25000
# Disconnect from Target FC2
wtxToolDetach
