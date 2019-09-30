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

#include "includes.h"
#include "lam-group.h"

static const DWORD MAX_NUM_GROUPS = 500;

void
LsaNssFreeLastGroup(
        VOID
        )
{
    LW_SAFE_FREE_MEMORY(gNssState.pLastGroup);
}

DWORD
LsaNssAllocateGroupFromInfo1(
        PLSA_GROUP_INFO_1 pInfo,
        struct group** ppResult
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    // Do not free directly. Free pStartMem instead on error
    struct group* pResult = NULL;
    void *pStartMem = NULL;
    // Do not free
    PBYTE pMem = NULL;
    size_t sRequiredSize = 0;
    size_t sMemberCount = 0;
    size_t sLen = 0;

    sRequiredSize += sizeof(struct group);
    sRequiredSize += strlen(pInfo->pszName) + 1;
    sRequiredSize += strlen(pInfo->pszPasswd) + 1;
    while (pInfo->ppszMembers[sMemberCount] != NULL)
    {
        sRequiredSize += strlen(pInfo->ppszMembers[sMemberCount]) + 1;
        sMemberCount++;
    }
    sRequiredSize += sizeof(char *) * (sMemberCount + 1);

    dwError = LwAllocateMemory(
            sRequiredSize,
            &pStartMem);
    BAIL_ON_LSA_ERROR(dwError);

    pMem = pStartMem;
    pResult = (struct group *)pMem;
    pMem += sizeof(struct group);

    // Get memory for the gr_mem field early so that is boundary aligned
    pResult->gr_mem = (char **)pMem;
    pMem += sizeof(char *) * (sMemberCount + 1);

    sLen = strlen(pInfo->pszName);
    pResult->gr_name = (char *)pMem;
    memcpy(pResult->gr_name, pInfo->pszName, sLen + 1);
    pMem += sLen + 1;

    sLen = strlen(pInfo->pszPasswd);
    pResult->gr_passwd = (char *)pMem;
    memcpy(pResult->gr_passwd, pInfo->pszPasswd, sLen + 1);
    pMem += sLen + 1;

    pResult->gr_gid = pInfo->gid;

    sMemberCount = 0;
    while (pInfo->ppszMembers[sMemberCount] != NULL)
    {
        sLen = strlen(pInfo->ppszMembers[sMemberCount]);
        pResult->gr_mem[sMemberCount] = (char *)pMem;
        memcpy(pResult->gr_mem[sMemberCount], pInfo->ppszMembers[sMemberCount], sLen + 1);
        pMem += sLen + 1;
        sMemberCount++;
    }
    pResult->gr_mem[sMemberCount] = NULL;
    assert(pMem == pStartMem + sRequiredSize);

    *ppResult = pResult;

cleanup:

    return dwError;

error:

    *ppResult = NULL;
    LW_SAFE_FREE_MEMORY(pStartMem);

    goto cleanup;
}

DWORD
LsaNssAllocateGroupFromInfo0(
        PLSA_GROUP_INFO_0 pInfo,
        struct group** ppResult
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    // Do not free directly. Free pStartMem instead on error
    struct group* pResult = NULL;
    void *pStartMem = NULL;
    // Do not free
    PBYTE pMem = NULL;
    size_t sRequiredSize = 0;
    size_t sLen = 0;

    sRequiredSize += sizeof(struct group);
    sRequiredSize += strlen(pInfo->pszName) + 1;
    sRequiredSize += 2;

    dwError = LwAllocateMemory(
            sRequiredSize,
            &pStartMem);
    BAIL_ON_LSA_ERROR(dwError);

    pMem = pStartMem;
    pResult = (struct group *)pMem;
    pMem += sizeof(struct group);

    pResult->gr_mem = NULL;

    sLen = strlen(pInfo->pszName);
    pResult->gr_name = (char *)pMem;
    memcpy(pResult->gr_name, pInfo->pszName, sLen + 1);
    pMem += sLen + 1;

    sLen = 1;
    pResult->gr_passwd = (char *)pMem;
    memcpy(pResult->gr_passwd, "*", sLen + 1);
    pMem += sLen + 1;

    pResult->gr_gid = pInfo->gid;

    assert(pMem == pStartMem + sRequiredSize);

    *ppResult = pResult;

cleanup:

    return dwError;

error:

    *ppResult = NULL;
    LW_SAFE_FREE_MEMORY(pStartMem);

    goto cleanup;
}

