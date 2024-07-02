/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Sky
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Copyright [2014] [Cisco Systems, Inc.]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef  _VLAN_APIS_H
#define  _VLAN_APIS_H

#include "vlan_mgr_apis.h"
#include "ssp_global.h"
#include "secure_wrapper.h"

/* * Telemetry Markers */
#define VLAN_MARKER_VLAN_IF_CREATE          "RDKB_VLAN_CREATE"
#define VLAN_MARKER_VLAN_IF_DELETE          "RDKB_VLAN_DELETE"

#define PSM_VLANMANAGER_COUNT             "dmsb.vlanmanager.ifcount"
#define PSM_VLANMANAGER_ENABLE            "dmsb.vlanmanager.%d.VlanEnable"
#define PSM_VLANMANAGER_ALIAS             "dmsb.vlanmanager.%d.alias"
#define PSM_VLANMANAGER_NAME              "dmsb.vlanmanager.%d.name"
#define PSM_VLANMANAGER_LOWERLAYERS       "dmsb.vlanmanager.%d.lowerlayers"
#define PSM_VLANMANAGER_VLANID            "dmsb.vlanmanager.%d.vlanid"
#define PSM_VLANMANAGER_TPID              "dmsb.vlanmanager.%d.tpid"
#define PSM_VLANMANAGER_BASEINTERFACE     "dmsb.vlanmanager.%d.baseinterface"
#define PSM_VLANMANAGER_PATH              "dmsb.vlanmanager.%d.path"

#define CCSP_SUBSYS "eRT."
#define PSM_VALUE_GET_VALUE(name, str) PSM_Get_Record_Value2(bus_handle, CCSP_SUBSYS, name, NULL, &(str))

#define PSM_ENABLE_STRING_TRUE  "TRUE"
#define PSM_ENABLE_STRING_FALSE  "FALSE"

/***********************************
    Actual definition declaration
************************************/
/*
    VLAN Part
*/
typedef enum { 
    VLAN_IF_UP = 1,
    VLAN_IF_DOWN,
    VLAN_IF_UNKNOWN,
    VLAN_IF_DORMANT,
    VLAN_IF_NOTPRESENT,
    VLAN_IF_LOWERLAYERDOWN,
    VLAN_IF_ERROR
}vlan_link_status_e;

typedef  struct
_DML_VLAN
{
    ULONG                InstanceNumber;
    BOOLEAN              Enable;
    vlan_link_status_e   Status;
    CHAR                 Alias[64];
    CHAR                 Name[64];
    UINT                 LastChange;
    CHAR                 LowerLayers[1024];
    CHAR                 BaseInterface[64];
    INT                  VLANId;
    UINT                 TPId;
    CHAR                 Path[1024];
}
DML_VLAN,  *PDML_VLAN;

static inline void DML_VLAN_INIT(PDML_VLAN pVlan)
{
    pVlan->Enable            = FALSE;
    pVlan->Status            = VLAN_IF_DOWN;
}

/*************************************
    The actual function declaration
**************************************/

/*
Description:
    This is the initialization routine for VLAN backend.
Arguments:
    hDml        Opaque handle from DM adapter. Backend saves this handle for calling pValueGenFn.
    phContext       Opaque handle passed back from backend, needed by CosaDmlVLANXyz() routines.
    pValueGenFn Function pointer to instance number/alias generation callback.
Return:
Status of operation.

*/
ANSC_STATUS
Vlan_Init( void );
/* APIs for VLAN */

ANSC_STATUS
Vlan_GetStatus
    (
        PDML_VLAN                   p_Vlan
    );

void * Vlan_Enable(void *Arg);
void * Vlan_Disable(void *Arg);

void get_uptime(long *uptime);
#endif
