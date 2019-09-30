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
 *        ipc_protocol.c
 *
 * Abstract:
 *
 *        BeyondTrust Security and Authentication Subsystem (LSASS)
 *
 *        Local Privileges Inter-process communication API
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 *
 */

#include "ipc.h"


#define LWMSG_PWSTR \
    LWMSG_POINTER_BEGIN, \
    LWMSG_UINT16(WCHAR), \
    LWMSG_POINTER_END, \
    LWMSG_ATTR_STRING

static LWMsgTypeSpec gLsaLuidAndAttributesSpec[] =
{
    LWMSG_STRUCT_BEGIN(LUID_AND_ATTRIBUTES),
    LWMSG_MEMBER_STRUCT_BEGIN(LUID_AND_ATTRIBUTES, Luid),
    LWMSG_MEMBER_UINT32(LUID, LowPart),
    LWMSG_MEMBER_UINT32(LUID, HighPart),
    LWMSG_STRUCT_END,
    LWMSG_MEMBER_UINT32(LUID_AND_ATTRIBUTES, Attributes),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};



LWMsgTypeSpec gLsaPrivsIPCEnumPrivilegesSidsReqSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_REQ),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_REQ, NumSids),
    LWMSG_MEMBER_POINTER_BEGIN(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_REQ, ppszSids),
    LWMSG_PSTR,
    LWMSG_ATTR_NOT_NULL,
    LWMSG_POINTER_END,
    LWMSG_ATTR_LENGTH_MEMBER(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_REQ, NumSids),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};

LWMsgTypeSpec gLsaPrivsIPCEnumPrivilegesSidsRespSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_RESP),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_RESP, NumPrivileges),
    LWMSG_MEMBER_POINTER_BEGIN(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_RESP, pPrivileges),
    LWMSG_TYPESPEC(gLsaLuidAndAttributesSpec),
    LWMSG_POINTER_END,
    LWMSG_ATTR_LENGTH_MEMBER(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_RESP, NumPrivileges),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_ENUM_PRIVILEGES_SIDS_RESP, SystemAccessRights),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};

LWMsgTypeSpec gLsaPrivsIPCAddAccountRightsReqSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_ADD_ACCOUNT_RIGHTS_REQ),
    LWMSG_MEMBER_PSTR(LSA_PRIVS_IPC_ADD_ACCOUNT_RIGHTS_REQ, pszSid),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_ADD_ACCOUNT_RIGHTS_REQ, NumAccountRights),
    LWMSG_MEMBER_POINTER_BEGIN(LSA_PRIVS_IPC_ADD_ACCOUNT_RIGHTS_REQ, ppwszAccountRights),
    LWMSG_PWSTR,
    LWMSG_ATTR_NOT_NULL,
    LWMSG_POINTER_END,
    LWMSG_ATTR_LENGTH_MEMBER(LSA_PRIVS_IPC_ADD_ACCOUNT_RIGHTS_REQ, NumAccountRights),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};

LWMsgTypeSpec gLsaPrivsIPCRemoveAccountRightsReqSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ),
    LWMSG_MEMBER_PSTR(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, pszSid),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, RemoveAll),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, NumAccountRights),
    LWMSG_MEMBER_POINTER_BEGIN(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, ppwszAccountRights),
    LWMSG_PWSTR,
    LWMSG_ATTR_NOT_NULL,
    LWMSG_POINTER_END,
    LWMSG_ATTR_LENGTH_MEMBER(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, NumAccountRights),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};

LWMsgTypeSpec gLsaPrivsIPCEnumAccountRightsReqSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_ENUM_ACCOUNT_RIGHTS_REQ),
    LWMSG_MEMBER_PSTR(LSA_PRIVS_IPC_REMOVE_ACCOUNT_RIGHTS_REQ, pszSid),
    LWMSG_TYPE_END
};

LWMsgTypeSpec gLsaPrivsIPCEnumAccountRightsRespSpec[] =
{
    LWMSG_STRUCT_BEGIN(LSA_PRIVS_IPC_ENUM_ACCOUNT_RIGHTS_RESP),
    LWMSG_MEMBER_UINT32(LSA_PRIVS_IPC_ENUM_ACCOUNT_RIGHTS_RESP, NumAccountRights),
    LWMSG_MEMBER_POINTER_BEGIN(LSA_PRIVS_IPC_ENUM_ACCOUNT_RIGHTS_RESP, ppwszAccountRights),
    LWMSG_PWSTR,
    LWMSG_ATTR_NOT_NULL,
    LWMSG_POINTER_END,
    LWMSG_ATTR_LENGTH_MEMBER(LSA_PRIVS_IPC_ENUM_ACCOUNT_RIGHTS_RESP, NumAccountRights),
    LWMSG_STRUCT_END,
    LWMSG_TYPE_END
};
