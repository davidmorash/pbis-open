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
 *        netr_unjoindomain2.c
 *
 * Abstract:
 *
 *        BeyondTrust Workstation Service (wkssvc) rpc server
 *
 *        NetrUnjoinDomain2 server API
 *
 * Authors: Rafal Szczesniak <rafal@likewise.com>
 */

#include "includes.h"


WINERROR
NetrSrvUnjoinDomain2(
    /* [in] */ handle_t                  hBinding,
    /* [in] */ PWSTR                     pwszServerName,
    /* [in] */ PWSTR                     pwszAccountName,
    /* [in] */ PENC_JOIN_PASSWORD_BUFFER pPassword,
    /* [in] */ DWORD                     dwUnjoinFlags
    )
{
    const DWORD dwRequiredAccessRights = WKSSVC_ACCESS_JOIN_DOMAIN;

    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    WKSS_SRV_CONTEXT SrvCtx = {0};
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = gpWkssSecDesc;
    GENERIC_MAPPING GenericMapping = {0};
    DWORD dwAccessGranted = 0;
    PWSTR pwszPassword = NULL;
    size_t sPasswordLen = 0;
    PSTR pszUsername = NULL;
    PSTR pszPassword = NULL;
    LSA_AD_IPC_LEAVE_DOMAIN_REQ Request = {0};
    HANDLE hServer = NULL;
    LWMsgDataContext *pDataCtx = NULL;
    size_t sInputBlobSize = 0;
    PVOID pInputBlob = NULL;
    DWORD dwOutputBlobSize = 0;
    PVOID pOutputBlob = NULL;

    dwError = WkssSrvInitAuthInfo(hBinding,
                                  &SrvCtx);
    BAIL_ON_LSA_ERROR(dwError);

    if (!RtlAccessCheck(pSecDesc,
                        SrvCtx.pUserToken,
                        dwRequiredAccessRights,
                        0,
                        &GenericMapping,
                        &dwAccessGranted,
                        &ntStatus))
    {
        BAIL_ON_NT_STATUS(ntStatus);
    }

    dwError = WkssSrvDecryptPasswordBlob(&SrvCtx,
                                         pPassword,
                                         NULL,
                                         0,
                                         &pwszPassword);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwWc16sLen(pwszPassword, &sPasswordLen);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwWc16sToMbs(pwszAccountName, &pszUsername);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwWc16sToMbs(pwszPassword, &pszPassword);
    BAIL_ON_LSA_ERROR(dwError);

    Request.pszUsername  = pszUsername;
    Request.pszPassword  = pszPassword;

    if (dwUnjoinFlags & NETSETUP_ACCT_DELETE)
    {
        Request.dwFlags |= LSA_NET_LEAVE_DOMAIN_ACCT_DELETE;
    }

    dwError = MAP_LWMSG_ERROR(lwmsg_data_context_new(NULL, &pDataCtx));
    BAIL_ON_LSA_ERROR(dwError);

    dwError = MAP_LWMSG_ERROR(lwmsg_data_marshal_flat_alloc(
                                      pDataCtx,
                                      LsaAdIPCGetLeaveDomainReqSpec(),
                                      &Request,
                                      &pInputBlob,
                                      &sInputBlobSize));
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvOpenServer(geteuid(),
                               getegid(),
                               getpid(),
                               &hServer);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvProviderIoControl(hServer,
                                      LSA_PROVIDER_TAG_AD,
                                      LSA_AD_IO_LEAVEDOMAIN,
                                      sInputBlobSize,
                                      pInputBlob,
                                      &dwOutputBlobSize,
                                      &pOutputBlob);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:
    if (hServer)
    {
        LsaSrvCloseServer(hServer);
    }

    if (pDataCtx)
    {
        lwmsg_data_context_delete(pDataCtx);
    }

    WkssSrvFreeAuthInfo(&SrvCtx);

    LW_SECURE_FREE_STRING(pszPassword);
    LW_SECURE_FREE_WSTRING(pwszPassword);
    LW_SECURE_FREE_MEMORY(pInputBlob, sInputBlobSize);
    LW_SAFE_FREE_MEMORY(pszUsername);

    return (WINERROR)dwError;

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
