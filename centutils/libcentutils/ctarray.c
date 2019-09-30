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

#include "config.h"
#include "ctbase.h"
#include "ctarray.h"

DWORD CTArrayConstruct(DynamicArray* array, size_t itemSize)
{
    DWORD ceError = ERROR_SUCCESS;
    array->size = 0;
    array->capacity = 32;

    BAIL_ON_CENTERIS_ERROR(ceError = CTAllocateMemory(array->capacity*itemSize, 
                                                      (PVOID*)&array->data));

error:
    return ceError;
}

DWORD CTSetCapacity(DynamicArray *array, size_t itemSize, size_t capacity)
{
    DWORD ceError = ERROR_SUCCESS;

    /* Resize the array */
    ceError = CTReallocMemory(array->data, &array->data, capacity * itemSize);
    BAIL_ON_CENTERIS_ERROR(ceError);
    array->capacity = capacity;
    if(array->size > capacity)
        array->size = capacity;

error:
    return ceError;
}

DWORD CTArrayInsert(DynamicArray *array, int insertPos, int itemSize, const void *data, size_t dataLen)
{
    DWORD ceError = ERROR_SUCCESS;

    if(array->size + dataLen > array->capacity)
    {
        /* Resize the array */
        ceError = CTSetCapacity(array, itemSize, array->capacity + dataLen + array->capacity);
        BAIL_ON_CENTERIS_ERROR(ceError);
    }
    /* Make room for the new value */
    memmove((char *)array->data + (insertPos + dataLen)*itemSize,
            (char *)array->data + insertPos*itemSize,
            (array->size - insertPos)*itemSize);
    memcpy((char *)array->data + insertPos*itemSize, data, dataLen*itemSize);
    array->size += dataLen;
error:
    return ceError;
}

DWORD CTArrayAppend(DynamicArray *array, int itemSize, const void *data, size_t dataLen)
{
    return CTArrayInsert(array, array->size, itemSize, data, dataLen);
}

DWORD CTArrayRemove(DynamicArray *array, int removePos, int itemSize, size_t dataLen)
{
    if(dataLen + removePos > array->size)
        dataLen = array->size - removePos;
    if(dataLen == 0)
        return ERROR_INVALID_PARAMETER;
    memmove((char *)array->data + removePos*itemSize,
            (char *)array->data + (removePos + dataLen)*itemSize,
            (array->size - removePos - dataLen)*itemSize);
    array->size -= dataLen;
    return ERROR_SUCCESS;
}

size_t CTArrayRemoveHead(DynamicArray *array, int itemSize, void *store, size_t dataLen)
{
    if(dataLen > array->size)
        dataLen = array->size;
    if(store != NULL)
    {
        memcpy(store, array->data, dataLen * itemSize);
    }
    CTArrayRemove(array, 0, itemSize, dataLen);
    return dataLen;
}

void CTArrayFree(DynamicArray *array)
{
    CT_SAFE_FREE_MEMORY(array->data);
    array->size = 0;
    array->capacity = 0;
}

void * CTArrayGetItem(DynamicArray *array, size_t index, size_t itemSize)
{
    if(index >= array->size)
        return NULL;
    return (char *)array->data + index*itemSize;
}

ssize_t CTArrayFindString(DynamicArray* array, PCSTR find)
{
    size_t i;
    for(i = 0; i < array->size; i++)
    {
        if(!strcmp(*(PCSTR *)CTArrayGetItem(array, i, sizeof(PCSTR)), find))
            return i;
    }
    return -1;
}
