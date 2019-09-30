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
 *        structs.h
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        Lsa rpc internal structures
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#ifndef _LSASRV_STRUCTS_H_
#define _LSASRV_STRUCTS_H_


/*
 * Domain cache entry types
 */
typedef enum {
    eDomainSid   = 1,
    eDomainName

} DOMAIN_KEY_TYPE;


typedef struct _DOMAIN_KEY
{
    DOMAIN_KEY_TYPE  eType;
    PSID             pSid;
    PWSTR            pwszName;

} DOMAIN_KEY, *PDOMAIN_KEY;


typedef struct _DOMAIN_ENTRY
{
    PWSTR          pwszName;
    PSID           pSid;
    LSA_BINDING    hLsaBinding;
    POLICY_HANDLE  hPolicy;

} DOMAIN_ENTRY, *PDOMAIN_ENTRY;


/*
 * Account names to lookup
 */
typedef struct _ACCOUNT_NAMES
{
    PWSTR  *ppwszNames;
    PDWORD  pdwIndices;
    DWORD   dwCount;

} ACCOUNT_NAMES, *PACCOUNT_NAMES;


/*
 * Account sids to lookup
 */
typedef struct _ACCOUNT_SIDS
{
    PSID   *ppSids;
    PDWORD  pdwIndices;
    DWORD   dwCount;

} ACCOUNT_SIDS, *PACCOUNT_SIDS;


#endif /* _LSASRV_STRUCTS_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
