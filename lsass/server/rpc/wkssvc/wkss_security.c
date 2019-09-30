/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

/*
 * Copyright © BeyondTrust Software 2004 - 2019
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * BEYONDTRUST MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING TERMS AS
 * WELL. IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT WITH
 * BEYONDTRUST, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE TERMS OF THAT
 * SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE APACHE LICENSE,
 * NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU HAVE QUESTIONS, OR WISH TO REQUEST
 * A COPY OF THE ALTERNATE LICENSING TERMS OFFERED BY BEYONDTRUST, PLEASE CONTACT
 * BEYONDTRUST AT beyondtrust.com/contact
 */

/*
 * Copyright (C) BeyondTrust Software. All rights reserved.
 *
 * Module Name:
 *
 *        wkssvc_security.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        WksSvc server security descriptor functions
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


static
DWORD
WkssSrvCreateServerDacl(
    PACL *ppDacl
    );


static
DWORD
WkssSrvCreateDacl(
    PACL *ppDacl,
    PACCESS_LIST pList
    );


DWORD
WkssSrvInitServerSecurityDescriptor(
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = NULL;
    PSID pOwnerSid = NULL;
    PSID pGroupSid = NULL;
    PACL pDacl = NULL;

    BAIL_ON_INVALID_PTR(ppSecDesc, dwError);

    dwError = LwAllocateMemory(SECURITY_DESCRIPTOR_ABSOLUTE_MIN_SIZE,
                               OUT_PPVOID(&pSecDesc));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCreateSecurityDescriptorAbsolute(
                                    pSecDesc,
                                    SECURITY_DESCRIPTOR_REVISION);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = LwAllocateWellKnownSid(WinLocalSystemSid,
                                   NULL,
                                   &pOwnerSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetOwnerSecurityDescriptor(
                                    pSecDesc,
                                    pOwnerSid,
                                    FALSE);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pGroupSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetGroupSecurityDescriptor(
                                    pSecDesc,
                                    pGroupSid,
                                    FALSE);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = WkssSrvCreateServerDacl(&pDacl);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetDaclSecurityDescriptor(
                                    pSecDesc,
                                    TRUE,
                                    pDacl,
                                    FALSE);

    *ppSecDesc = pSecDesc;

cleanup:
    if (dwError == ERROR_SUCCESS &&
        ntStatus != STATUS_SUCCESS)
    {
        dwError = LwNtStatusToWin32Error(ntStatus);
    }

    return dwError;

error:
    LW_SAFE_FREE_MEMORY(pSecDesc);

    *ppSecDesc = NULL;
    goto cleanup;
}


static
DWORD
WkssSrvCreateServerDacl(
    PACL *ppDacl
    )
{
    ACCESS_MASK SystemAccessMask = STANDARD_RIGHTS_REQUIRED |
                                   WKSSVC_ACCESS_GET_INFO_1 |
                                   WKSSVC_ACCESS_GET_INFO_2 |
                                   WKSSVC_ACCESS_SET_INFO_1 |
                                   WKSSVC_ACCESS_SET_INFO_2 |
                                   WKSSVC_ACCESS_JOIN_DOMAIN |
                                   WKSSVC_ACCESS_RENAME_MACHINE;

    ACCESS_MASK AdminAccessMask = STANDARD_RIGHTS_REQUIRED |
                                  WKSSVC_ACCESS_GET_INFO_1 |
                                  WKSSVC_ACCESS_GET_INFO_2 |
                                  WKSSVC_ACCESS_SET_INFO_1 |
                                  WKSSVC_ACCESS_SET_INFO_2 |
                                  WKSSVC_ACCESS_JOIN_DOMAIN |
                                  WKSSVC_ACCESS_RENAME_MACHINE;

    ACCESS_MASK AuthenticatedAccessMask = STANDARD_RIGHTS_READ |
                                          WKSSVC_ACCESS_GET_INFO_1 |
                                          WKSSVC_ACCESS_GET_INFO_2;

    ACCESS_MASK WorldAccessMask = STANDARD_RIGHTS_READ |
                                  WKSSVC_ACCESS_GET_INFO_1;

    DWORD dwError = ERROR_SUCCESS;
    DWORD dwSystemSidLen = 0;
    DWORD dwBuiltinAdminsSidLen = 0;
    DWORD dwAuthenticatedSidLen = 0;
    DWORD dwWorldSidLen = 0;
    PSID pSystemSid = NULL;
    PSID pBuiltinAdminsSid = NULL;
    PSID pAuthenticatedSid = NULL;
    PSID pWorldSid = NULL;
    PACL pDacl = NULL;

    ACCESS_LIST AccessList[] = {
        {
            .ppSid        = &pSystemSid,
            .AccessMask   = SystemAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pBuiltinAdminsSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pAuthenticatedSid,
            .AccessMask   = AuthenticatedAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pWorldSid,
            .AccessMask   = WorldAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = NULL,
            .AccessMask   = 0,
            .ulAccessType = 0
        }
    };

    /* create local system sid */
    dwError = LwAllocateWellKnownSid(WinLocalSystemSid,
                                   NULL,
                                   &pSystemSid,
                                   &dwSystemSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create administrators sid */
    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pBuiltinAdminsSid,
                                   &dwBuiltinAdminsSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create authenticated users sid */
    dwError = LwAllocateWellKnownSid(WinAuthenticatedUserSid,
                                   NULL,
                                   &pAuthenticatedSid,
                                   &dwAuthenticatedSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create world (everyone) sid */
    dwError = LwAllocateWellKnownSid(WinWorldSid,
                                   NULL,
                                   &pWorldSid,
                                   &dwWorldSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = WkssSrvCreateDacl(&pDacl,
                                AccessList);
    BAIL_ON_LSA_ERROR(dwError);

    *ppDacl = pDacl;

cleanup:
    LW_SAFE_FREE_MEMORY(pSystemSid);
    LW_SAFE_FREE_MEMORY(pBuiltinAdminsSid);
    LW_SAFE_FREE_MEMORY(pAuthenticatedSid);
    LW_SAFE_FREE_MEMORY(pWorldSid);

    return dwError;

error:
    *ppDacl = NULL;

    goto cleanup;
}


static
DWORD
WkssSrvCreateDacl(
    PACL *ppDacl,
    PACCESS_LIST pList
    )
{
    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD dwDaclSize = 0;
    PACL pDacl = NULL;
    DWORD i = 0;
    ULONG ulSidSize = 0;

    dwDaclSize += ACL_HEADER_SIZE;

    for (i = 0; pList[i].ppSid && (*pList[i].ppSid); i++)
    {
        ulSidSize = RtlLengthSid(*(pList[i].ppSid));

        if (pList[i].ulAccessType == ACCESS_ALLOWED_ACE_TYPE)
        {
            dwDaclSize += ulSidSize + sizeof(ACCESS_ALLOWED_ACE);
        }
        else if (pList[i].ulAccessType == ACCESS_DENIED_ACE_TYPE)
        {
            dwDaclSize += ulSidSize + sizeof(ACCESS_DENIED_ACE);
        }
    }

    dwError = LwAllocateMemory(dwDaclSize,
                               OUT_PPVOID(&pDacl));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCreateAcl(pDacl, dwDaclSize, ACL_REVISION);
    BAIL_ON_NT_STATUS(ntStatus);

    for (i = 0; pList[i].ppSid && (*pList[i].ppSid); i++)
    {
        if (pList[i].ulAccessType == ACCESS_ALLOWED_ACE_TYPE)
        {
            ntStatus = RtlAddAccessAllowedAceEx(pDacl,
                                                ACL_REVISION,
                                                0,
                                                pList[i].AccessMask,
                                                *(pList[i].ppSid));
        }
        else if (pList[i].ulAccessType == ACCESS_DENIED_ACE_TYPE)
        {
            ntStatus = RtlAddAccessDeniedAceEx(pDacl,
                                               ACL_REVISION,
                                               0,
                                               pList[i].AccessMask,
                                               *(pList[i].ppSid));
        }

        BAIL_ON_NT_STATUS(ntStatus);
    }

    *ppDacl = pDacl;

cleanup:
    if (ntStatus == STATUS_SUCCESS &&
        dwError != ERROR_SUCCESS)
    {
        dwError = LwNtStatusToWin32Error(ntStatus);
    }

    return dwError;

error:
    LW_SAFE_FREE_MEMORY(pDacl);
    *ppDacl = NULL;

    goto cleanup;
}


DWORD
WkssSrvDestroyServerSecurityDescriptor(
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = NULL;
    PSID pOwnerSid = NULL;
    BOOLEAN bOwnerDefaulted = FALSE;
    PSID pPrimaryGroupSid = NULL;
    BOOLEAN bPrimaryGroupDefaulted = FALSE;
    PACL pDacl = NULL;
    BOOLEAN bDaclPresent = FALSE;
    BOOLEAN bDaclDefaulted = FALSE;
    PACL pSacl = NULL;
    BOOLEAN bSaclPresent = FALSE;
    BOOLEAN bSaclDefaulted = FALSE;

    BAIL_ON_INVALID_PTR(ppSecDesc, dwError);

    pSecDesc = *ppSecDesc;
    if (pSecDesc == NULL)
    {
        dwError = ERROR_SUCCESS;
        goto cleanup;
    }

    ntStatus = RtlGetOwnerSecurityDescriptor(pSecDesc,
                                             &pOwnerSid,
                                             &bOwnerDefaulted);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = RtlGetGroupSecurityDescriptor(pSecDesc,
                                             &pPrimaryGroupSid,
                                             &bPrimaryGroupDefaulted);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = RtlGetDaclSecurityDescriptor(pSecDesc,
                                            &bDaclPresent,
                                            &pDacl,
                                            &bDaclDefaulted);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = RtlGetSaclSecurityDescriptor(pSecDesc,
                                            &bSaclPresent,
                                            &pSacl,
                                            &bSaclDefaulted);
    BAIL_ON_NT_STATUS(ntStatus);

cleanup:
    LW_SAFE_FREE_MEMORY(pOwnerSid);
    LW_SAFE_FREE_MEMORY(pPrimaryGroupSid);

    if (bDaclPresent)
    {
        LW_SAFE_FREE_MEMORY(pDacl);
    }

    if (bSaclPresent)
    {
        LW_SAFE_FREE_MEMORY(pSacl);
    }

    LW_SAFE_FREE_MEMORY(pSecDesc);
    *ppSecDesc = NULL;

    return dwError;

error:
    goto cleanup;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
