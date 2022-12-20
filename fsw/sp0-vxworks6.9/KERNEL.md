# VxWorks 6.9 Kernel Configuration for SP0

GATEWAY PSP for SP0 target requires specific kernel modules and settings. Some features are optional.

## Kernel Features for CFE and PSP

### Auto loading Gateway CFS

To automatically start Gateway CFS on boot, the kernel needs to read the boot startup string from the Boot Parameters area in the SP0 EEPROM. Aitech BSP already provides API to read and modify the EEPROM area. Gateway CFS also includes APIs to modify the Boot Parameters.

\code{.cpp}

```C
/**
* \func Run a script
* 
* \par Description:
* Function runs a script line by line
* 
* \param scriptPath - full path to script to execute
* \param fdRedirect - File descriptor for output of script
* 
* \return OK
* \return ERROR
*/
STATUS runScript ( char * scriptPath, int fdRedirect ) {
    char * shellTaskName;
    int fdScript;
    STATUS retVal = ERROR;

    if((fdScript = open(scriptPath,O_RDONLY,0))==ERROR)
    {
        printf("runScript: Error opening script at '%s' [[%s]!\n", scriptPath, strerror(errnoGet()));
        return retVal;
    }
    /*Launch a new session to execute the script*/
    retVal = shellGenericInit(NULL, 0x100000,NULL,&shellTaskName, FALSE, FALSE, fdScript, fdRedirect, fdRedirect);
    /*Wait for the script execution to end by monitoring the shell task life*/
    do
    {
        taskDelay(sysClkRateGet());
    }
    while (taskNameToId(shellTaskName) != TASK_ID_ERROR);
    close(fdScript);

    return retVal;
}
```

\endcode

## Optional Features

Those in this category are not necessary, but they do enhance CFS.

### Handling Multiple CFS Partitions

Kernel needs to declare a variable called `address_of_active_flash_partition_name`, 
and initialize it with the device name used in the boot startup string. For example, if the boot startup string is `/ffx0/startup`, the address set in `address_of_active_flash_partition_name` should be `/ffx0`. 
It is must not have the `/` character at the end of the address.

If the variable is not declared, CFS will use the default `/ffx0`.

Below is the sample code to implement the above logic. Call function `set_active_cfs_partition()` prior to loading the TTE network description and then prepend the string `address_of_active_flash_partition_name` to the TTE network description file name.

\code{.cpp}

```C
#include <bootLib.h>

extern char *sysBootLine;
char address_of_active_flash_partition_name[6] = {EOS};

/**
 * \func Run a script
 * 
 * \par Description:
 * Function will find the currently active CFS partition and save it to a
 * special variable: `address_of_active_flash_partition_name`
 * 
 * \par Notes:
 * Active CFS partition path shall not have an ending slash.
 * 
 * \param scriptPath - full path to script to execute
 * \param fdRedirect - File descriptor for output of script
 * 
 * \return OK
 * \return ERROR
 */
void set_active_cfs_partition(void)
{
    BOOT_PARAMS bootParams;
    char    *retStr = bootStringToStruct(sysBootLine,&bootParams);
    void    *firstSlash = NULL;
    void    *secondSlash = NULL;
    uint8_t root_path_length = 0;
    char    str_buffer[256] = {EOS};

    /*If the boot line parameters were acquired proceed with processing*/
    if (retStr[0] == EOS)
    {
        if(strlen(bootParams.startupScript) != 0)
        {
            /* Extract the root path. We are expecting the full path to be in the
            * form of /ffx0/startup. So we are looking for the word in between the first 2
            * slashes.
            */
            firstSlash = memchr(bootParams.startupScript, '/', strlen(bootParams.startupScript));
            secondSlash = memchr((firstSlash + 1), '/', strlen(bootParams.startupScript));
            /* number of characters between the 2 occurrences of '/' */
            root_path_length = secondSlash - firstSlash;

            /* Only copy if there is enough space in the receiving variable */
            if (root_path_length < sizeof(address_of_active_flash_partition_name))
            {
                memcpy(address_of_active_flash_partition_name, firstSlash, root_path_length);
                address_of_active_flash_partition_name[root_path_length+1] = EOS;
            }
            else
            {
                printf("ERROR: Could not find root path string `%s` - %d - %d\n", bootParams.startupScript, firstSlash, secondSlash);
                address_of_active_flash_partition_name[0] = EOS;
            }
            printf("Active CFS partition is `%s`\n", address_of_active_flash_partition_name);
        }
        else
        {
        	snprintf(address_of_active_flash_partition_name, 6, "/ffx0");
        }
    }
    else
    {
        printf("ERROR: Boot Startup parameters could not be parsed\n");
    }
}
```

\endcode