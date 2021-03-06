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
 * Module Name:
 *
 *        security.h
 *
 * Abstract:
 *
 *        Security token API (public header)
 *
 * Authors: Brian Koropoff (bkoropoff@likewisesoftware.com)
 *
 */
#ifndef __LWMSG_SECURITY_H__
#define __LWMSG_SECURITY_H__

#include <lwmsg/status.h>
#include <lwmsg/common.h>

#include <stdlib.h>

/**
 * @file security.h
 * @brief Security token API
 */

/**
 * @defgroup security Security tokens
 * @ingroup public
 * @brief Manipulate and inspect security tokens
 *
 * The security token interface provides an abstract means
 * to access and set identification and credential information
 * on an association.  A security token always has a type
 * which is expressed as a simple string constant.  Once
 * the type of a security token is established, it's contents
 * can be accessed through functions specific to that type,
 * e.g. #lwmsg_local_token_get_eid() for tokens of type "local".
 *
 * Security tokens can always be copied, compared, and deleted
 * regardless of their type.
 */

/**
 * @ingroup security
 * @brief Security token object
 *
 * An opaque structure representing a security token
 */
typedef struct LWMsgSecurityToken LWMsgSecurityToken;

/**
 * @ingroup security
 * @brief Query security token type
 *
 * Gets the type of the specified security token as a
 * simple string constant.
 * 
 * @param[in] token the token object
 * @return a string constant identifying the type of the token
 */
const char*
lwmsg_security_token_get_type(
    LWMsgSecurityToken* token
    );

/**
 * @ingroup security
 * @brief Compare two security tokens
 *
 * Compares two security tokens for equality.  The equality
 * logic used is determined by the first token.
 *
 * @param[in] token the first token
 * @param[in] other the other token
 * @retval #LWMSG_TRUE the tokens are equal
 * @retval #LWMSG_FALSE the tokens are not equal
 */
LWMsgBool
lwmsg_security_token_equal(
    LWMsgSecurityToken* token,
    LWMsgSecurityToken* other
    );

/**
 * @ingroup security
 * @brief Test access permissions
 *
 * Determines if another security token is permitted to
 * access resources owned by the first security token.
 * This test is more general than the test for equality
 * and may succeed for tokens that are not equal.
 * The logic used is determined by the first token.
 *
 * @param[in] token the first token
 * @param[in] other the other token
 * @retval #LWMSG_TRUE access is permitted
 * @retval #LWMSG_FALSE access is not permitted
 */
LWMsgBool
lwmsg_security_token_can_access(
    LWMsgSecurityToken* token,
    LWMsgSecurityToken* other
    );

/**
 * @ingroup security
 * @brief Copy security token
 *
 * Creates an exact copy of a security token.
 *
 * @param[in] token the token object
 * @param[out] copy the token copy
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_security_token_copy(
    LWMsgSecurityToken* token,
    LWMsgSecurityToken** copy
    );

/**
 * @ingroup security
 * @brief Delete security token
 *
 * Deletes a security token.
 *
 * @param[in,out] token the token object
 */
void
lwmsg_security_token_delete(
    LWMsgSecurityToken* token
    );

#endif
