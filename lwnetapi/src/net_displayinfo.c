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
 *        net_displayinfo.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Client Interface
 *
 *        NetAPI display info buffer handling functions
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


static
DWORD
NetAllocateDisplayUserInformation(
    PVOID                *ppBuffer,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    );


static
DWORD
NetAllocateDisplayMachineInformation(
    PVOID                *ppBuffer,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    );


static
DWORD
NetAllocateDisplayGroupInformation(
    PVOID                *ppBuffer,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    );


DWORD
NetAllocateDisplayInformation(
    PVOID                 pInfoBuffer,
    PDWORD                pdwSpaceLeft,
    DWORD                 dwLevel,
    SamrDisplayInfo      *pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    )
{
    DWORD dwError = ERROR_SUCCESS;
    PVOID pCursor = pInfoBuffer;
    DWORD iEntry = 0;
    

    switch (dwLevel)
    {
    case 1:
        for (iEntry = 0; iEntry < pSource->info1.count; iEntry++)
        {
            dwError = NetAllocateDisplayUserInformation(
                                        &pCursor,
                                        pdwSpaceLeft,
                                        &(pSource->info1.entries[iEntry]),
                                        pdwSize,
                                        eValidation);
        }
        break;

    case 2:
        for (iEntry = 0; iEntry < pSource->info2.count; iEntry++)
        {
            dwError = NetAllocateDisplayMachineInformation(
                                        &pCursor,
                                        pdwSpaceLeft,
                                        &(pSource->info2.entries[iEntry]),
                                        pdwSize,
                                        eValidation);
        }
        break;

    case 3:
        for (iEntry = 0; iEntry < pSource->info3.count; iEntry++)
        {
            dwError = NetAllocateDisplayGroupInformation(
                                        &pCursor,
                                        pdwSpaceLeft,
                                        &(pSource->info3.entries[iEntry]),
                                        pdwSize,
                                        eValidation);
        }
        break;

    default:
        dwError = ERROR_INVALID_LEVEL;
        break;
    }
    BAIL_ON_WIN_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;
}


static
DWORD
NetAllocateDisplayUserInformation(
    PVOID                *ppCursor,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    )
{
    DWORD dwError = ERROR_SUCCESS;
    PVOID pCursor = NULL;
    DWORD dwSpaceLeft = 0;
    DWORD dwSize = 0;
    SamrDisplayEntryFull *pEntry = (SamrDisplayEntryFull*)pSource;

    if (pdwSpaceLeft)
    {
        dwSpaceLeft = *pdwSpaceLeft;
    }

    if (pdwSize)
    {
        dwSize = *pdwSize;
    }

    if (ppCursor)
    {
        pCursor = *ppCursor;
    }

    /* usri1_name */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->account_name,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* usri1_comment */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->description,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* usri1_flags */
    dwError = NetAllocBufferUserFlagsFromAcbFlags(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   pEntry->account_flags,
                                   &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    ALIGN_PTR_IN_BUFFER(NET_DISPLAY_USER, usri1_flags,
                        pCursor, dwSize, dwSpaceLeft);

    /* usri1_full_name */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->full_name,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* usri1_user_id */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->rid,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    /* usri1_next_index */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->idx,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    ALIGN_PTR_IN_BUFFER(NET_DISPLAY_USER, usri1_next_index,
                        pCursor, dwSize, dwSpaceLeft);

    if (pdwSpaceLeft)
    {
        *pdwSpaceLeft = dwSpaceLeft;
    }

    if (pdwSize)
    {
        *pdwSize = dwSize;
    }

    if (ppCursor)
    {
        *ppCursor = pCursor;
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}


static
DWORD
NetAllocateDisplayMachineInformation(
    PVOID                *ppCursor,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    )
{
    DWORD dwError = ERROR_SUCCESS;
    PVOID pCursor = NULL;
    DWORD dwSpaceLeft = 0;
    DWORD dwSize = 0;
    SamrDisplayEntryGeneral *pEntry = (SamrDisplayEntryGeneral*)pSource;

    if (pdwSpaceLeft)
    {
        dwSpaceLeft = *pdwSpaceLeft;
    }

    if (pdwSize)
    {
        dwSize = *pdwSize;
    }

    if (ppCursor)
    {
        pCursor = *ppCursor;
    }

    /* usri2_name */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->account_name,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* usri2_comment */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->description,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* usri2_flags */
    dwError = NetAllocBufferUserFlagsFromAcbFlags(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   pEntry->account_flags,
                                   &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    /* usri2_user_id */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->rid,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    /* usri2_next_index */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->idx,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    ALIGN_PTR_IN_BUFFER(NET_DISPLAY_MACHINE, usri2_next_index,
                        pCursor, dwSize, dwSpaceLeft);

    if (pdwSpaceLeft)
    {
        *pdwSpaceLeft = dwSpaceLeft;
    }

    if (pdwSize)
    {
        *pdwSize = dwSize;
    }

    if (ppCursor)
    {
        *ppCursor = pCursor;
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}


static
DWORD
NetAllocateDisplayGroupInformation(
    PVOID                *ppCursor,
    PDWORD                pdwSpaceLeft,
    PVOID                 pSource,
    PDWORD                pdwSize,
    NET_VALIDATION_LEVEL  eValidation
    )
{
    DWORD dwError = ERROR_SUCCESS;
    PVOID pCursor = NULL;
    DWORD dwSpaceLeft = 0;
    DWORD dwSize = 0;
    SamrDisplayEntryGeneralGroup *pEntry = (SamrDisplayEntryGeneralGroup*)pSource;

    if (pdwSpaceLeft)
    {
        dwSpaceLeft = *pdwSpaceLeft;
    }

    if (pdwSize)
    {
        dwSize = *pdwSize;
    }

    if (ppCursor)
    {
        pCursor = *ppCursor;
    }

    /* grpi3_name */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->account_name,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* grpi3_comment */
    dwError = NetAllocBufferWC16StringFromUnicodeString(
                                   &pCursor,
                                   &dwSpaceLeft,
                                   &pEntry->description,
                                   &dwSize,
                                   eValidation
                                   );
    BAIL_ON_WIN_ERROR(dwError);

    /* grpi3_group_id */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->rid,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    /* grpi3_attributes */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->account_flags,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    /* grpi3_next_index */
    dwError = NetAllocBufferDword(&pCursor,
                              &dwSpaceLeft,
                              pEntry->idx,
                              &dwSize);
    BAIL_ON_WIN_ERROR(dwError);

    ALIGN_PTR_IN_BUFFER(NET_DISPLAY_GROUP, grpi3_next_index,
                        pCursor, dwSize, dwSpaceLeft);

    if (pdwSpaceLeft)
    {
        *pdwSpaceLeft = dwSpaceLeft;
    }

    if (pdwSize)
    {
        *pdwSize = dwSize;
    }

    if (ppCursor)
    {
        *ppCursor = pCursor;
    }

cleanup:
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
