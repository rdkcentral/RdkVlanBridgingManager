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

/***********************************************************************

    module: plugin_main.c

        Implement COSA Data Model Library Init and Unload apis.
 
    ---------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    ---------------------------------------------------------------

    revision:

        09/12/2019    initial revision.

**********************************************************************/

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
#include "plugin_main_apis.h"
#include "vlan_dml.h"
#include "ethernet_dml.h"

PBACKEND_MANAGER_OBJECT g_pBEManager;
void *                       g_pDslhDmlAgent;
extern ANSC_HANDLE     g_MessageBusHandle_Irep;
extern char            g_SubSysPrefix_Irep[32];
extern COSARepopulateTableProc            g_COSARepopulateTable;

#define THIS_PLUGIN_VERSION                         1

int ANSC_EXPORT_API
VlanManager_Init
    (
        ULONG                       uMaxVersionSupported, 
        void*                       hCosaPlugInfo         /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO               pPlugInfo                   = (PCOSA_PLUGIN_INFO                 )hCosaPlugInfo;
    COSAGetParamValueByPathNameProc pGetParamValueByPathNameProc = (COSAGetParamValueByPathNameProc)NULL;
    COSASetParamValueByPathNameProc pSetParamValueByPathNameProc = (COSASetParamValueByPathNameProc)NULL;
    COSAGetParamValueStringProc     pGetStringProc              = (COSAGetParamValueStringProc       )NULL;
    COSAGetParamValueUlongProc      pGetParamValueUlongProc     = (COSAGetParamValueUlongProc        )NULL;
    COSAGetParamValueIntProc        pGetParamValueIntProc       = (COSAGetParamValueIntProc          )NULL;
    COSAGetParamValueBoolProc       pGetParamValueBoolProc      = (COSAGetParamValueBoolProc         )NULL;
    COSASetParamValueStringProc     pSetStringProc              = (COSASetParamValueStringProc       )NULL;
    COSASetParamValueUlongProc      pSetParamValueUlongProc     = (COSASetParamValueUlongProc        )NULL;
    COSASetParamValueIntProc        pSetParamValueIntProc       = (COSASetParamValueIntProc          )NULL;
    COSASetParamValueBoolProc       pSetParamValueBoolProc      = (COSASetParamValueBoolProc         )NULL;
    COSAGetInstanceNumbersProc      pGetInstanceNumbersProc     = (COSAGetInstanceNumbersProc        )NULL;

    COSAValidateHierarchyInterfaceProc
                                    pValInterfaceProc           = (COSAValidateHierarchyInterfaceProc)NULL;
    COSAGetHandleProc               pGetRegistryRootFolder      = (COSAGetHandleProc                 )NULL;
    COSAGetInstanceNumberByIndexProc
                                    pGetInsNumberByIndexProc    = (COSAGetInstanceNumberByIndexProc  )NULL;
    COSAGetInterfaceByNameProc      pGetInterfaceByNameProc     = (COSAGetInterfaceByNameProc        )NULL;

    if ( uMaxVersionSupported < THIS_PLUGIN_VERSION )
    {
      /* this version is not supported */
        return -1;
    }

    pPlugInfo->uPluginVersion       = THIS_PLUGIN_VERSION;
    g_pDslhDmlAgent                 = pPlugInfo->hDmlAgent;

/*
    pGetCHProc = (COSAGetCommonHandleProc)pPlugInfo->AcquireFunction("COSAGetDiagPluginInfo");

    if( pGetCHProc != NULL)
    {
        g_pCosaDiagPluginInfo = pGetCHProc(NULL);
    }
    else
    {
        goto EXIT;
    }
*/
    pGetParamValueByPathNameProc = (COSAGetParamValueByPathNameProc)pPlugInfo->AcquireFunction("COSAGetParamValueByPathName");

    if( pGetParamValueByPathNameProc != NULL)
    {
        g_GetParamValueByPathNameProc = pGetParamValueByPathNameProc;
    }
    else
    {
        goto EXIT;
    }

    pSetParamValueByPathNameProc = (COSASetParamValueByPathNameProc)pPlugInfo->AcquireFunction("COSASetParamValueByPathName");

    if( pSetParamValueByPathNameProc != NULL)
    {
        g_SetParamValueByPathNameProc = pSetParamValueByPathNameProc;
    }
    else
    {
        goto EXIT;
    }

    pGetStringProc = (COSAGetParamValueStringProc)pPlugInfo->AcquireFunction("COSAGetParamValueString");

    if( pGetStringProc != NULL)
    {
        g_GetParamValueString = pGetStringProc;
    }
    else
    {
        goto EXIT;
    }

    pGetParamValueUlongProc = (COSAGetParamValueUlongProc)pPlugInfo->AcquireFunction("COSAGetParamValueUlong");

    if( pGetParamValueUlongProc != NULL)
    {
        g_GetParamValueUlong = pGetParamValueUlongProc;
    }
    else
    {
        goto EXIT;
    }


    pGetParamValueIntProc = (COSAGetParamValueIntProc)pPlugInfo->AcquireFunction("COSAGetParamValueInt");

    if( pGetParamValueIntProc != NULL)
    {
        g_GetParamValueInt = pGetParamValueIntProc;
    }
    else
    {
        goto EXIT;
    }

    pGetParamValueBoolProc = (COSAGetParamValueBoolProc)pPlugInfo->AcquireFunction("COSAGetParamValueBool");

    if( pGetParamValueBoolProc != NULL)
    {
        g_GetParamValueBool = pGetParamValueBoolProc;
    }
    else
    {
        goto EXIT;
    }

    pSetStringProc = (COSASetParamValueStringProc)pPlugInfo->AcquireFunction("COSASetParamValueString");

    if( pSetStringProc != NULL)
    {
        g_SetParamValueString = pSetStringProc;
    }
    else
    {
        goto EXIT;
    }

    pSetParamValueUlongProc = (COSASetParamValueUlongProc)pPlugInfo->AcquireFunction("COSASetParamValueUlong");

    if( pSetParamValueUlongProc != NULL)
    {
        g_SetParamValueUlong = pSetParamValueUlongProc;
    }
    else
    {
        goto EXIT;
    }


    pSetParamValueIntProc = (COSASetParamValueIntProc)pPlugInfo->AcquireFunction("COSASetParamValueInt");

    if( pSetParamValueIntProc != NULL)
    {
        g_SetParamValueInt = pSetParamValueIntProc;
    }
    else
    {
        goto EXIT;
    }

    pSetParamValueBoolProc = (COSASetParamValueBoolProc)pPlugInfo->AcquireFunction("COSASetParamValueBool");

    if( pSetParamValueBoolProc != NULL)
    {
        g_SetParamValueBool = pSetParamValueBoolProc;
    }
    else
    {
        goto EXIT;
    }

    pGetInstanceNumbersProc = (COSAGetInstanceNumbersProc)pPlugInfo->AcquireFunction("COSAGetInstanceNumbers");

    if( pGetInstanceNumbersProc != NULL)
    {
        g_GetInstanceNumbers = pGetInstanceNumbersProc;
    }
    else
    {
        goto EXIT;
    }

    pValInterfaceProc = (COSAValidateHierarchyInterfaceProc)pPlugInfo->AcquireFunction("COSAValidateHierarchyInterface");

    if ( pValInterfaceProc )
    {
        g_ValidateInterface = pValInterfaceProc;
    }
    else
    {
        goto EXIT;
    }
/*
#ifndef _ANSC_WINDOWSNT
#ifdef _SOFTWAREMODULES_SUPPORT_NAF
    CosaSoftwareModulesInit(hCosaPlugInfo);
#endif
#endif
*/
    pGetRegistryRootFolder = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetRegistryRootFolder");

    if ( pGetRegistryRootFolder != NULL )
    {
        g_GetRegistryRootFolder = pGetRegistryRootFolder;
    }
    else
    {
        printf("!!! haha, catcha !!!\n");
        goto EXIT;
    }

    pGetInsNumberByIndexProc = (COSAGetInstanceNumberByIndexProc)pPlugInfo->AcquireFunction("COSAGetInstanceNumberByIndex");

    if ( pGetInsNumberByIndexProc != NULL )
    {
        g_GetInstanceNumberByIndex = pGetInsNumberByIndexProc;
    }
    else
    {
        goto EXIT;
    }

    pGetInterfaceByNameProc = (COSAGetInterfaceByNameProc)pPlugInfo->AcquireFunction("COSAGetInterfaceByName");

    if ( pGetInterfaceByNameProc != NULL )
    {
        g_GetInterfaceByName = pGetInterfaceByNameProc;
    }
    else
    {
        goto EXIT;
    }

    g_pPnmCcdIf = g_GetInterfaceByName(g_pDslhDmlAgent, CCSP_CCD_INTERFACE_NAME);

    if ( !g_pPnmCcdIf )
    {
        CcspTraceError(("g_pPnmCcdIf is NULL !\n"));

        goto EXIT;
    }

    g_RegisterCallBackAfterInitDml = (COSARegisterCallBackAfterInitDmlProc)pPlugInfo->AcquireFunction("COSARegisterCallBackAfterInitDml");

    if ( !g_RegisterCallBackAfterInitDml )
    {
        goto EXIT;
    }

    g_COSARepopulateTable = (COSARepopulateTableProc)pPlugInfo->AcquireFunction("COSARepopulateTable");

    if ( !g_COSARepopulateTable )
    {
        goto EXIT;
    }

    /* Get Message Bus Handle */
    g_GetMessageBusHandle = (COSAGetHandleProc)pPlugInfo->AcquireFunction("COSAGetMessageBusHandle");
    if ( g_GetMessageBusHandle == NULL )
    {
        goto EXIT;
    }

    g_MessageBusHandle = (ANSC_HANDLE)g_GetMessageBusHandle(g_pDslhDmlAgent);
    if ( g_MessageBusHandle == NULL )
    {
        goto EXIT;
    }
    g_MessageBusHandle_Irep = g_MessageBusHandle;

    /* Get Subsystem prefix */
    g_GetSubsystemPrefix = (COSAGetSubsystemPrefixProc)pPlugInfo->AcquireFunction("COSAGetSubsystemPrefix");
    if ( g_GetSubsystemPrefix != NULL )
    {
        char*   tmpSubsystemPrefix;

        if (( tmpSubsystemPrefix = g_GetSubsystemPrefix(g_pDslhDmlAgent) ))
        {
            AnscCopyString(g_SubSysPrefix_Irep, tmpSubsystemPrefix);
        }

        /* retrieve the subsystem prefix */
        g_SubsystemPrefix = g_GetSubsystemPrefix(g_pDslhDmlAgent);
    }

    /* register the back-end apis for the data model */
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetEntryCount", Vlan_GetEntryCount );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetEntry",  Vlan_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetParamBoolValue",  Vlan_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetParamUlongValue",  Vlan_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetParamStringValue",  Vlan_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_SetParamBoolValue",  Vlan_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_SetParamIntValue",  Vlan_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_GetParamIntValue",  Vlan_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_SetParamUlongValue", Vlan_SetParamUlongValue );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_SetParamStringValue",  Vlan_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_Validate",  Vlan_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_Commit",  Vlan_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Vlan_Rollback", Vlan_Rollback );

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_GetEntryCount", EthLink_GetEntryCount );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_GetEntry",  EthLink_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_GetParamBoolValue",  EthLink_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_GetParamUlongValue",  EthLink_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_GetParamStringValue",  EthLink_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_SetParamBoolValue",  EthLink_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_SetParamUlongValue", EthLink_SetParamUlongValue );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_SetParamStringValue",  EthLink_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_Validate",  EthLink_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_Commit",  EthLink_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "EthLink_Rollback", EthLink_Rollback );

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_GetEntryCount", Marking_GetEntryCount );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_GetEntry", Marking_GetEntry );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_IsUpdated", Marking_IsUpdated );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_Synchronize", Marking_Synchronize );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_GetParamIntValue", Marking_GetParamIntValue );
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Marking_GetParamUlongValue", Marking_GetParamUlongValue );

    /* Create backend framework */
    g_pBEManager = (PBACKEND_MANAGER_OBJECT)BackEndManagerCreate();

    if ( g_pBEManager && g_pBEManager->Initialize )
    {
        g_pBEManager->hCosaPluginInfo = pPlugInfo;

        g_pBEManager->Initialize   ((ANSC_HANDLE)g_pBEManager);
    }
    
    return  0;

