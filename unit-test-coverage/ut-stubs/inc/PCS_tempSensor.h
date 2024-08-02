/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/* PSP coverage stub replacement for tempSensor.h */
#ifndef PCS_TEMPSENSOR_H
#define PCS_TEMPSENSOR_H

#include "PCS_basetypes.h"
#include "PCS_vxWorks.h"

/* ----------------------------------------- */
/* constants normally defined in tempSensor.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* types normally defined in tempSensor.h */
/* ----------------------------------------- */
typedef enum {
    LEFT_TEMP_SENSOR=0,
    RIGHT_TEMP_SENSOR=1,
    CPU_TEMP_SENSOR=2,
    CORE_CPU_TEMP_SENSOR=3  /* available in SP0 Upgrade HW configuration only */
} PCS_TEMP_SENSOR_TYPE;

typedef enum {
    CURRENT_TEMP=0, 
    TLOW=1, 
    THIGH=2
} PCS_TEMP_DATA_TYPE;

typedef enum {
    VALL_SENSOR=0,          /* available in SP0 Upgrade HW configuration only */
    V0P9_SENSOR=1,          /* available in SP0 Upgrade HW configuration only */
    V1P1_SENSOR=2,          /* available in SP0 Upgrade HW configuration only */
    V1P5_SENSOR=3,          /* available in SP0 Upgrade HW configuration only */
    V1P8_SENSOR=4,          /* available in SP0 Upgrade HW configuration only */
    V2P5_SENSOR=5,          /* available in SP0 Upgrade HW configuration only */
    V3P3_SENSOR=6,          /* available in SP0 Upgrade HW configuration only */
} PCS_VOL_SENSOR_TYPE;

typedef enum {
    CURRENT_VOL=0,
    VLOW=1,
    VHIGH=2
} PCS_VOL_DATA_TYPE;

/* ----------------------------------------- */
/* prototypes normally declared in tempSensor.h */
/* ----------------------------------------- */
extern int PCS_tempSensorRead(PCS_TEMP_SENSOR_TYPE sensor,
    PCS_TEMP_DATA_TYPE dataType, float *temperature, bool talkative);

extern int PCS_volSensorRead(PCS_VOL_SENSOR_TYPE sensor, PCS_VOL_DATA_TYPE dataType, float *voltage,
                         bool talkative);

#endif
