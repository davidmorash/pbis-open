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
 * Authors: Rafal Szczesniak (rafal@likewisesoftware.com)
 */

#ifndef _SECDESC_P_H_
#define _SECDESC_P_H_


#define BAIL_ON_NTSTATUS_ERROR(err)               \
    do {                                          \
        if ((err) != STATUS_SUCCESS) {            \
            goto error;                           \
        }                                         \
    } while (0);


#define BAIL_ON_NULL_PTR(ptr)                     \
    do {                                          \
        if ((ptr) == NULL) {                      \
            status = STATUS_INSUFFICIENT_RESOURCES; \
            goto error;                           \
        }                                         \
    } while (0);


#define BAIL_ON_NULL_PTR_PARAM(ptr)               \
    do {                                          \
        if ((ptr) == NULL) {                      \
            status = STATUS_INVALID_PARAMETER;    \
            goto cleanup;                         \
        }                                         \
    } while (0);

/* Variation of the above that drops to the error label */

#define BAIL_ON_INVALID_PTR(ptr, err)                  \
    do {                                               \
        if ((ptr) == NULL) {                           \
            err = STATUS_INVALID_PARAMETER;            \
            goto error;                                \
        }                                              \
    } while (0);


#define ZERO_STRUCT(s)                            \
    do {                                          \
        memset((void*)&(s), 0, sizeof((s)));      \
    } while (0)


#endif /* _SECDESC_P_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