EXIT:

    return -1;
}

BOOL ANSC_EXPORT_API
VlanManager_IsObjectSupported
    (
        char*                        pObjName
    )
{
    
    return TRUE;
}

void ANSC_EXPORT_API
VlanManager_Unload
    (
        void
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;

    /* unload the memory here */

    returnStatus  =  BackEndManagerRemove(g_pBEManager);
        
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        g_pBEManager = NULL;
    }
    else
    {
        /* print error trace*/
        g_pBEManager = NULL;
    }
}



void ANSC_EXPORT_API
COSA_MemoryCheck
    (
        void
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCOSA_PLUGIN_INFO               pPlugInfo               = (PCOSA_PLUGIN_INFO)g_pBEManager->hCosaPluginInfo;

    /* unload the memory here */

    returnStatus  =  BackEndManagerRemove(g_pBEManager);

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        g_pBEManager = NULL;
    }
    else
    {
        g_pBEManager = NULL;
    }


    g_pBEManager = (PBACKEND_MANAGER_OBJECT)BackEndManagerCreate();

    if ( g_pBEManager && g_pBEManager->Initialize )
    {
        g_pBEManager->hCosaPluginInfo = pPlugInfo;

        g_pBEManager->Initialize   ((ANSC_HANDLE)g_pBEManager);
    }
}
