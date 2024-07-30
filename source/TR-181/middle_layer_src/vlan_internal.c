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

#include "vlan_mgr_apis.h"
#include "vlan_apis.h"
#include "vlan_internal.h"
#include "plugin_main_apis.h"
#include "poam_irepfo_interface.h"
#include "sys_definitions.h"
#include "ccsp_psm_helper.h"
#include "platform_hal.h"

/*TODO
 *Need to be Reviewed after Unification is finalised.
 */
#define PSM_VLANMANAGER_CFG_COUNT  "dmsb.vlanmanager.cfg.count"
#define PSM_VLANMANAGER_CFG_REGION "dmsb.vlanmanager.cfg.%d.region"
#define PSM_VLANMANAGER_CFG_VLANID "dmsb.vlanmanager.cfg.%d.vlanid"
#define PSM_VLANMANAGER_CFG_TPID   "dmsb.vlanmanager.cfg.%d.tpid"

extern char                     g_Subsystem[32];
extern ANSC_HANDLE              bus_handle;

static ANSC_STATUS VlanTerminationInitialize( ANSC_HANDLE hThisObject);

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaVlanCreate
            (
            );

    description:

        This function constructs cosa vlan object and return handle.

    argument:

    return:     newly created vlan object.

**********************************************************************/

ANSC_HANDLE
VlanCreate
    (
        VOID
    )
{
    PDATAMODEL_VLAN             pMyObject    = (PDATAMODEL_VLAN)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PDATAMODEL_VLAN)AnscAllocateMemory(sizeof(DATAMODEL_VLAN));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    //pMyObject->Oid             = DATAMODEL_VLAN_OID;
    pMyObject->Create            = VlanCreate;
    pMyObject->Remove            = VlanRemove;
    pMyObject->Initialize        = VlanInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS DmlVlanGetPSMRecordValue ( char *pPSMEntry, char *pOutputString )
{
    int   retPsmGet = CCSP_SUCCESS;
    char *strValue  = NULL;

    //Validate buffer
    if( ( NULL == pPSMEntry ) && ( NULL == pOutputString ) )
    {
        CcspTraceError(("%s %d Invalid buffer\n",__FUNCTION__,__LINE__));
        return retPsmGet;
    }

    retPsmGet = PSM_VALUE_GET_VALUE(pPSMEntry, strValue);
    if ( retPsmGet == CCSP_SUCCESS )
    {
        //Copy till end of the string
        snprintf( pOutputString, strlen( strValue ) + 1, "%s", strValue );

        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    }

    return retPsmGet;
}

/*TODO
 *need to Reviewed after Unification is finalised.
 */
#if defined(_HUB4_PRODUCT_REQ_)
static ANSC_STATUS VlanTerminationInitialize( ANSC_HANDLE hThisObject)
{
    PDATAMODEL_VLAN pMyObject = (PDATAMODEL_VLAN)hThisObject;
    INT vlanCfgIndexes[10] = {0};
    char acPSMQuery[128]    = { 0 };
    char acPSMValue[64]     = { 0 };
    INT vlanCfgCount = 0;
    INT vlanRegionCount = 0;
    INT vlanCount = 0;
    INT nIndex = 0;
    INT j = 0;
    char region[16] = {0};
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;

    // get resgion from platform and set VLAN info according.
    if( 0 == platform_hal_GetRouterRegion(region) )
    {
        memset(acPSMQuery, 0, sizeof(acPSMQuery));
        memset(acPSMValue, 0, sizeof(acPSMValue));
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_CFG_COUNT );
        if (CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
            vlanCfgCount = atoi (acPSMValue);
        }

        for (int i = 0; i < vlanCfgCount; i++)
        {
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_CFG_REGION, (i + 1) );
            if (CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                if(strncmp(region, acPSMValue, sizeof(region)) == 0)
                {
                    vlanRegionCount = (vlanRegionCount + 1);
                    vlanCfgIndexes[j++] = i;
                }
            }
        }
    }

    if (vlanRegionCount > 0)
    {
        memset(acPSMQuery, 0, sizeof(acPSMQuery));
        memset(acPSMValue, 0, sizeof(acPSMValue));
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_COUNT );
        if (CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
            vlanCount = atoi (acPSMValue);
        }

        if (vlanCount <= vlanRegionCount)
        {
            CcspTraceInfo(("%s-%d : VlanTermCount(%d) <= Region VlanID Count(%d) \n", __FUNCTION__, __LINE__, vlanCount, vlanRegionCount));
            pMyObject->ulVlantrInstanceNumber = vlanCount;
        }
        else
        {
            CcspTraceInfo(("%s-%d : VlanTermCount(%d) > Region VlanID Count(%d) \n", __FUNCTION__, __LINE__, vlanCount, vlanRegionCount));
            pMyObject->ulVlantrInstanceNumber = vlanRegionCount;
        }

        PDML_VLAN pVlan = (PDML_VLAN)AnscAllocateMemory(sizeof(DML_VLAN)* pMyObject->ulVlantrInstanceNumber);
        memset(pVlan, 0, sizeof(DML_VLAN));

        for(nIndex = 0; nIndex < pMyObject->ulVlantrInstanceNumber; nIndex++)
        {
            DML_VLAN_INIT(&pVlan[nIndex]);
            pVlan[nIndex].InstanceNumber = nIndex + 1;

            /* get enable value from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_ENABLE, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                if(strcmp (acPSMValue ,PSM_ENABLE_STRING_TRUE) == 0)
                {
                    pVlan[nIndex].Enable = TRUE ;
                }
                else
                {
                    pVlan[nIndex].Enable = FALSE;
                }
            }

            /* get alias from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_ALIAS, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                strcpy(pVlan[nIndex].Alias, acPSMValue);
            }

            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_NAME, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                strcpy(pVlan[nIndex].Name, acPSMValue);
            }

            /* get lowerlayers from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_LOWERLAYERS, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                strcpy(pVlan[nIndex].LowerLayers, acPSMValue);
            }

            /* get base interface from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_BASEINTERFACE, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                strcpy(pVlan[nIndex].BaseInterface, acPSMValue);
            }

            /* get vlanid from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_CFG_VLANID, (vlanCfgIndexes[nIndex] + 1) );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                pVlan[nIndex].VLANId = atoi(acPSMValue) ;
            }

            /* get cfg tpid from psm */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_CFG_TPID, (vlanCfgIndexes[nIndex] + 1) );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                pVlan[nIndex].TPId = atoi(acPSMValue) ;
            }

            /*TODO:
             *Need to be Removed Path From PSM Once RBUS Support Available in VlanManager and WanManager.
             */
            memset(acPSMQuery, 0, sizeof(acPSMQuery));
            memset(acPSMValue, 0, sizeof(acPSMValue));
            snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_PATH, nIndex + 1 );
            if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
            {
                strcpy(pVlan[nIndex].Path, acPSMValue);
            }
        }
        pMyObject->VlanTer = pVlan;
        returnStatus = ANSC_STATUS_SUCCESS;
    }
    return returnStatus;
}

