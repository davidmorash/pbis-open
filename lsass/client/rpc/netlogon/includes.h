/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

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

#include "config.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>
#include <pthread.h>

#include <lwio/lwio.h>

#include <dce/rpc.h>
#include <dce/smb.h>
#include <dce/dcethread.h>
#include <dce/schannel.h>
#include <wc16str.h>
#include <wc16printf.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/rc4.h>
#include <openssl/hmac.h>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <lw/ntstatus.h>
#include <lw/winerror.h>
#include <lwmem.h>
#include <lwstr.h>
#include <lwbuffer.h>

#include <lsautils.h>
#include <lw/rpc/samr.h>
#include <lw/rpc/netlogon.h>
#include <lw/rpcmacros.h>
#include <lw/swab.h>

#include "netrdefs.h"
#include "netr_credentials.h"
#include "netr_memory.h"
#include "netr_stubmemory.h"
#include "netlogon_h.h"

#include "externs.h"