static 
struct group *_LsaNssGetGrGid(gid_t gid)
{
    DWORD dwError = LW_ERROR_SUCCESS;
    PLSA_GROUP_INFO_1 pInfo = NULL;
    const DWORD dwInfoLevel = 1;
    struct group *pResult = NULL;

    dwError = LsaNssCommonEnsureConnected(&lsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaFindGroupById(
                lsaConnection.hLsaConnection,
                gid,
                LSA_FIND_FLAGS_NSS,
                dwInfoLevel,
                (PVOID*)&pInfo);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaNssAllocateGroupFromInfo1(
                pInfo,
                &pResult);
    BAIL_ON_LSA_ERROR(dwError);

    LsaNssFreeLastGroup();
    gNssState.pLastGroup = pResult;

cleanup:

    if (pInfo != NULL)
    {
        LsaFreeGroupInfo(
		dwInfoLevel,
                (PVOID)pInfo);
    }

    if (dwError != LW_ERROR_SUCCESS)
    {
        LsaNssMapErrorCode(dwError, &errno);
    }
    return pResult;

error:
    LsaNssCommonCloseConnection(&lsaConnection);

    goto cleanup;
}

struct group *LsaNssGetGrGid(gid_t gid)
{
    struct group *rc = NULL;
    
    NSS_LOCK();
    
    rc = _LsaNssGetGrGid(gid);
    
    NSS_UNLOCK();
    
    return rc;
}

static 
struct group *_LsaNssGetGrNam(PCSTR pszName)
{
    DWORD dwError = LW_ERROR_SUCCESS;
    PLSA_GROUP_INFO_1 pInfo = NULL;
    const DWORD dwInfoLevel = 1;
    struct group *pResult = NULL;

