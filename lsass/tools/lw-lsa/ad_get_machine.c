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
 *        ad_get_machine.c
 *
 * Abstract:
 *
 *        BeyondTrust Security and Authentication Subsystem (LSASS)
 *        AD Provider Get Machine Info
 *
 * Authors: Danilo Almeida (dalmeida@likewise.com)
 *
 */

#include <lsa/ad.h>
#include "common.h"
#include <stdarg.h>
#include "lsautils.h"
#include <stdio.h>
#include <string.h>

static
VOID
ShowUsage(
    IN PCSTR pszProgramName,
    IN int ExitCode
    );

static
PCSTR
PopNextOption(
    IN OUT PLW_ARGV_CURSOR Cursor
    )
{
    PCSTR option = LwArgvCursorPeek(Cursor);

    if (option)
    {
        if (!strcmp("--", option))
        {
            option = NULL;
            LwArgvCursorPop(Cursor);
        }
        else if (strncmp("-", option, 1))
        {
            option = NULL;
        }
        else
        {
            LwArgvCursorPop(Cursor);
        }
    }

    return option;
}

static
BOOLEAN
IsHelpOption(
    IN PCSTR Option
    )
{
    return (!strcmp(Option, "--help") ||
            !strcmp(Option, "-h") ||
            !strcmp(Option, "-?"));
}

static
BOOLEAN
IsHelpCommand(
    IN PCSTR Command
    )
{
    return !strcmp(Command, "help");
}

static
VOID
ShowUsageError(
    IN PCSTR pszProgramName
    )
{
    ShowUsage(pszProgramName, 1);
}

static
VOID
ShowUsageHelp(
    IN PCSTR pszProgramName
    )
{
    ShowUsage(pszProgramName, 0);
}

static
inline
VOID
PrintSuperUserWarningOnError(
    IN DWORD dwError
    )
{
    if ((dwError == LW_ERROR_ACCESS_DENIED) ||
        (dwError == ERROR_ACCESS_DENIED))
    {
        if (geteuid() != 0)
        {
            fprintf(stderr, "You are not super-user.  Access was denied.\n");
        }
    }
}

static
VOID
PrintAccountInfo(
    IN PLSA_MACHINE_ACCOUNT_INFO_A pAccountInfo
    )
{
    printf(""
           "  DNS Domain Name: %s\n"
           "  NetBIOS Domain Name: %s\n"
           "  Domain SID: %s\n"
           "  SAM Account Name: %s\n"
           "  FQDN: %s\n"
           "  Account Flags: 0x%08x (%u)\n"
           "  Key Version: %u\n"
           "  Last Change Time: %lld\n"
           "",
           LW_PRINTF_STRING(pAccountInfo->DnsDomainName),
           LW_PRINTF_STRING(pAccountInfo->NetbiosDomainName),
           LW_PRINTF_STRING(pAccountInfo->DomainSid),
           LW_PRINTF_STRING(pAccountInfo->SamAccountName),
           LW_PRINTF_STRING(pAccountInfo->Fqdn),
           pAccountInfo->AccountFlags, pAccountInfo->AccountFlags,
           pAccountInfo->KeyVersionNumber,
           (long long int)pAccountInfo->LastChangeTime);
}

static
VOID
PrintPasswordInfo(
    IN PLSA_MACHINE_PASSWORD_INFO_A pPasswordInfo
    )
{
    PrintAccountInfo(&pPasswordInfo->Account);
    printf("  Password: %s\n", pPasswordInfo->Password);
}

static
DWORD
DoGetMachineAccount(
    IN PCSTR pszDnsDomainName
    )
{
    DWORD dwError = 0;
    HANDLE hLsaConnection = NULL;
    PLSA_MACHINE_ACCOUNT_INFO_A pAccountInfo = NULL;

    dwError = LsaOpenServer(&hLsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaAdGetMachineAccountInfo(
                    hLsaConnection,
                    pszDnsDomainName,
                    &pAccountInfo);
    PrintSuperUserWarningOnError(dwError);
    BAIL_ON_LSA_ERROR(dwError);

    printf("Machine Account Info:\n");
    PrintAccountInfo(pAccountInfo);

error:
    if (pAccountInfo)
    {
        LsaAdFreeMachineAccountInfo(pAccountInfo);
    }

    if (hLsaConnection)
    {
        LsaCloseServer(hLsaConnection);
    }

    return dwError;
}

static
DWORD
DoGetMachinePassword(
    IN PCSTR pszDnsDomainName
    )
{
    DWORD dwError = 0;
    HANDLE hLsaConnection = NULL;
    PLSA_MACHINE_PASSWORD_INFO_A pPasswordInfo = NULL;

    dwError = LsaOpenServer(&hLsaConnection);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaAdGetMachinePasswordInfo(
                    hLsaConnection,
                    pszDnsDomainName,
                    &pPasswordInfo);
    PrintSuperUserWarningOnError(dwError);
    BAIL_ON_LSA_ERROR(dwError);

    printf("Machine Password Info:\n");
    PrintPasswordInfo(pPasswordInfo);

error:
    if (pPasswordInfo)
    {
        LsaAdFreeMachinePasswordInfo(pPasswordInfo);
    }

    if (hLsaConnection)
    {
        LsaCloseServer(hLsaConnection);
    }

    return dwError;
}

static
VOID
ShowUsage(
    IN PCSTR pszProgramName,
    IN int ExitCode
    )
{
    PCSTR pszUseProgramName = Basename(pszProgramName);
    printf("Usage: %s <account | password> [DNS-DOMAIN-NAME]\n", pszUseProgramName);
    exit(ExitCode);
}

int
ad_get_machine_main(
    int argc,
    const char* argv[]
    )
{
    DWORD dwError = 0;
    PCSTR programName = NULL;
    PCSTR option = NULL;
    PCSTR command = NULL;
    LW_ARGV_CURSOR cursor;

    LwArgvCursorInit(&cursor, argc, argv);
    programName = LwArgvCursorPop(&cursor);

    // Process Options:

    for (;;)
    {
        option = PopNextOption(&cursor);
        if (!option)
        {
            break;
        }
        else if (IsHelpOption(option))
        {
            ShowUsageHelp(programName);
        }
        else
        {
            fprintf(stderr, "Unrecognized option: %s\n", option);
            ShowUsageError(programName);
        }
    }

    // Process Command:

    command = LwArgvCursorPop(&cursor);
    if (!command)
    {
        fprintf(stderr, "Missing command.\n");
        ShowUsageError(programName);
    }
    else if (IsHelpCommand(command))
    {
        ShowUsageHelp(programName);
    }
    else if (!strcmp(command, "account"))
    {
        PCSTR domainName = LwArgvCursorPop(&cursor);
        if (LwArgvCursorRemaining(&cursor))
        {
            fprintf(stderr, "Too many arguments.\n");
            ShowUsageError(programName);
        }
        dwError = DoGetMachineAccount(domainName);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else if (!strcmp(command, "password"))
    {
        PCSTR domainName = LwArgvCursorPop(&cursor);
        if (LwArgvCursorRemaining(&cursor))
        {
            fprintf(stderr, "Too many arguments.\n");
            ShowUsageError(programName);
        }
        dwError = DoGetMachinePassword(domainName);
        BAIL_ON_LSA_ERROR(dwError);
    }
    else
    {
        fprintf(stderr, "Unrecognized command: %s\n", command);
        ShowUsageError(programName);
    }

error:
    if (dwError)
    {
        PrintErrorMessage(dwError);
    }

    return dwError;
}
