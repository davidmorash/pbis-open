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
 *        cfg.c
 *
 * Abstract:
 *
 *        NTLM Security registry settings
 *
 * Authors: Kyle Stemen <kstemen@likewise.com>
 *
 */

#include "ntlmsrvapi.h"
#include "lsasrvutils.h"

DWORD
NtlmReadRegistry(
    OUT PNTLM_CONFIG pConfig
    )
{
    DWORD dwError = LW_ERROR_SUCCESS;

    pConfig->bSendNTLMv2 = FALSE;
    pConfig->bSupportUnicode = TRUE;
    pConfig->bSupportNTLM2SessionSecurity = TRUE;
    pConfig->bSupportKeyExchange = TRUE;
    pConfig->bSupport56bit = TRUE;
    pConfig->bSupport128bit = TRUE;

    LWREG_CONFIG_ITEM configItems[] =
    {
        {
            "SendNTLMv2",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSendNTLMv2,
            NULL
        },
        {
            "SupportUnicode",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSupportUnicode,
            NULL
        },
        {
            "SupportNTLM2SessionSecurity",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSupportNTLM2SessionSecurity,
            NULL
        },
        {
            "SupportKeyExchange",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSupportKeyExchange,
            NULL
        },
        {
            "Support56bit",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSupport56bit,
            NULL
        },
        {
            "Support128bit",
            TRUE,
            LwRegTypeBoolean,
            0,
            MAXDWORD,
            NULL,
            &pConfig->bSupport128bit,
            NULL
        },
    };

    dwError = RegProcessConfig(
                "Services\\lsass\\Parameters\\NTLM",
                "Policy\\Services\\lsass\\Parameters\\NTLM",
                configItems,
                sizeof(configItems)/sizeof(configItems[0]));
    BAIL_ON_LSA_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;
}