#else

static ANSC_STATUS VlanTerminationInitialize( ANSC_HANDLE hThisObject)
{
    PDATAMODEL_VLAN pMyObject = (PDATAMODEL_VLAN)hThisObject;
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    char acPSMQuery[128]    = { 0 };
    char acPSMValue[64]     = { 0 };
    INT vlanCount = 0;
    INT nIndex = 0;

    /* delay to let it initialize */
    //sleep(2);
    /* get cfg count */
    snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_COUNT );
    if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
    {
        vlanCount = atoi (acPSMValue);
    }

    pMyObject->ulVlantrInstanceNumber = vlanCount ;

    PDML_VLAN pVlan = (PDML_VLAN)AnscAllocateMemory(sizeof(DML_VLAN)* vlanCount);
    memset(pVlan, 0, sizeof(DML_VLAN));

    for(nIndex = 0; nIndex < vlanCount; nIndex++)
    {
        pVlan[nIndex].InstanceNumber = nIndex + 1;
        /* get enable from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_ENABLE, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
            if(strcmp(acPSMValue,PSM_ENABLE_STRING_TRUE) == 0)
            {
                pVlan[nIndex].Enable = TRUE;
            }
            else
            {
                pVlan[nIndex].Enable = FALSE;
            }
        }

        /* get alias from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_ALIAS, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             strcpy(pVlan[nIndex].Alias,acPSMValue);
        }

        /* get name from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_NAME, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             strcpy(pVlan[nIndex].Name,acPSMValue);
        }
        /* get lowerlayes from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_LOWERLAYERS, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             strcpy(pVlan[nIndex].LowerLayers,acPSMValue);
        }

        /* get vlanid from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_VLANID, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             pVlan[nIndex].VLANId = atoi(acPSMValue) ;
        }

        /* get cfg tpid from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_TPID, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             pVlan[nIndex].TPId = atoi(acPSMValue) ;
        }

        /* get base interface from psm */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_BASEINTERFACE, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             strcpy(pVlan[nIndex].BaseInterface,acPSMValue);
        }

         /*TODO:
          *Need to be Removed Path From PSM Once RBUS Support Available in VlanManager and WanManager.
          */
        snprintf( acPSMQuery, sizeof( acPSMQuery ), PSM_VLANMANAGER_PATH, nIndex + 1 );
        if ( CCSP_SUCCESS == DmlVlanGetPSMRecordValue( acPSMQuery, acPSMValue ) )
        {
             strcpy(pVlan[nIndex].Path,acPSMValue);
        }
     }

    pMyObject->VlanTer = pVlan;

    return returnStatus;
}
#endif
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        VlanInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa vlan object and return handle.

    argument:	ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
VlanInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PDATAMODEL_VLAN                 pMyObject        = (PDATAMODEL_VLAN)hThisObject;

    /* Call Initiation */
    returnStatus = Vlan_Init();
    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        return returnStatus;
    }

    returnStatus = VlanTerminationInitialize( pMyObject );
    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        return returnStatus;
    }

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        VlanRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa vlan object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
VlanRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PDATAMODEL_VLAN                 pMyObject    = (PDATAMODEL_VLAN)hThisObject;

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