    if (LsaShouldIgnoreGroup(pszName))
    {
        dwError = LW_ERROR_NOT_HANDLED;
        BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaNssCommonEnsureConnected(&lsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaFindGroupByName(
                lsaConnection.hLsaConnection,
                pszName,
                LSA_FIND_FLAGS_NSS,
                dwInfoLevel,
                (PVOID*)&pInfo);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaNssAllocateGroupFromInfo1(
                pInfo,
                &pResult);
    BAIL_ON_LSA_ERROR(dwError);

    LsaNssFreeLastGroup();
    gNssState.pLastGroup = pResult;

cleanup:

    if (pInfo != NULL)
    {
        LsaFreeGroupInfo(
                dwInfoLevel,
                pInfo);
    }

    if (dwError != LW_ERROR_SUCCESS)
    {
        LsaNssMapErrorCode(dwError, &errno);
    }
    return pResult;

error:
    LsaNssCommonCloseConnection(&lsaConnection);

    goto cleanup;
}

struct group *LsaNssGetGrNam(PCSTR pszName)
{
    struct group *rc = NULL;
    
    NSS_LOCK();
    
    rc = _LsaNssGetGrNam(pszName);
    
    NSS_UNLOCK();
    
    return rc;
}

static
struct group *
_LsaNssGetGrAcct(
        PVOID pId,
        int iType
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    PLSA_GROUP_INFO_0 pInfo = NULL;
    const DWORD dwInfoLevel = 0;
    struct group *pResult = NULL;

    dwError = LsaNssCommonEnsureConnected(&lsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    if (iType == 0)
    {
        dwError = LsaFindGroupByName(
                    lsaConnection.hLsaConnection,
                    (PSTR)pId,
                    LSA_FIND_FLAGS_NSS,
                    dwInfoLevel,
                    (PVOID*)&pInfo);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else if (iType == 1)
    {
        dwError = LsaFindGroupById(
                    lsaConnection.hLsaConnection,
                    *(gid_t *)pId,
                    LSA_FIND_FLAGS_NSS,
                    dwInfoLevel,
                    (PVOID*)&pInfo);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else
    {
        dwError = EINVAL;
        BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaNssAllocateGroupFromInfo0(
                pInfo,
                &pResult);
    BAIL_ON_LSA_ERROR(dwError);

    LsaNssFreeLastGroup();
    gNssState.pLastGroup = pResult;

cleanup:

    if (pInfo != NULL)
    {
        LsaFreeGroupInfo(
                dwInfoLevel,
                pInfo);
    }

    if (dwError != LW_ERROR_SUCCESS)
    {
        LsaNssMapErrorCode(dwError, &errno);
    }
    return pResult;

error:
    LsaNssCommonCloseConnection(&lsaConnection);

    goto cleanup;
}

struct group *
LsaNssGetGrAcct(
        PVOID pId,
        int iType
        )
{
    struct group *rc = NULL;
    
    NSS_LOCK();
    
    rc = _LsaNssGetGrAcct(pId, iType);
    
    NSS_UNLOCK();
    
    return rc;
}

static
PSTR
_LsaNssGetGrSet(
        PSTR pszName
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    DWORD dwGroupCount = 0;
    gid_t* pGids = NULL;
    char szGidNumBuf[16];
    size_t sRequiredLen = 0;
    DWORD dwIndex = 0;
    PSTR pszResult = NULL;
    // Do not free
    PSTR pszPos = NULL;

    dwError = LsaNssCommonEnsureConnected(&lsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaGetGidsForUserByName(
            lsaConnection.hLsaConnection,
            pszName,
            &dwGroupCount,
            &pGids);
    BAIL_ON_LSA_ERROR(dwError);

    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        // Add space for the gid number in decimal
        sprintf(szGidNumBuf, "%lu", (unsigned long)pGids[dwIndex]);
        sRequiredLen += strlen(szGidNumBuf);
    }
    // Add space for a comma between each gid and a terminating NULL
    sRequiredLen += dwGroupCount;

    dwError = LwAllocateMemory(
            sRequiredLen,
            (PVOID*)&pszResult);
    BAIL_ON_LSA_ERROR(dwError);

    pszPos = pszResult;
    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        if (dwIndex > 0)
        {
            *pszPos++ = ',';
        }
        sprintf(pszPos, "%lu", (unsigned long)pGids[dwIndex]);
        pszPos += strlen(pszPos);
    }

cleanup:

    LW_SAFE_FREE_MEMORY(pGids);
    if (dwError != LW_ERROR_SUCCESS)
    {
        LsaNssMapErrorCode(dwError, &errno);
    }
    return pszResult;

error:

    LW_SAFE_FREE_MEMORY(pszResult);
    LsaNssCommonCloseConnection(&lsaConnection);

    goto cleanup;
}

PSTR
LsaNssGetGrSet(
        PSTR pszName
        )
{
    PSTR rc = NULL;
    
    NSS_LOCK();
    
    rc = _LsaNssGetGrSet(pszName);
    
    NSS_UNLOCK();
    
    return rc;
}

DWORD
LsaNssListGroups(
        HANDLE hLsaConnection,
        attrval_t* pResult
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    const DWORD dwInfoLevel = 0;
    const DWORD dwEnumLimit = MAX_NUM_GROUPS;
    DWORD dwIndex = 0;
    DWORD dwTotalIndex = 0;
    DWORD dwGroupsFound = 0;
    HANDLE hResume = (HANDLE)NULL;
    size_t sRequiredMem = 1;
    size_t sUsedMem = 0;
    PLSA_GROUP_INFO_0* ppGroupList = NULL;
    PSTR pszListStart = NULL;
    PSTR pDisabled = getenv(DISABLE_NSS_ENUMERATION_ENV);
    // Do not free
    PSTR pszPos = NULL;

    LSA_LOG_PAM_DEBUG("Enumerating groups");

    if (pDisabled) 
    {
        pResult->attr_flag = ENOENT;
        goto error;
    }

    dwError = LsaBeginEnumGroups(
                hLsaConnection,
                dwInfoLevel,
                dwEnumLimit,
                LSA_FIND_FLAGS_NSS,
                &hResume);
    BAIL_ON_LSA_ERROR(dwError);

    do {
        if (ppGroupList != NULL)
        {
            LsaFreeGroupInfoList(
                    dwInfoLevel,
                    (PVOID*)ppGroupList,
                    dwGroupsFound);

            ppGroupList = NULL;
            dwGroupsFound = 0;
        }

        dwError = LsaEnumGroups(
                    hLsaConnection,
                    hResume,
                    &dwGroupsFound,
                    (PVOID**)&ppGroupList);
        BAIL_ON_LSA_ERROR(dwError);

        if (dwGroupsFound == 0) continue;

        LSA_LOG_PAM_DEBUG("Found %u groups", (unsigned int)dwGroupsFound);

        for (dwIndex = 0; dwIndex < dwGroupsFound; dwIndex++)
        {
            if (ppGroupList[dwIndex]->pszName) 
            {
                sRequiredMem += strlen(ppGroupList[dwIndex]->pszName) + 1;
            }
        }

        dwError = LwReallocMemory(
                    pszListStart,
                    (PVOID*)&pszListStart,
                    sRequiredMem);
        BAIL_ON_LSA_ERROR(dwError);

        pszPos = pszListStart + sUsedMem;

        for (dwIndex = 0; dwIndex < dwGroupsFound; dwIndex++)
        {
            if (ppGroupList[dwIndex]->pszName) 
            {
                strcpy(pszPos, ppGroupList[dwIndex]->pszName);
                pszPos += strlen(pszPos) + 1;
            }
        }
        
        sUsedMem = pszPos - pszListStart;
        
        dwTotalIndex += dwGroupsFound;

    } while (dwGroupsFound > 0);
    
    if (pszPos) {
        *pszPos++ = 0;
        assert(pszPos == pszListStart + sRequiredMem);
    }

    pResult->attr_un.au_char = pszListStart;
    pResult->attr_flag = 0;

cleanup:

    if (hResume != (HANDLE)NULL)
    {
        LsaEndEnumGroups(
                hLsaConnection,
                hResume);
    }
    if (ppGroupList != NULL)
    {
        LsaFreeGroupInfoList(
                dwInfoLevel,
                (PVOID*)ppGroupList,
                dwGroupsFound);
    }

    return dwError;

error:

    pResult->attr_un.au_char = NULL;
    LW_SAFE_FREE_MEMORY(pszListStart);
    goto cleanup;
}

DWORD
LsaNssGetGidList(
        HANDLE hLsaConnection,
        PLSA_USER_INFO_2 pInfo,
        PSTR* ppszList
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    DWORD dwGroupCount = 0;
    PLSA_GROUP_INFO_0* ppGroupList = NULL;
    const DWORD dwGroupInfoLevel = 0;
    char szGidNumBuf[16];
    size_t sRequiredLen = 0;
    DWORD dwIndex = 0;
    PSTR pszResult = NULL;
    // Do not free
    PSTR pszPos = NULL;

    dwError = LsaGetGroupsForUserById(
        hLsaConnection,
        pInfo->uid,
        LSA_FIND_FLAGS_NSS,
        dwGroupInfoLevel,
        &dwGroupCount,
        (PVOID**)&ppGroupList);
    BAIL_ON_LSA_ERROR(dwError);

    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        // Add space for the gid number in decimal, plus NULL
        sprintf(szGidNumBuf, "%lu", (unsigned long)ppGroupList[dwIndex]->gid);
        sRequiredLen += strlen(szGidNumBuf) + 1;
    }
    // Add space for a list terminating NULL
    if (dwIndex == 0)
    {
        sRequiredLen++;
    }
    sRequiredLen++;

    dwError = LwAllocateMemory(
            sRequiredLen,
            (PVOID*)&pszResult);
    BAIL_ON_LSA_ERROR(dwError);

    pszPos = pszResult;
    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        sprintf(pszPos, "%lu", (unsigned long)ppGroupList[dwIndex]->gid);
        pszPos += strlen(pszPos) + 1;
    }
    // Add list terminating NULL
    if (dwIndex == 0)
    {
        *pszPos++ = 0;
    }
    *pszPos++ = 0;

    assert(pszPos == pszResult + sRequiredLen);
    *ppszList = pszResult;

cleanup:

    if (ppGroupList != NULL)
    {
        LsaFreeGroupInfoList(
                dwGroupInfoLevel,
                (PVOID*)ppGroupList,
                dwGroupCount);
    }
    return dwError;

error:

    *ppszList = NULL;
    LW_SAFE_FREE_MEMORY(pszResult);
    goto cleanup;
}

DWORD
LsaNssGetGroupList(
        HANDLE hLsaConnection,
        PLSA_USER_INFO_2 pInfo,
        PSTR* ppszList
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    DWORD dwGroupCount = 0;
    PLSA_GROUP_INFO_0* ppGroupList = NULL;
    const DWORD dwGroupInfoLevel = 0;
    size_t sRequiredLen = 0;
    DWORD dwIndex = 0;
    PSTR pszResult = NULL;
    // Do not free
    PSTR pszPos = NULL;

    dwError = LsaGetGroupsForUserById(
        hLsaConnection,
        pInfo->uid,
        LSA_FIND_FLAGS_NSS,
        dwGroupInfoLevel,
        &dwGroupCount,
        (PVOID**)&ppGroupList);
    BAIL_ON_LSA_ERROR(dwError);

    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        // Add space for the gid number in decimal, plus NULL
        sRequiredLen += strlen(ppGroupList[dwIndex]->pszName) + 1;
    }
    // Add space for a list terminating NULL
    if (dwIndex == 0)
    {
        sRequiredLen++;
    }
    sRequiredLen++;

    dwError = LwAllocateMemory(
            sRequiredLen,
            (PVOID*)&pszResult);
    BAIL_ON_LSA_ERROR(dwError);

    pszPos = pszResult;
    for (dwIndex = 0; dwIndex < dwGroupCount; dwIndex++)
    {
        strcpy(pszPos, ppGroupList[dwIndex]->pszName);
        pszPos += strlen(pszPos) + 1;
    }
    // Add list terminating NULL
    if (dwIndex == 0)
    {
        *pszPos++ = 0;
    }
    *pszPos++ = 0;

    assert(pszPos == pszResult + sRequiredLen);
    *ppszList = pszResult;

cleanup:

    if (ppGroupList != NULL)
    {
        LsaFreeGroupInfoList(
                dwGroupInfoLevel,
                (PVOID*)ppGroupList,
                dwGroupCount);
    }
    return dwError;

error:

    *ppszList = NULL;
    LW_SAFE_FREE_MEMORY(pszResult);
    goto cleanup;
}

DWORD
LsaNssFillMemberList(
        PLSA_GROUP_INFO_1 pInfo,
        PSTR* ppResult
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    size_t sIndex = 0;
    size_t sRequiredMem = 0;
    PSTR pszListStart = NULL;
    // Do not free
    PSTR pszPos = NULL;

    sRequiredMem = 1;
    for (sIndex = 0; pInfo->ppszMembers[sIndex]; sIndex++)
    {
        sRequiredMem += strlen(pInfo->ppszMembers[sIndex]) + 1;
    }
    if (sIndex == 0)
    {
        sRequiredMem++;
    }

    dwError = LwAllocateMemory(
                sRequiredMem,
                (PVOID*)&pszListStart);
    BAIL_ON_LSA_ERROR(dwError);
    pszPos = pszListStart;

    for (sIndex = 0; pInfo->ppszMembers[sIndex]; sIndex++)
    {
        strcpy(pszPos, pInfo->ppszMembers[sIndex]);
        pszPos += strlen(pszPos) + 1;
    }
    if (sIndex == 0)
    {
        *pszPos++ = 0;
    }
    *pszPos++ = 0;

    assert(pszPos == pszListStart + sRequiredMem);

    *ppResult = pszListStart;

cleanup:

    return dwError;

error:

    *ppResult = NULL;
    LW_SAFE_FREE_MEMORY(pszListStart);
    goto cleanup;
}

VOID
LsaNssGetGroupAttr(
        HANDLE hLsaConnection,
        PLSA_GROUP_INFO_1 pInfo,
        PSTR pszAttribute,
        attrval_t* pResult
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;

    if (!strcmp(pszAttribute, S_ID))
    {
        pResult->attr_un.au_int = pInfo->gid;
    }
    else if (!strcmp(pszAttribute, S_PWD))
    {
        dwError = LwAllocateString(
                    pInfo->pszPasswd,
                    &pResult->attr_un.au_char);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else if (!strcmp(pszAttribute, S_ADMIN))
    {
        pResult->attr_un.au_int = FALSE;
    }
    else if (!strcmp(pszAttribute, S_ADMS))
    {
        // Allocate an empty list, which is just two '\0's in a row
        dwError = LwAllocateMemory(
                    2,
                    (PVOID*)&pResult->attr_un.au_char);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else if (!strcmp(pszAttribute, S_USERS))
    {
        dwError = LsaNssFillMemberList(
                    pInfo,
                    &pResult->attr_un.au_char);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else if (!strcmp(pszAttribute, "SID"))
    {
        dwError = LwAllocateString(
                    pInfo->pszSid,
                    &pResult->attr_un.au_char);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else
    {
        dwError = EINVAL;
        BAIL_ON_LSA_ERROR(dwError);
    }

cleanup:

    if (dwError != LW_ERROR_SUCCESS)
    {
        LsaNssMapErrorCode(dwError, &pResult->attr_flag);
    }
    else
    {
        pResult->attr_flag = 0;
    }
    return;

error:

    goto cleanup;
}

DWORD
LsaNssGetGroupAttrs(
        HANDLE hLsaConnection,
        PSTR pszKey,
        PSTR* ppszAttributes,
        attrval_t* pResults,
        int iAttrCount
        )
{
    DWORD dwError = LW_ERROR_SUCCESS;
    const DWORD dwInfoLevel = 1;
    PLSA_GROUP_INFO_1 pInfo = NULL;
    int iIndex = 0;

    dwError = LsaFindGroupByName(
                hLsaConnection,
                pszKey,
                LSA_FIND_FLAGS_NSS,
                dwInfoLevel,
                (PVOID*)&pInfo);
    BAIL_ON_LSA_ERROR(dwError);

    for (iIndex = 0; iIndex < iAttrCount; iIndex++)
    {
        // This function stores the error in pResults[iIndex].attr_flag
        LsaNssGetGroupAttr(
            hLsaConnection,
            pInfo,
            ppszAttributes[iIndex],
            &pResults[iIndex]);
    }

cleanup:

    if (pInfo != NULL)
    {
        LsaFreeGroupInfo(
                dwInfoLevel,
                (PVOID)pInfo);
    }
    return dwError;

error:

    goto cleanup;
}
