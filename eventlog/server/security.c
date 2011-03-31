/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Centeris Corporation 2004-2007
 * Copyright (C) Likewise Software 2007
 * All rights reserved.
 *
 * Authors: Rafal Szczesniak (rafal@likewisesoftware.com)
 *
 * Eventlog server security wrapper
 *
 */

#include "includes.h"
#include <gssapi/gssapi.h>


DWORD
LWICheckSecurity(
    handle_t        hBindingHandle,
    ACCESS_MASK dwAccessMask
    )
{
    DWORD dwError = ERROR_SUCCESS;
    volatile unsigned32 rpcError;
    PACCESS_TOKEN        pUserToken = NULL;

    TRY
    {
        rpc_binding_inq_access_token_caller(
            hBindingHandle,
            &pUserToken,
            (unsigned32*)&rpcError);
    }
    CATCH_ALL
    ENDTRY;

    BAIL_ON_DCE_ERROR(dwError, rpcError);

    dwError = EVTCheckAllowed(
            pUserToken, 
            dwAccessMask);
    BAIL_ON_EVT_ERROR(dwError);

error:
    if (pUserToken)
    {
        RtlReleaseAccessToken(&pUserToken);
    }
    return dwError;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
