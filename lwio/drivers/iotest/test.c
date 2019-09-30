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
 *        test.c
 *
 * Abstract:
 *
 *        IO Test Driver
 *
 * Authors: Danilo Almeida (dalmeida@likewise.com)
 */

#include "includes.h"
#include <lwio/ioapi.h>
#include "lwthreads.h"

NTSTATUS
ItTestStartup(
    IN PCSTR pszPath
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    IO_FILE_HANDLE fileHandle = NULL;
    IO_STATUS_BLOCK ioStatusBlock = { 0 };
    IO_FILE_NAME fileName = { 0 };
    UNICODE_STRING filePath = { 0 };
    PIO_CREATE_SECURITY_CONTEXT securityContext = NULL;

    status = IoSecurityCreateSecurityContextFromUidGid(&securityContext, 0, 0, NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = RtlUnicodeStringAllocateFromCString(&filePath, pszPath);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    fileName.Name = filePath;

    status = IoCreateFile(&fileHandle,
                          NULL,
                          &ioStatusBlock,
                          securityContext,
                          &fileName,
                          NULL,
                          NULL,
                          0,
                          0,
                          0,
                          0,
                          0,
                          0,
                          NULL,
                          0,
                          NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    LW_RTL_UNICODE_STRING_FREE(&filePath);

    if (fileHandle)
    {
        IoCloseFile(fileHandle);
    }

    IoSecurityDereferenceSecurityContext(&securityContext);

    LOG_LEAVE_IF_STATUS_EE(status, EE);
    return status;
}

NTSTATUS
ItTestSyncCreate(
    VOID
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    IO_FILE_HANDLE fileHandle = NULL;
    IO_STATUS_BLOCK ioStatusBlock = { 0 };
    PIO_CREATE_SECURITY_CONTEXT securityContext = NULL;
    IO_FILE_NAME filename = { 0 };

    IO_LOG_ENTER("");

    status = RtlUnicodeStringAllocateFromCString(&filename.Name, IOTEST_PATH_ALLOW);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = IoSecurityCreateSecurityContextFromUidGid(&securityContext, 0, 0, NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = IoCreateFile(
                    &fileHandle,
                    NULL,
                    &ioStatusBlock,
                    securityContext,
                    &filename,
                    NULL,
                    NULL,
                    SYNCHRONIZE,
                    0,
                    0,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    FILE_OPEN_IF,
                    0,
                    NULL,
                    0,
                    NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    if (fileHandle)
    {
        IoCloseFile(fileHandle);
    }
    LW_RTL_UNICODE_STRING_FREE(&filename.Name);
    IoSecurityDereferenceSecurityContext(&securityContext);

    IO_LOG_LEAVE_STATUS_EE(status, EE);

    return status;
}

NTSTATUS
ItTestAsyncCreate(
    IN BOOLEAN UseAsyncCall,
    IN BOOLEAN DoCancel
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    IO_FILE_HANDLE fileHandle = NULL;
    IO_STATUS_BLOCK ioStatusBlock = { 0 };
    PIO_CREATE_SECURITY_CONTEXT securityContext = NULL;
    IO_FILE_NAME filename = { 0 };
    IO_ASYNC_CONTROL_BLOCK asyncControlBlock = { 0 };
    LW_RTL_EVENT event = LW_RTL_EVENT_ZERO_INITIALIZER;
    PIO_ASYNC_CONTROL_BLOCK useAsyncControlBlock = NULL;

    IO_LOG_ENTER("");

    status = LwRtlInitializeEvent(&event);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = RtlUnicodeStringAllocateFromCString(&filename.Name, IOTEST_PATH_ASYNC);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = IoSecurityCreateSecurityContextFromUidGid(&securityContext, 0, 0, NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    asyncControlBlock.Callback = ItAsyncCompleteSetEvent;
    asyncControlBlock.CallbackContext = &event;

    if (UseAsyncCall)
    {
        useAsyncControlBlock = &asyncControlBlock;
    }

    status = IoCreateFile(
                    &fileHandle,
                    useAsyncControlBlock,
                    &ioStatusBlock,
                    securityContext,
                    &filename,
                    NULL,
                    NULL,
                    SYNCHRONIZE,
                    0,
                    0,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    FILE_OPEN_IF,
                    0,
                    NULL,
                    0,
                    NULL);
    if (STATUS_PENDING == status)
    {
        LW_RTL_WINDOWS_TIME timeout = - LW_RTL_WINDOWS_TIMESPAN_SECOND;
        PLW_RTL_WINDOWS_TIME useTimeout = NULL;

        LWIO_LOG_DEBUG("Waiting for async completion");
        if (DoCancel)
        {
            useTimeout = &timeout;
        }
        if (!LwRtlWaitEvent(&event, useTimeout))
        {
            LWIO_LOG_DEBUG("Cancelling");
            IoCancelAsyncCancelContext(asyncControlBlock.AsyncCancelContext);
            LwRtlWaitEvent(&event, NULL);
        }
        LWIO_LOG_DEBUG("Got async completion");
        status = ioStatusBlock.Status;
        LWIO_ASSERT(NT_SUCCESS_OR_NOT(status));
        LWIO_ASSERT(IS_BOTH_OR_NEITHER(STATUS_SUCCESS == status, fileHandle));

        IoDereferenceAsyncCancelContext(&asyncControlBlock.AsyncCancelContext);
    }
    else
    {
        LWIO_LOG_DEBUG("Completed synchronously");
    }
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    if (fileHandle)
    {
        NTSTATUS localStatus = IoCloseFile(fileHandle);
        LWIO_ASSERT(!localStatus);
    }
    LW_RTL_UNICODE_STRING_FREE(&filename.Name);
    IoSecurityDereferenceSecurityContext(&securityContext);
    LwRtlCleanupEvent(&event);

    IO_LOG_LEAVE_STATUS_EE(status, EE);

    return status;
}

NTSTATUS
ItTestRundown(
    VOID
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    IO_FILE_HANDLE fileHandle = NULL;
    IO_ASYNC_CONTROL_BLOCK asyncControlBlock = { 0 };
    IO_STATUS_BLOCK ioStatusBlock = { 0 };
    PIO_CREATE_SECURITY_CONTEXT securityContext = NULL;
    IO_FILE_NAME filename = { 0 };
    LW_RTL_EVENT event = LW_RTL_EVENT_ZERO_INITIALIZER;

    IO_LOG_ENTER("");

    status = LwRtlInitializeEvent(&event);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = RtlUnicodeStringAllocateFromCString(&filename.Name, IOTEST_PATH_ALLOW);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = IoSecurityCreateSecurityContextFromUidGid(&securityContext, 0, 0, NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = IoCreateFile(
                    &fileHandle,
                    NULL,
                    &ioStatusBlock,
                    securityContext,
                    &filename,
                    NULL,
                    NULL,
                    SYNCHRONIZE,
                    0,
                    0,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    FILE_OPEN_IF,
                    0,
                    NULL,
                    0,
                    NULL);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    asyncControlBlock.Callback = ItAsyncCompleteSetEvent;
    asyncControlBlock.CallbackContext = &event;

    status = IoDeviceIoControlFile(
                    fileHandle,
                    &asyncControlBlock,
                    &ioStatusBlock,
                    IOTEST_IOCTL_TEST_SLEEP,
                    NULL,
                    0,
                    NULL,
                    0);
    if (STATUS_PENDING == status)
    {
        LWIO_LOG_DEBUG("calling close");
        status = IoCloseFile(fileHandle);
        LWIO_ASSERT(STATUS_SUCCESS == status);
        GOTO_CLEANUP_ON_STATUS_EE(status, EE);
        fileHandle = NULL;
        LWIO_LOG_DEBUG("close finished");

        LWIO_LOG_DEBUG("Waiting for async completion");
        LwRtlWaitEvent(&event, NULL);
        LWIO_LOG_DEBUG("Got async completion");

        status = ioStatusBlock.Status;
        LWIO_ASSERT((STATUS_SUCCESS == status) || (STATUS_CANCELLED == status));

        IoDereferenceAsyncCancelContext(&asyncControlBlock.AsyncCancelContext);
    }
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    if (fileHandle)
    {
        NTSTATUS localStatus = IoCloseFile(fileHandle);
        LWIO_ASSERT(NT_SUCCESS(localStatus));
    }
    LW_RTL_UNICODE_STRING_FREE(&filename.Name);
    IoSecurityDereferenceSecurityContext(&securityContext);

    IO_LOG_LEAVE_STATUS_EE(status, EE);

    return status;
}

NTSTATUS
ItTestSleep(
    IN PIRP pIrp,
    IN ULONG Seconds
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    int EE = 0;

    status = ItDispatchAsync(pIrp, Seconds, ItSimpleSuccessContinueCallback, NULL);
    GOTO_CLEANUP_EE(EE);

cleanup:
    LOG_LEAVE_IF_STATUS_EE(status, EE);
    return status;
}
