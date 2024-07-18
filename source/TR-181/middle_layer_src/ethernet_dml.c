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

#include "ansc_platform.h"
#include "ethernet_dml.h"
#include "ethernet_apis.h"
#include "plugin_main_apis.h"
#include "ethernet_internal.h"

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/
/***********************************************************************

 APIs for Object:

    Ethernet.Link.{i}.

    *  EthLink_GetEntryCount
    *  EthLink_GetEntry
    *  EthLink_GetParamBoolValue
    *  EthLink_GetParamUlongValue
    *  EthLink_GetParamStringValue
    *  EthLink_SetParamBoolValue
    *  EthLink_SetParamUlongValue
    *  EthLink_SetParamStringValue
    *  EthLink_Validate
    *  EthLink_Commit
    *  EthLink_Rollback

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthLink_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/

ULONG
EthLink_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PDATAMODEL_ETHERNET             pEthLink         = (PDATAMODEL_ETHERNET)g_pBEManager->hEth;

    if (pEthLink)
    {
        return (pEthLink->ulEthlinkInstanceNumber);
    }
    else
    {
        return 0;
    }


}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        EthLink_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/

ANSC_HANDLE
EthLink_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PDATAMODEL_ETHERNET             pMyObject         = (PDATAMODEL_ETHERNET)g_pBEManager->hEth;
    PDML_ETHERNET          pEthlink = NULL;

    if ( pMyObject->EthLink && nIndex < pMyObject->ulEthlinkInstanceNumber )
    {
        pEthlink = &(pMyObject->EthLink[nIndex]);
        *pInsNumber = pEthlink->InstanceNumber;
         return pEthlink;
    }

    return NULL;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EthLink_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PDML_ETHERNET              p_EthLink  = (PDML_ETHERNET   )hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = p_EthLink->Enable;
        return TRUE;
    }

    if (strcmp(ParamName, "PriorityTagging") == 0)
    {
        *pBool = p_EthLink->PriorityTagging;
        return TRUE;
    }

    if (strcmp(ParamName, "X_RDK_Refresh") == 0)
    {
        *pBool = FALSE;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EthLink_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PDML_ETHERNET           p_EthLink     = (PDML_ETHERNET   )hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Status") == 0)
    {
        if(ANSC_STATUS_SUCCESS == EthLink_GetStatus(p_EthLink)) {
            *puLong = p_EthLink->Status;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "MACAddrOffSet") == 0)
    {
        *puLong = p_EthLink->MACAddrOffSet;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthLink_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/

ULONG
EthLink_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PDML_ETHERNET              p_EthLink     = (PDML_ETHERNET   )hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Alias") == 0)
    {
        if ( AnscSizeOfString(p_EthLink->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, p_EthLink->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(p_EthLink->Alias)+1;
            return 1;
        }
    }
    if (strcmp(ParamName, "Name") == 0)
    {
        if ( AnscSizeOfString(p_EthLink->Name) < *pUlSize)
        {
            AnscCopyString(pValue, p_EthLink->Name);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(p_EthLink->Name)+1;
            return 1;
        }
    }
    if (strcmp(ParamName, "LowerLayers") == 0)
    {
        if ( AnscSizeOfString(p_EthLink->LowerLayers) < *pUlSize)
        {   
            AnscCopyString(pValue, p_EthLink->LowerLayers);
            return 0;
        }
        else
        {   
            *pUlSize = AnscSizeOfString(p_EthLink->LowerLayers)+1;
            return 1;
        }

        return 0;
    }
    if (strcmp(ParamName, "MACAddress") == 0)
    {
        //Get MAC from corresponding interface

        if (EthLink_GetMacAddr(p_EthLink) == ANSC_STATUS_SUCCESS)
        {
            if ( AnscSizeOfString(p_EthLink->MACAddress) < *pUlSize)
            {
                AnscCopyString(pValue, p_EthLink->MACAddress);
                return 0;
            }
            else
            {
                *pUlSize = AnscSizeOfString(p_EthLink->MACAddress)+1;
                return 1;
            }
        }
        AnscCopyString(pValue, p_EthLink->MACAddress);
        return 0;
    }
    if (strcmp(ParamName, "X_RDK_BaseInterface") == 0)
    {
        if ( AnscSizeOfString(p_EthLink->BaseInterface) < *pUlSize)
        {   
            AnscCopyString(pValue, p_EthLink->BaseInterface);
            return 0;
        }
        else
        {   
            *pUlSize = AnscSizeOfString(p_EthLink->BaseInterface)+1;
            return 1;
        }

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EthLink_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{

    PDML_ETHERNET             p_EthLink  = (PDML_ETHERNET) hInsContext;
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        //Check whether enable param changed or not
        if( bValue == p_EthLink->Enable )
        {
            return TRUE;
        }

        /* save update to backup */
        p_EthLink->Enable  = bValue;

        if(!p_EthLink->Enable)
        {
            EthLink_Disable(p_EthLink);
        }
        else
        {
            EthLink_Enable(p_EthLink);
        }

        return TRUE;
    }

    if (strcmp(ParamName, "PriorityTagging") == 0)
    {
        p_EthLink->PriorityTagging = bValue;
        return TRUE;
    }

    if (strcmp(ParamName, "X_RDK_Refresh") == 0)
    {
        //Handle Ethernet Link Refresh
        if( TRUE == bValue )
        {
            if (p_EthLink->Enable == TRUE)
            {
                INT iErrorCode = -1;
                pthread_t VlanThreadId;

                iErrorCode = pthread_create( &VlanThreadId, NULL, &EthLink_RefreshHandleThread, (void *)p_EthLink );
                if( 0 != iErrorCode )
                {
                    CcspTraceInfo(("%s %d - Failed to Start VLAN Refresh thread EC:%d\n", __FUNCTION__, __LINE__, iErrorCode ));
                    return FALSE;
                }
            }
        }
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EthLink_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EthLink_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PDML_ETHERNET              p_EthLink      = (PDML_ETHERNET   )hInsContext;

    /* check the parameter name and set the corresponding value */
   
    if (strcmp(ParamName, "Alias") == 0)
    {
        AnscCopyString(p_EthLink->Alias, pString);
        return TRUE;
    }
    if (strcmp(ParamName, "Name") == 0)
    {
        AnscCopyString(p_EthLink->Name, pString);
        return TRUE;
    }
    if (strcmp(ParamName, "LowerLayers") == 0)
    {
        AnscCopyString(p_EthLink->LowerLayers, pString);
        return TRUE;
    }
    if (strcmp(ParamName, "MACAddress") == 0)
    {
        AnscCopyString(p_EthLink->MACAddress, pString);
        return TRUE;
    }
    if (strcmp(ParamName, "X_RDK_BaseInterface") == 0)
    {
        AnscCopyString(p_EthLink->BaseInterface, pString);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EthLink_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation.

                ULONG*                      puLength
                The output length of the param name.

    return:     TRUE if there's no validation.

**********************************************************************/

BOOL
EthLink_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthLink_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

ULONG
EthLink_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    
    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthLink_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

ULONG
EthLink_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS  returnStatus  = ANSC_STATUS_SUCCESS;

    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    Device.Ethernet.Link.{i}.X_RDK_Marking.{i}.

    *  Marking_GetEntryCount
    *  Marking_GetEntry
    *  Marking_IsUpdated
    *  Marking_Synchronize
    *  Marking_GetParamBoolValue
    *  Marking_GetParamIntValue
    *  Marking_GetParamUlongValue
    *  Marking_GetParamStringValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        Marking_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/

ULONG
Marking_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PDML_ETHERNET                   p_EthLink     = (PDML_ETHERNET   )hInsContext;

    if(p_EthLink == NULL) {
        return 0;
    }

    return p_EthLink->NumberofMarkingEntries;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        Marking_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/

ANSC_HANDLE
Marking_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PDML_ETHERNET                   p_EthLink     = (PDML_ETHERNET   )hInsContext;

    if ( nIndex < p_EthLink->NumberofMarkingEntries)
    {
        *pInsNumber = (nIndex + 1);
        return (ANSC_HANDLE)(p_EthLink->pstDataModelMarking + nIndex);
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Marking_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/

BOOL
Marking_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    BOOL    bIsUpdated   = TRUE;
    return bIsUpdated;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        Marking_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

ULONG
Marking_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS  returnStatus = ANSC_STATUS_SUCCESS;
    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Marking_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Marking_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MARKING   p_EthMarking  = (PCOSA_DML_MARKING)hInsContext;

    if(p_EthMarking == NULL) {
        CcspTraceError(("%s %d - Invalid memory \n",__FUNCTION__, __LINE__));
        return FALSE;
    }

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SKBPort") == 0)
    {
        *puLong = p_EthMarking->SKBPort;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Marking_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Marking_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    PCOSA_DML_MARKING   p_EthMarking  = (PCOSA_DML_MARKING)hInsContext;

    if(p_EthMarking == NULL) {
        CcspTraceError(("%s %d - Invalid memory \n",__FUNCTION__, __LINE__));
        return FALSE;
    }
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "EthernetPriorityMark") == 0)
    {
        *pInt =  p_EthMarking->EthernetPriorityMark;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
