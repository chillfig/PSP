# Pseudo-terminal interface module
add_psp_module(ping cfe_psp_ping.c)

target_include_directories(ping PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

target_include_directories(ping PRIVATE 
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
)