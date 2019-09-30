/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

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
 *        samr_enumdomains.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Client Interface
 *
 *        SamrEnumDomains function
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


NTSTATUS
SamrEnumDomains(
    IN  SAMR_BINDING     hBinding,
    IN  CONNECT_HANDLE   hConn,
    IN OUT UINT32       *pResume,
    IN  UINT32           Size,
    OUT PWSTR          **pppwszNames,
    OUT UINT32          *pCount
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS ntRetStatus = STATUS_SUCCESS;
    PENTRY_ARRAY pDomains = NULL;
    UINT32 Resume = 0;
    UINT32 Count = 0;
    PWSTR *ppwszNames = NULL;
    DWORD dwOffset = 0;
    DWORD dwSpaceLeft = 0;
    DWORD dwSize = 0;

    BAIL_ON_INVALID_PTR(hBinding, ntStatus);
    BAIL_ON_INVALID_PTR(hConn, ntStatus);
    BAIL_ON_INVALID_PTR(pResume, ntStatus);
    BAIL_ON_INVALID_PTR(pppwszNames, ntStatus);
    BAIL_ON_INVALID_PTR(pCount, ntStatus);

    Resume = *pResume;

    DCERPC_CALL(ntStatus, cli_SamrEnumDomains((handle_t)hBinding,
                                              hConn,
                                              &Resume,
                                              Size,
                                              &pDomains,
                                              &Count));

    /* Preserve returned status code */
    ntRetStatus = ntStatus;

    /* Status other than success doesn't have to mean failure here */
    if (ntRetStatus != STATUS_SUCCESS &&
        ntRetStatus != STATUS_MORE_ENTRIES)
    {
        BAIL_ON_NT_STATUS(ntStatus);
    }

    if (pDomains)
    {
        ntStatus = SamrAllocateNames(NULL,
                                     &dwOffset,
                                     NULL,
                                     pDomains,
                                     &dwSize);
        BAIL_ON_NT_STATUS(ntStatus);

        dwSpaceLeft = dwSize;
        dwSize      = 0;
        dwOffset    = 0;

        ntStatus = SamrAllocateMemory(OUT_PPVOID(&ppwszNames),
                                      dwSpaceLeft);
        BAIL_ON_NT_STATUS(ntStatus);

        ntStatus = SamrAllocateNames(ppwszNames,
                                     &dwOffset,
                                     &dwSpaceLeft,
                                     pDomains,
                                     &dwSize);
        BAIL_ON_NT_STATUS(ntStatus);
    }

    *pResume     = Resume;
    *pCount      = Count;
    *pppwszNames = ppwszNames;

cleanup:
    if (pDomains)
    {
        SamrFreeStubEntryArray(pDomains);
    }

    if (ntStatus == STATUS_SUCCESS &&
        (ntRetStatus == STATUS_SUCCESS ||
         ntRetStatus == STATUS_MORE_ENTRIES))
    {
        ntStatus = ntRetStatus;
    }

    return ntStatus;

error:
    if (ppwszNames)
    {
        SamrFreeMemory(ppwszNames);
    }

    if (pResume)
    {
        *pResume = 0;
    }

    if (pCount)
    {
        *pCount = 0;
    }

    if (pppwszNames)
    {
        *pppwszNames = NULL;
    }

    goto cleanup;
}
