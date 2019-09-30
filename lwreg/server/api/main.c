/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

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
 *        main.c
 *
 * Abstract:
 *
 *        Registry
 *
 *        Server API main
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *          Wei Fu (wfu@likewise.com)
 */
#include "api.h"


static LWMsgDispatchSpec gMessageHandlers[] =
{
    LWMSG_DISPATCH_BLOCK(REG_Q_ENUM_ROOT_KEYSW, RegSrvIpcEnumRootKeysW),
    LWMSG_DISPATCH_BLOCK(REG_Q_ENUM_VALUEW, RegSrvIpcEnumValueW),
    LWMSG_DISPATCH_BLOCK(REG_Q_GET_VALUEW, RegSrvIpcGetValueW),
    LWMSG_DISPATCH_BLOCK(REG_Q_OPEN_KEYW_EX, RegSrvIpcOpenKeyExW),
    LWMSG_DISPATCH_BLOCK(REG_Q_SET_VALUEW_EX, RegSrvIpcSetValueExW),
    LWMSG_DISPATCH_BLOCK(REG_Q_CREATE_KEY_EX, RegSrvIpcCreateKeyEx),
    LWMSG_DISPATCH_BLOCK(REG_Q_CLOSE_KEY, RegSrvIpcCloseKey),
    LWMSG_DISPATCH_BLOCK(REG_Q_DELETE_KEY, RegSrvIpcDeleteKey),
    LWMSG_DISPATCH_BLOCK(REG_Q_DELETE_KEY_VALUE, RegSrvIpcDeleteKeyValue),
    LWMSG_DISPATCH_BLOCK(REG_Q_DELETE_TREE, RegSrvIpcDeleteTree),
    LWMSG_DISPATCH_BLOCK(REG_Q_DELETE_VALUE, RegSrvIpcDeleteValue),
    LWMSG_DISPATCH_BLOCK(REG_Q_ENUM_KEYW_EX, RegSrvIpcEnumKeyExW),
    LWMSG_DISPATCH_BLOCK(REG_Q_QUERY_INFO_KEYW, RegSrvIpcQueryInfoKeyW),
    LWMSG_DISPATCH_BLOCK(REG_Q_QUERY_MULTIPLE_VALUES, RegSrvIpcQueryMultipleValues),
    LWMSG_DISPATCH_BLOCK(REG_Q_SET_KEY_SECURITY, RegSrvIpcSetKeySecurity),
    LWMSG_DISPATCH_BLOCK(REG_Q_GET_KEY_SECURITY, RegSrvIpcGetKeySecurity),
    LWMSG_DISPATCH_BLOCK(REG_Q_SET_VALUEW_ATTRIBUTES, RegSrvIpcSetValueAttibutesW),
    LWMSG_DISPATCH_BLOCK(REG_Q_GET_VALUEW_ATTRIBUTES, RegSrvIpcGetValueAttibutesW),
    LWMSG_DISPATCH_BLOCK(REG_Q_DELETE_VALUEW_ATTRIBUTES, RegSrvIpcDeleteValueAttibutesW),
    LWMSG_DISPATCH_END
};


DWORD
RegSrvApiInit(
    VOID
    )
{
    DWORD dwError = 0;

    dwError = LwNtStatusToWin32Error(LwMapSecurityInitialize());
    BAIL_ON_REG_ERROR(dwError);

    dwError = LwNtStatusToWin32Error(LwMapSecurityCreateContext(&gpRegLwMapSecurityCtx));
    BAIL_ON_REG_ERROR(dwError);

#if defined(REG_USE_FILE)
    dwError = FileProvider_Initialize(&gpRegProvider);
    BAIL_ON_REG_ERROR(dwError);
#elif defined(REG_USE_SQLITE)
    dwError = SqliteProvider_Initialize(&gpRegProvider, ROOT_KEYS);
    BAIL_ON_REG_ERROR(dwError);
#elif defined(REG_USE_MEMORY)
    dwError = MemProvider_Initialize(&gpRegProvider, ROOT_KEYS);
    BAIL_ON_REG_ERROR(dwError);
#endif

    // make sure gpRegProvider is not NULL
    if (!gpRegProvider)
    {
        dwError = ERROR_INTERNAL_ERROR;
        BAIL_ON_REG_ERROR(dwError);
    }

cleanup:

    return dwError;

error:

    goto cleanup;
}

static
void
RegSrvFreeProviders()
{
    return;
}

DWORD
RegSrvApiShutdown(
    VOID
    )
{
    RegSrvFreeProviders();

#if defined(REG_USE_FILE)
    FileProvider_Shutdown(gpRegProvider);
#elif defined(REG_USE_SQLITE)
    SqliteProvider_Shutdown(gpRegProvider);
#elif defined(REG_USE_MEMORY)
    MemProvider_Shutdown(gpRegProvider);
#endif

    LwMapSecurityFreeContext(&gpRegLwMapSecurityCtx);
    LwMapSecurityCleanup();

    return 0;
}

LWMsgDispatchSpec*
RegSrvGetDispatchSpec(
    void
    )
{
    return gMessageHandlers;
}

