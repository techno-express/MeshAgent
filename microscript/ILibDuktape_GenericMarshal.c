/*
Copyright 2006 - 2018 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#if defined(WIN32) && !defined(_WIN32_WCE) && !defined(_MINCORE)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "duktape.h"
#include "ILibDuktape_GenericMarshal.h"
#include "ILibDuktapeModSearch.h"
#include "ILibDuktape_Helpers.h"
#include "ILibDuktape_EventEmitter.h"
#include "microstack/ILibParsers.h"
#include "microstack/ILibCrypto.h"
#include "microstack/ILibRemoteLogging.h"

#if defined(_WIN64) || defined(__LP64__)
	typedef uint_fast64_t PTRSIZE;
#else
	typedef uint_fast32_t PTRSIZE;
#endif

#ifdef WIN32
#define APICALLTYPE __stdcall

#else
#define APICALLTYPE
#include <dlfcn.h>
#endif

#define ILibDuktape_GenericMarshal_VariableType			"\xFF_GenericMarshal_VarType"
#define ILibDuktape_GenericMarshal_GlobalSet			"\xFF_GenericMarshal_GlobalSet"
#define ILibDuktape_GenericMarshal_Variable_AutoFree	"\xFF_GenericMarshal_Variable_AutoFree"
#define ILibDuktape_GenericMarshal_Variable_Parms		"\xFF_GenericMarshal_Variable_Parms"
#define ILibDuktape_GenericMarshal_StashTable			"\xFF_GenericMarshal_StashTable"
#define ILibDuktape_GenericMarshal_Variable_EnableAutoFree(ctx, idx) duk_dup(ctx, idx);duk_push_true(ctx);duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_Variable_AutoFree);duk_pop(ctx)
#define ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, idx) duk_dup(ctx, idx);duk_push_false(ctx);duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_Variable_AutoFree);duk_pop(ctx)

typedef PTRSIZE(APICALLTYPE *R0)();
typedef PTRSIZE(APICALLTYPE *R1)(PTRSIZE V1);
typedef PTRSIZE(APICALLTYPE *R2)(PTRSIZE V1, PTRSIZE V2);
typedef PTRSIZE(APICALLTYPE *R3)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3);
typedef PTRSIZE(APICALLTYPE *R4)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4);
typedef PTRSIZE(APICALLTYPE *R5)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5);
typedef PTRSIZE(APICALLTYPE *R6)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6);
typedef PTRSIZE(APICALLTYPE *R7)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7);
typedef PTRSIZE(APICALLTYPE *R8)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8);
typedef PTRSIZE(APICALLTYPE *R9)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9);
typedef PTRSIZE(APICALLTYPE *R10)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10);
typedef PTRSIZE(APICALLTYPE *R11)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11);
typedef PTRSIZE(APICALLTYPE *R12)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12);
typedef PTRSIZE(APICALLTYPE *R13)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13);
typedef PTRSIZE(APICALLTYPE *R14)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14);
typedef PTRSIZE(APICALLTYPE *R15)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15);
typedef PTRSIZE(APICALLTYPE *R16)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15, PTRSIZE V16);
typedef PTRSIZE(APICALLTYPE *R17)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15, PTRSIZE V16, PTRSIZE V17);
typedef PTRSIZE(APICALLTYPE *R18)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15, PTRSIZE V16, PTRSIZE V17, PTRSIZE V18);
typedef PTRSIZE(APICALLTYPE *R19)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15, PTRSIZE V16, PTRSIZE V17, PTRSIZE V18, PTRSIZE V19);
typedef PTRSIZE(APICALLTYPE *R20)(PTRSIZE V1, PTRSIZE V2, PTRSIZE V3, PTRSIZE V4, PTRSIZE V5, PTRSIZE V6, PTRSIZE V7, PTRSIZE V8, PTRSIZE V9, PTRSIZE V10, PTRSIZE V11, PTRSIZE V12, PTRSIZE V13, PTRSIZE V14, PTRSIZE V15, PTRSIZE V16, PTRSIZE V17, PTRSIZE V18, PTRSIZE V19, PTRSIZE V20);

ILibLinkedList GlobalCallbackList = NULL;

typedef struct Duktape_GenericMarshal_Proxy
{
	duk_context *ctx;
	void *jsCallbackPtr;
	void *jsProxyObject;
}Duktape_GenericMarshal_Proxy;

typedef struct Duktape_GlobalGeneric_Data
{
	ILibDuktape_EventEmitter *emitter;
	void *retVal;
	void *chain;
	sem_t contextWaiter;
	int numArgs;
	PTRSIZE args[];
}Duktape_GlobalGeneric_Data;
typedef struct Duktape_MarshalledObject
{
	duk_context *ctx;
	void *heapptr;
}Duktape_MarshalledObject;

void ILibDuktape_GenericMarshal_Variable_PUSH(duk_context *ctx, void *ptr, int size);

duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_STRING(duk_context *ctx)
{
	void *ptr;
	//int size;
	
	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	//duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	//size = duk_to_int(ctx, -1);

	duk_push_string(ctx, (char*)ptr);

	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_HSTRING(duk_context *ctx)
{
	void *ptr;
	int size;
	char hexString[600];

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	size = duk_to_int(ctx, -1);

	util_tohex((char*)ptr, size < 255 ? size : 254, hexString);

	duk_push_string(ctx, (char*)hexString);
	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_HSTRING2(duk_context *ctx)
{
	void *ptr;
	int size;
	char hexString[255];

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	size = duk_to_int(ctx, -1);

	util_tohex2((char*)ptr, size < 255 ? size : 254, hexString);

	duk_push_string(ctx, (char*)hexString);
	return 1;
}

duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_ASTRING(duk_context *ctx)
{
	void *ptr;
	int size;

#ifdef WIN32
	char astr[65535];
	size_t s;
#endif

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	size = duk_to_int(ctx, -1);

#ifdef WIN32
	if (size == 0) { size = (int)wcsnlen_s((const wchar_t*)ptr, sizeof(astr) * 2); }
	wcstombs_s(&s, astr, sizeof(astr), (const wchar_t*)ptr, size);
	duk_push_string(ctx, (char*)astr);
#else
	duk_push_lstring(ctx, (const char*)ptr, size);
#endif
	return 1;
}

duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_GET(duk_context *ctx)
{
	void *ptr;

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	
	duk_push_int(ctx, (int)(PTRSIZE)ptr);

	return 1;
}

duk_ret_t ILibDuktape_GenericMarshal_Variable_Val_SET(duk_context *ctx)
{
	void *ptr;
	int size;

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = duk_to_pointer(ctx, -1);
	duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	size = duk_to_int(ctx, -1);

	if (duk_is_number(ctx, 0))
	{
		switch (size)
		{
		case 2:
			((unsigned short*)ptr)[0] = (unsigned short)duk_require_int(ctx, 0);
			break;
		case 4:
			((unsigned int*)ptr)[0] = (unsigned int)duk_require_int(ctx, 0);
			break;
		default:
			duk_push_string(ctx, "UNSUPPORTED VAL SIZE, with integral type");
			duk_throw(ctx);
			return(DUK_RET_ERROR);
		}
	}
	else if (duk_is_object(ctx, 0) && duk_has_prop_string(ctx, 0, ILibDuktape_GenericMarshal_VariableType))
	{
		((void**)ptr)[0] = Duktape_GetPointerProperty(ctx, 0, "_ptr");
		duk_push_this(ctx);		// [var]
		duk_dup(ctx, 0);		// [var][var]
		duk_put_prop_string(ctx, -2, "\xFF_ref");
	}
	else
	{
		return(ILibDuktape_Error(ctx, "Invalid Parameter"));
	}
	return 0;
}

duk_ret_t ILibDuktape_GenericMarshal_Variable_Deref(duk_context *ctx)
{
	int nargs = duk_get_top(ctx);
	char *ptr;
	//int size;

	duk_push_this(ctx);							// [var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [var][ptr]
	ptr = (char*)duk_to_pointer(ctx, -1);
	//duk_get_prop_string(ctx, -2, "_size");		// [var][ptr][size]
	//size = duk_to_int(ctx, -1);

	if (nargs < 2)
	{
		// Just Dereference Memory
		ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ((void**)ptr)[0], nargs == 1 ? duk_require_int(ctx, 0) : 0);
	}
	else
	{
		ILibDuktape_GenericMarshal_Variable_PUSH(ctx, (void*)(ptr + duk_require_int(ctx, 0)), duk_require_int(ctx, 1));
	}

	// We need to add a reference to the root variable, for memory management purposes
	duk_push_this(ctx);							// [deref][parent]
	if (duk_has_prop_string(ctx, -1, "_root"))
	{
		// Parent Var is not the root, but has a reference to it
		duk_get_prop_string(ctx, -1, "_root");	// [deref][parent][root]
		duk_put_prop_string(ctx, -3, "_root");	// [deref][parent]
		duk_pop(ctx);							// [deref]
	}
	else
	{
		// Parent Var is the root
		duk_put_prop_string(ctx, -2, "_root");	// [deref]
	}

	return 1;
}

duk_ret_t ILibDuktape_GenericMarshal_Variable_GetEx(duk_context *ctx)
{
	int varSize = 0;
	void *ptr;

	duk_push_current_function(ctx);				// [func]
	duk_get_prop_string(ctx, -1, "_VarSize");	// [func][varSize]
	varSize = duk_to_int(ctx, -1);

	duk_push_this(ctx);							// [func][varSize][var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [func][varSize][var][ptr]
	ptr = duk_to_pointer(ctx, -1);

	switch (varSize)
	{
	case 2:
		duk_push_int(ctx, (int)((unsigned short*)ptr)[0]);
		break;
	case 4:
		duk_push_int(ctx, (int)((unsigned int*)ptr)[0]);
		break;
	default:
		duk_push_string(ctx, "Invalid Variable"); duk_throw(ctx); return(DUK_RET_ERROR);
	}
	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_SetEx(duk_context *ctx)
{
	int varSize = 0;
	void *ptr;
	int newVal = duk_require_int(ctx, 0);

	duk_push_current_function(ctx);				// [func]
	duk_get_prop_string(ctx, -1, "_VarSize");	// [func][varSize]
	varSize = duk_to_int(ctx, -1);

	duk_push_this(ctx);							// [func][varSize][var]
	duk_get_prop_string(ctx, -1, "_ptr");		// [func][varSize][var][ptr]
	ptr = duk_to_pointer(ctx, -1);

	switch (varSize)
	{
	case 2:
		((unsigned short*)ptr)[0] = (unsigned short)newVal;
		break;
	case 4:
		((unsigned int*)ptr)[0] = (unsigned int)newVal;
		break;
	default:
		duk_push_string(ctx, "Invalid Variable"); duk_throw(ctx); return(DUK_RET_ERROR);
	}
	return 0;
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_toBuffer(duk_context *ctx)
{
	duk_push_this(ctx);						// [variable]

	void *buffer = Duktape_GetPointerProperty(ctx, -1, "_ptr");
	int bufferLen = Duktape_GetIntPropertyValue(ctx, -1, "_size", 0);

	duk_push_external_buffer(ctx);								// [variable][ext]
	duk_config_buffer(ctx, -1, buffer, (duk_size_t)bufferLen);
	duk_push_buffer_object(ctx, -1, 0, (duk_size_t)bufferLen, DUK_BUFOBJ_NODEJS_BUFFER);
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_Finalizer(duk_context *ctx)
{
	void *ptr = NULL;
	if (duk_has_prop_string(ctx, 0, "_ptr") && Duktape_GetBooleanProperty(ctx, 0, ILibDuktape_GenericMarshal_Variable_AutoFree, 0))
	{
		duk_get_prop_string(ctx, 0, "_ptr");
		ptr = duk_to_pointer(ctx, -1);
		if (ptr != NULL)
		{
			free(ptr);
			duk_del_prop_string(ctx, 0, "_ptr");
		}
	}

	return 0;
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_autoFree(duk_context *ctx)
{
	int nargs = duk_get_top(ctx);
	duk_push_this(ctx);																// [variable]
	if (nargs > 0 && !duk_require_boolean(ctx, 0))
	{
		ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, -1);
	}
	else
	{
		ILibDuktape_GenericMarshal_Variable_EnableAutoFree(ctx, -1);
	}
	return(0);
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_pointerBuffer(duk_context *ctx)
{
	duk_push_this(ctx);							// [var]
	void *ptr = Duktape_GetPointerProperty(ctx, -1, "_ptr");
	duk_push_fixed_buffer(ctx, sizeof(void*));
	duk_push_buffer_object(ctx, -1, 0, sizeof(void*), DUK_BUFOBJ_NODEJS_BUFFER);
	if (ptr != NULL)
	{
		memcpy_s(Duktape_GetBuffer(ctx, -2, NULL), sizeof(void*), (void*)&ptr, sizeof(void*));
	}
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_Variable_pointerpointer(duk_context *ctx)
{
	duk_push_this(ctx);																// [var]
	void *ptr = Duktape_GetPointerProperty(ctx, -1, "_ptr");
	duk_push_fixed_buffer(ctx, sizeof(void*));										// [var][ptrptr]
	void **ptrptr = (void**)Duktape_GetBuffer(ctx, -1, NULL);
	ptrptr[0] = ptr;

	ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptrptr, (int)sizeof(void*));		// [var][ptrptr][var2]
	ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, -1);
	duk_swap_top(ctx, -2);															// [var][var2][ptrptr]
	duk_put_prop_string(ctx, -2, "_ptrptr");										// [var][var2]
	return(1);
}
void ILibDuktape_GenericMarshal_Variable_PUSH(duk_context *ctx, void *ptr, int size)
{
	duk_push_object(ctx);						// [var]
	ILibDuktape_WriteID(ctx, "_GenericMarshal.Variable");
	duk_push_pointer(ctx, ptr);					// [var][ptr]
	duk_put_prop_string(ctx, -2, "_ptr");		// [var]
	duk_push_int(ctx, size);					// [var][size]
	duk_put_prop_string(ctx, -2, "_size");		// [var]
	duk_push_true(ctx);
	duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_VariableType);

	ILibDuktape_CreateEventWithGetterAndSetterEx(ctx, "Val", ILibDuktape_GenericMarshal_Variable_Val_GET, ILibDuktape_GenericMarshal_Variable_Val_SET);

	ILibDuktape_CreateInstanceMethod(ctx, "Deref", ILibDuktape_GenericMarshal_Variable_Deref, DUK_VARARGS);
	ILibDuktape_CreateEventWithGetter(ctx, "String", ILibDuktape_GenericMarshal_Variable_Val_STRING);
	ILibDuktape_CreateEventWithGetter(ctx, "AnsiString", ILibDuktape_GenericMarshal_Variable_Val_ASTRING);
	ILibDuktape_CreateEventWithGetter(ctx, "HexString", ILibDuktape_GenericMarshal_Variable_Val_HSTRING);
	ILibDuktape_CreateEventWithGetter(ctx, "HexString2", ILibDuktape_GenericMarshal_Variable_Val_HSTRING2);

	ILibDuktape_CreateInstanceMethod(ctx, "toBuffer", ILibDuktape_GenericMarshal_Variable_toBuffer, 0);
	ILibDuktape_CreateInstanceMethod(ctx, "autoFree", ILibDuktape_GenericMarshal_Variable_autoFree, DUK_VARARGS);
	ILibDuktape_CreateInstanceMethod(ctx, "pointerBuffer", ILibDuktape_GenericMarshal_Variable_pointerBuffer, 0);
	ILibDuktape_CreateInstanceMethod(ctx, "getPointerPointer", ILibDuktape_GenericMarshal_Variable_pointerpointer, 0);

	ILibDuktape_CreateFinalizer(ctx, ILibDuktape_GenericMarshal_Variable_Finalizer);
}

duk_ret_t ILibDuktape_GenericMarshal_CreateVariable(duk_context *ctx)
{
	char* ptr;
	int size;
	char *str = NULL;
	duk_size_t strLen;

	if (duk_is_number(ctx, 0))
	{
		size = duk_require_int(ctx, 0);
	}
	else if(duk_is_string(ctx, 0))
	{
		str = Duktape_GetBuffer(ctx, 0, &strLen);
		size = (int)strLen + 1;

		if (duk_is_object(ctx, 1))
		{
			if (Duktape_GetBooleanProperty(ctx, 1, "wide", 0) != 0)
			{
#ifdef WIN32
				wchar_t *wbuffer = (wchar_t*)ILibMemory_AllocateA(((int)strLen * 2) + 2);
				size_t converted;
				mbstowcs_s(&converted, wbuffer, (size_t)strLen+1, str, (size_t)strLen);
				str = (char*)wbuffer;
				size = (int)ILibMemory_AllocateA_Size(str);
				strLen = size - 1;
#else
				return(ILibDuktape_Error(ctx, "Not supported on this platform"));
#endif
			}
		}
	}
	else
	{
		return(ILibDuktape_Error(ctx, "_GenericMarshal.CreateVariable(): Invalid Parameter"));
	}

	ptr = (char*)ILibMemory_Allocate(size, 0, NULL, NULL);
	if (str != NULL)
	{
		memcpy_s(ptr, size, str, strLen);
		ptr[strLen] = 0;
	}
	ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptr, size);							// [var]
	ILibDuktape_GenericMarshal_Variable_EnableAutoFree(ctx, -1);

	return 1;
}

void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_0()
{

}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_1(PTRSIZE v1)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v1;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	if (duk_pcall_method(user->ctx, 0) != 0)											// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);																	// ...
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_2(PTRSIZE v1, PTRSIZE v2)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v2;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	if(duk_pcall_method(user->ctx, 1)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);																	// ...
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_3(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v3;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	if(duk_pcall_method(user->ctx, 2)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);																	// ...
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_4(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v4;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	if(duk_pcall_method(user->ctx, 3)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);																	// ...
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_5(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v5;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v4, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4]
	if(duk_pcall_method(user->ctx, 4)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);																	// ...
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_6(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v6;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v4, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v5, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5]
	if(duk_pcall_method(user->ctx, 5)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_7(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6, PTRSIZE v7)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v7;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v4, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v5, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v6, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6]
	if(duk_pcall_method(user->ctx, 6)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_8(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6, PTRSIZE v7, PTRSIZE v8)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v8;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v4, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v5, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v6, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v7, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6][v7]
	if(duk_pcall_method(user->ctx, 7)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);
}
void ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_9(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6, PTRSIZE v7, PTRSIZE v8, PTRSIZE v9)
{
	Duktape_GenericMarshal_Proxy  *user = (Duktape_GenericMarshal_Proxy*)v9;
	duk_push_heapptr(user->ctx, user->jsCallbackPtr);									// [func]
	duk_push_heapptr(user->ctx, user->jsProxyObject);									// [func][this]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v1, sizeof(PTRSIZE));	// [func][this][v1]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v2, sizeof(PTRSIZE));	// [func][this][v1][v2]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v3, sizeof(PTRSIZE));	// [func][this][v1][v2][v3]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v4, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v5, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v6, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v7, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6][v7]
	ILibDuktape_GenericMarshal_Variable_PUSH(user->ctx, (void*)v8, sizeof(PTRSIZE));	// [func][this][v1][v2][v3][v4][v5][v6][v7][v8]
	if(duk_pcall_method(user->ctx, 8)!=0)												// [retVal]
	{
		ILibDuktape_Process_UncaughtException(user->ctx);
	}
	duk_pop(user->ctx);
}

duk_ret_t ILibDuktape_GenericMashal_CallbackProxy_Callback(duk_context *ctx)
{
	int parms;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "_numParms");
	parms = duk_to_int(ctx, -1);

	switch (parms)
	{
		case 0:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_0);
			break;
		case 1:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_1);
			break;
		case 2:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_2);
			break;
		case 3:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_3);
			break;
		case 4:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_4);
			break;
		case 5:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_5);
			break;
		case 6:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_6);
			break;
		case 7:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_7);
			break;
		case 8:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_8);
			break;
		case 9:
			duk_push_pointer(ctx, (void*)ILibDuktape_GenericMarshal_CallbackProxy_NativeSink_9);
			break;
		default:
			duk_push_string(ctx, "More than 9 parameters in the callback isn't supported yet");
			duk_throw(ctx);
			return(DUK_RET_ERROR);
			break;
	}
	return 1;
}
duk_ret_t ILibDuktape_GenericMashal_CallbackProxy_State(duk_context *ctx)
{
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "_statePtr");
	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_CreateCallbackProxy(duk_context *ctx)
{
	Duktape_GenericMarshal_Proxy *ptr;

	duk_push_object(ctx);													// [proxy]
	ptr = Duktape_PushBuffer(ctx, sizeof(Duktape_GenericMarshal_Proxy));	// [proxy][buffer]
	duk_put_prop_string(ctx, -2, "_buffer");								// [proxy]

	duk_dup(ctx, 1);														// [proxy][parms]
	duk_put_prop_string(ctx, -2, "_numParms");								// [proxy]

	duk_push_pointer(ctx, ptr);												// [proxy][statePtr]
	duk_put_prop_string(ctx, -2, "_statePtr");								// [proxy]

	ptr->ctx = ctx;
	ptr->jsCallbackPtr = (void*)duk_require_heapptr(ctx, 0);
	ptr->jsProxyObject = (void*)duk_get_heapptr(ctx, -1);

	duk_dup(ctx, 0);														// [proxy][jsCallback]
	duk_put_prop_string(ctx, -2, "_jsCallback");							// [proxy]
	
	ILibDuktape_CreateEventWithGetter(ctx, "Callback", ILibDuktape_GenericMashal_CallbackProxy_Callback);
	ILibDuktape_CreateEventWithGetter(ctx, "State", ILibDuktape_GenericMashal_CallbackProxy_State);


	return 1;
}

PTRSIZE ILibDuktape_GenericMarshal_MethodInvoke_Native(int parms, void *fptr, PTRSIZE *vars)
{
	PTRSIZE retVal = 0;

	switch (parms)
	{
	case 0:
		retVal = ((R0)fptr)();
		break;
	case 1:
		retVal = ((R1)fptr)(vars[0]);
		break;
	case 2:
		retVal = ((R2)fptr)(vars[0], vars[1]);
		break;
	case 3:
		retVal = ((R3)fptr)(vars[0], vars[1], vars[2]);
		break;
	case 4:
		retVal = ((R4)fptr)(vars[0], vars[1], vars[2], vars[3]);
		break;
	case 5:
		retVal = ((R5)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4]);
		break;
	case 6:
		retVal = ((R6)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5]);
		break;
	case 7:
		retVal = ((R7)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6]);
		break;
	case 8:
		retVal = ((R8)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7]);
		break;
	case 9:
		retVal = ((R9)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8]);
		break;
	case 10:
		retVal = ((R10)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9]);
		break;
	case 11:
		retVal = ((R11)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10]);
		break;
	case 12:
		retVal = ((R12)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11]);
		break;
	case 13:
		retVal = ((R13)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12]);
		break;
	case 14:
		retVal = ((R14)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13]);
		break;
	case 15:
		retVal = ((R15)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14]);
		break;
	case 16:
		retVal = ((R16)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14], vars[15]);
		break;
	case 17:
		retVal = ((R17)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14], vars[15], vars[16]);
		break;
	case 18:
		retVal = ((R18)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14], vars[15], vars[16], vars[17]);
		break;
	case 19:
		retVal = ((R19)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14], vars[15], vars[16], vars[17], vars[18]);
		break;
	case 20:
		retVal = ((R20)fptr)(vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7], vars[8], vars[9], vars[10], vars[11], vars[12], vars[13], vars[14], vars[15], vars[16], vars[17], vars[18], vars[19]);
		break;
	}

	return(retVal);
}
void ILibDuktape_GenericMarshal_MethodInvoke_ThreadSink_Return(void *chain, void *args)
{
	if (!ILibMemory_CanaryOK(args)) { return; }

	PTRSIZE *vars = (PTRSIZE*)((void**)args)[2];
	ILibDuktape_EventEmitter *e = (ILibDuktape_EventEmitter*)((void**)args)[0];
	PTRSIZE retVal = (PTRSIZE)((void**)args)[3];

	if (!ILibMemory_CanaryOK(e) || e->ctx == NULL) { return; }

	duk_context *ctx = e->ctx;
	ILibDuktape_EventEmitter_SetupEmit(e->ctx, e->object, "done");									// [emit][this][done]
	ILibDuktape_GenericMarshal_Variable_PUSH(e->ctx, (void*)(PTRSIZE)retVal, (int)sizeof(void*));	// [emit][this][done][retVal]
	if (duk_pcall_method(e->ctx, 2) != 0) 
	{ 
		ILibDuktape_Process_UncaughtException(e->ctx); duk_pop(e->ctx);
	}
	else
	{
		duk_pop(ctx);
	}
	ILibMemory_Free(vars);
}
void ILibDuktape_GenericMarshal_MethodInvoke_ThreadSink(void *args)
{
	void *chain = ((void**)args)[1];
	PTRSIZE *vars = (PTRSIZE*)((void**)args)[2];
	int parms = (int)(PTRSIZE)((void**)args)[3];
	void *fptr = ((void**)args)[4];
	PTRSIZE retVal = ILibDuktape_GenericMarshal_MethodInvoke_Native(parms, fptr, vars);

	((void**)args)[3] = (void*)retVal;
	ILibChain_RunOnMicrostackThreadEx(chain, ILibDuktape_GenericMarshal_MethodInvoke_ThreadSink_Return, args);

}
duk_ret_t ILibDuktape_GenericMarshal_MethodInvoke(duk_context *ctx)
{
	void *fptr = NULL;
	int parms = duk_get_top(ctx);
	int i;
	PTRSIZE retVal = (PTRSIZE)(-1);
	if (parms > 20) { return(ILibDuktape_Error(ctx, "Too many parameters")); }
	
	duk_push_current_function(ctx);					// [func]
	int spawnThread = Duktape_GetBooleanProperty(ctx, -1, "_spawnThread", 0);
	PTRSIZE *vars = spawnThread == 0 ? ILibMemory_AllocateA(sizeof(PTRSIZE)*parms) : ILibMemory_SmartAllocateEx(sizeof(PTRSIZE)*parms, 5 * sizeof(void*));
	duk_get_prop_string(ctx, -1, "_address");		// [func][addr]
	fptr = duk_to_pointer(ctx, -1);

	if (fptr == NULL)
	{
		duk_push_string(ctx, "INVALID METHOD");
		duk_throw(ctx);
		return(DUK_RET_ERROR);
	}

	for (i = 0; i < parms; ++i)
	{
		if (duk_is_object(ctx, i))
		{
			duk_get_prop_string(ctx, i, "_ptr");
			vars[i] = (PTRSIZE)duk_to_pointer(ctx, -1);
		}
		else if (duk_is_number(ctx, i))
		{
			vars[i] = (PTRSIZE)duk_require_int(ctx, i);
		}
		else if (duk_is_pointer(ctx, i))
		{
			vars[i] = (PTRSIZE)duk_require_pointer(ctx, i);
		}
		else
		{
			duk_push_string(ctx, "INVALID PARAMETER");
			duk_throw(ctx);
			return(DUK_RET_ERROR);
		}
	}

	if (parms > 20)
	{
		duk_push_string(ctx, "INVALID NUMBER OF PARAMETERS, MAX of 20");
		duk_throw(ctx);
		return(DUK_RET_ERROR);
	}
	else
	{
		if (spawnThread == 0)
		{
			retVal = ILibDuktape_GenericMarshal_MethodInvoke_Native(parms, fptr, vars);
			ILibDuktape_GenericMarshal_Variable_PUSH(ctx, (void*)(PTRSIZE)retVal, (int)sizeof(void*));
		}
		else
		{
			duk_push_object(ctx);														// [ret]
			ILibDuktape_WriteID(ctx, "GenericMarshal.Variable.DispatcherEvent");
			ILibDuktape_EventEmitter *e = ILibDuktape_EventEmitter_Create(ctx);
			ILibDuktape_EventEmitter_CreateEventEx(e, "done");

			void **args = (void**)ILibMemory_Extra(vars);
			args[0] = e;
			args[1] = Duktape_GetChain(ctx);
			args[2] = vars;
			args[3] = (void*)(PTRSIZE)parms;
			args[4] = fptr;

			void *thptr = ILibSpawnNormalThread(ILibDuktape_GenericMarshal_MethodInvoke_ThreadSink, args);
			duk_push_fixed_buffer(ctx, sizeof(void*));									// [ret][buffer]
			((void**)Duktape_GetBuffer(ctx, -1, NULL))[0] = thptr;
			duk_push_buffer_object(ctx, -1, 0, sizeof(void*), DUK_BUFOBJ_NODEJS_BUFFER);// [ret][buffer][NodeBuffer]

			duk_swap_top(ctx, -2);														// [ret][NodeBuffer][buffer]
			duk_put_prop_string(ctx, -3, "\xFF_BuffPtr");								// [ret][NodeBuffer]	
			ILibDuktape_CreateReadonlyProperty(ctx, "_ThreadHandle");					// [ret]
		}
	}

	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_CreateMethod(duk_context *ctx)
{
	void* module = NULL;
	void* funcAddress = NULL;
	char* funcName;
	char* exposedMethod;
	int threadDispatch = 0;
	int deref = 0;

	if (duk_is_object(ctx, 0))
	{
		funcName = Duktape_GetStringPropertyValue(ctx, 0, "method", NULL);
		exposedMethod = Duktape_GetStringPropertyValue(ctx, 0, "newName", funcName);
		threadDispatch = Duktape_GetIntPropertyValue(ctx, 0, "threadDispatch", 0);
		deref = Duktape_GetIntPropertyValue(ctx, 0, "dereferencePointer", 0);
	}
	else
	{
		funcName = (char*)duk_require_string(ctx, 0);
		exposedMethod = duk_get_top(ctx) == 1 ? funcName : (char*)duk_require_string(ctx, 1);
	}

	duk_push_this(ctx);																	// [obj]
	duk_get_prop_string(ctx, -1, "_moduleAddress");										// [obj][module]
	module = duk_to_pointer(ctx, -1);
	duk_pop(ctx);																		// [obj]

#ifdef WIN32
	funcAddress = (void*)GetProcAddress((HMODULE)module, (LPCSTR)funcName);
#else
	funcAddress = (void*)dlsym(module, funcName);
#endif

	if (funcAddress == NULL)
	{
		char errstr[4096];
		sprintf_s(errstr, sizeof(errstr), "CreateMethod Error: Method Name [%s] Not Found", funcName);
		duk_push_string(ctx, errstr);
		duk_throw(ctx);
		return(DUK_RET_ERROR);
	}
	else if(deref!=0)
	{
		funcAddress = ((void**)funcAddress)[0];
	}

	duk_push_c_function(ctx, ILibDuktape_GenericMarshal_MethodInvoke, DUK_VARARGS);					// [obj][func]
	duk_push_pointer(ctx, funcAddress);																// [obj][func][addr]
	duk_put_prop_string(ctx, -2, "_address");														// [obj][func]
	if (threadDispatch != 0) { duk_push_true(ctx); duk_put_prop_string(ctx, -2, "_spawnThread"); }	// [obj][func]
	duk_put_prop_string(ctx, -2, exposedMethod);													// [obj]

	return 0;
}
duk_ret_t ILibDuktape_GenericMarshal_NativeProxy_Finalizer(duk_context *ctx)
{
#ifdef WIN32
	HMODULE hm = (HMODULE)Duktape_GetPointerProperty(ctx, 0, "_moduleAddress");
	if (hm != NULL)
	{
		FreeLibrary(hm);
	}
#else
	void *hm = Duktape_GetPointerProperty(ctx, 0, "_moduleAddress");
	if (hm != NULL)
	{
		dlclose(hm);
	}
#endif
	return(0);
}
duk_ret_t ILibDuktape_GenericMarshal_CreateNativeProxy(duk_context *ctx)
{
	void* module = NULL;
	char* libName = duk_is_string(ctx, 0) ? (char*)duk_require_string(ctx, 0) : NULL;

#ifdef WIN32
	if (libName != NULL)
	{
		module = (void*)LoadLibraryA((LPCSTR)libName);
	}
	else
	{
		HMODULE hModule = NULL;
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)ILibDuktape_GenericMarshal_CreateNativeProxy, &hModule);
		module = (void*)hModule;
	}
#else
	module = dlopen(libName, RTLD_NOW);
#endif

	if (module == NULL)
	{
#ifdef WIN32
		duk_push_string(ctx, "Could not create Native Proxy");
#else
		duk_push_string(ctx, dlerror());
#endif
		duk_throw(ctx);
		return(DUK_RET_ERROR);
	}

	duk_push_object(ctx);																							// [obj]
	duk_push_pointer(ctx, module);																					// [obj][module]
	duk_put_prop_string(ctx, -2, "_moduleAddress");																	// [obj]
	ILibDuktape_CreateInstanceMethod(ctx, "CreateMethod", ILibDuktape_GenericMarshal_CreateMethod, DUK_VARARGS);	// [obj]
	if (libName != NULL) { ILibDuktape_CreateFinalizer(ctx, ILibDuktape_GenericMarshal_NativeProxy_Finalizer); }

	return 1;
}
duk_ret_t ILibDuktape_GenericMarshal_CreateVariableEx(duk_context *ctx)
{
	int nargs = duk_get_top(ctx);
	char* ptr = NULL;
	int size;
	duk_push_current_function(ctx);														// [func]
	duk_get_prop_string(ctx, -1, "_VarSize");											// [func][size]
	size = duk_to_int(ctx, -1);

	if (nargs == 1)
	{
		duk_size_t buflen;
		ptr = Duktape_GetBuffer(ctx, 0, &buflen);
	}
	
	if (ptr == NULL)
	{
		ptr = (char*)ILibMemory_Allocate(size, 0, NULL, NULL);
		ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptr, size);							// [func][size][var]
		ILibDuktape_GenericMarshal_Variable_EnableAutoFree(ctx, -1);
	}
	else
	{
		ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ((void**)ptr)[0], size);				// [func][size][var]
		ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, -1);
	}
	return 1;
}

void ILibDuktape_GlobalGenericCallback_ProcessEx(void *chain, void *user)
{
	int i;
	//void *retVal = NULL;
	Duktape_GlobalGeneric_Data *data = (Duktape_GlobalGeneric_Data*)user;
	ILibDuktape_EventEmitter_SetupEmit(data->emitter->ctx, data->emitter->object, "GlobalCallback");	// [emit][this][GlobalCallback]
	for (i = 0; i < data->numArgs; ++i)
	{
		ILibDuktape_GenericMarshal_Variable_PUSH(data->emitter->ctx, (void*)data->args[i], sizeof(void*));
	}
	if (duk_pcall_method(data->emitter->ctx, data->numArgs + 1) != 0) 
	{ 
		ILibDuktape_Process_UncaughtException(data->emitter->ctx); 
	}

	duk_pop(data->emitter->ctx);

	if (data->emitter->lastReturnValue != NULL)
	{
		duk_push_heapptr(data->emitter->ctx, data->emitter->lastReturnValue);
		data->retVal = Duktape_GetPointerProperty(data->emitter->ctx, -1, "_ptr");
		duk_pop(data->emitter->ctx);
	}

	sem_post(&(data->contextWaiter));
}
void* ILibDuktape_GlobalGenericCallback_Process(int numParms, ...)
{
	void *retVal = NULL;
	PTRSIZE v;
	ILibLinkedList_Lock(GlobalCallbackList);
	Duktape_GlobalGeneric_Data *data;
	int i = 0, maxCount = ILibLinkedList_GetCount(GlobalCallbackList), count = 0;
	void *node = ILibLinkedList_GetNode_Head(GlobalCallbackList);
	Duktape_GlobalGeneric_Data **refList = (Duktape_GlobalGeneric_Data**)ILibMemory_AllocateA(maxCount * sizeof(Duktape_GlobalGeneric_Data*));
	while (node != NULL)
	{
		data = (Duktape_GlobalGeneric_Data*)ILibLinkedList_GetDataFromNode(node);
		if (!ILibIsRunningOnChainThread(data->chain))
		{
			// Need to Context Switch
			Duktape_GlobalGeneric_Data *user = ILibMemory_Allocate(sizeof(Duktape_GlobalGeneric_Data) + (numParms * sizeof(PTRSIZE)), 0, NULL, NULL);
			sem_init(&(user->contextWaiter), 0, 0);
			user->chain = data->chain;
			user->emitter = data->emitter;
			user->numArgs = numParms;
			if (numParms > 0)
			{
				va_list vlist;
				va_start(vlist, numParms);
				for (i = 0; i < numParms; ++i)
				{
					user->args[i] = va_arg(vlist, PTRSIZE);
				}
				va_end(vlist);
			}
			ILibChain_RunOnMicrostackThreadEx(user->chain, ILibDuktape_GlobalGenericCallback_ProcessEx, user);
			sem_wait(&(user->contextWaiter));

			if (user->retVal != NULL) { retVal = user->retVal; }
			sem_destroy(&(user->contextWaiter));
			free(user);
		}
		else
		{
			// No need to context switch, so just reference this
			refList[count++] = data;
		}
		node = ILibLinkedList_GetNextNode(node);
	}
	ILibLinkedList_UnLock(GlobalCallbackList);

	for (i = 0; i < count; ++i)
	{
		ILibDuktape_EventEmitter_SetupEmit(refList[i]->emitter->ctx, refList[i]->emitter->object, "GlobalCallback");	// [emit][this][GlobalCallback]
		if (numParms > 0)
		{
			int z;
			va_list vlist;
			va_start(vlist, numParms);
			for (z = 0; z < numParms; ++z)
			{
				v = va_arg(vlist, PTRSIZE);
				ILibDuktape_GenericMarshal_Variable_PUSH(refList[i]->emitter->ctx, (void*)v, sizeof(void*));
				ILibDuktape_GenericMarshal_Variable_DisableAutoFree(refList[i]->emitter->ctx, -1);
			}
			va_end(vlist);
		}
		if (duk_pcall_method(refList[i]->emitter->ctx, numParms + 1) != 0) 
		{ 
			ILibDuktape_Process_UncaughtException(refList[i]->emitter->ctx); 
		}
		else
		{
			if ((retVal = refList[i]->emitter->lastReturnValue) != NULL)
			{
				duk_push_heapptr(refList[i]->emitter->ctx, refList[i]->emitter->lastReturnValue);				// [retVal]
				if (duk_has_prop_string(refList[i]->emitter->ctx, -1, ILibDuktape_GenericMarshal_VariableType))
				{
					retVal = Duktape_GetPointerProperty(refList[i]->emitter->ctx, -1, "_ptr");
				}
				duk_pop(refList[i]->emitter->ctx);																// ...
			}
		}
		duk_pop(refList[i]->emitter->ctx);
	}
	return(retVal);
}

PTRSIZE ILibDuktape_GlobalGenericCallback0()
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(0);
}
PTRSIZE ILibDuktape_GlobalGenericCallback1(PTRSIZE v1)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(1, v1);
}										 
PTRSIZE ILibDuktape_GlobalGenericCallback2(PTRSIZE v1, PTRSIZE v2)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(2, v1, v2);
}										  
PTRSIZE ILibDuktape_GlobalGenericCallback3(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(3, v1, v2, v3);
}										 
PTRSIZE ILibDuktape_GlobalGenericCallback4(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(4, v1, v2, v3, v4);
}										  
PTRSIZE ILibDuktape_GlobalGenericCallback5(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(5, v1, v2, v3, v4, v5);
}										 
PTRSIZE ILibDuktape_GlobalGenericCallback6(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(6, v1, v2, v3, v4, v5, v6);
}										  
PTRSIZE ILibDuktape_GlobalGenericCallback7(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6, PTRSIZE v7)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(7, v1, v2, v3, v4, v5, v6, v7);
}										  
PTRSIZE ILibDuktape_GlobalGenericCallback8(PTRSIZE v1, PTRSIZE v2, PTRSIZE v3, PTRSIZE v4, PTRSIZE v5, PTRSIZE v6, PTRSIZE v7, PTRSIZE v8)
{
	return (PTRSIZE)ILibDuktape_GlobalGenericCallback_Process(8, v1, v2, v3, v4, v5, v6, v7, v8);
}
duk_ret_t ILibDuktape_GenericMarshal_GlobalGenericCallback_EventSink(duk_context *ctx)
{
	int nargs = duk_get_top(ctx), i;
	duk_push_current_function(ctx);			// [func]
	duk_get_prop_string(ctx, -1, "self");	// [func][variable]
	void *self = duk_get_heapptr(ctx, -1);
	if (Duktape_GetIntPropertyValue(ctx, -1, ILibDuktape_GenericMarshal_Variable_Parms, -1) == nargs)
	{
		ILibDuktape_EventEmitter_SetupEmit(ctx, duk_get_heapptr(ctx, -1), "GlobalCallback");	// [emit][this][GlobalCallback]
		for (i = 0; i < nargs; ++i) { duk_dup(ctx, i); }
		duk_pcall_method(ctx, nargs + 1);
		duk_push_heapptr(ctx, self);						// [this]
		duk_get_prop_string(ctx, -1, "emit_returnValue");	// [this][emit_returnValue]
		duk_swap_top(ctx, -2);								// [emit_returnValue][this]
		duk_call_method(ctx, 0);
		return(1);
	}
	else
	{
		return(0);
	}
}
duk_ret_t ILibDuktape_GenericMarshal_ObjectToPtr_Verify(duk_context *ctx)
{
	void *ptr = duk_require_heapptr(ctx, 0);
	void *var = Duktape_GetPointerProperty(ctx, 1, "_ptr");

	duk_push_boolean(ctx, ptr == var);
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_GetGlobalGenericCallback(duk_context *ctx)
{
	int numParms = duk_require_int(ctx, 0);
	
	duk_push_this(ctx);																		// [GenericMarshal]
	if (!duk_has_prop_string(ctx, -1, ILibDuktape_GenericMarshal_GlobalSet))
	{
		if (GlobalCallbackList == NULL)
		{
			GlobalCallbackList = ILibLinkedList_Create();
		}

		Duktape_GlobalGeneric_Data *data = ILibMemory_Allocate(sizeof(Duktape_GlobalGeneric_Data), 0, NULL, NULL);
		data->emitter = ILibDuktape_EventEmitter_Create(ctx);
		data->chain = Duktape_GetChain(ctx);
		ILibDuktape_EventEmitter_CreateEventEx(data->emitter, "GlobalCallback");

		ILibLinkedList_Lock(GlobalCallbackList);
		ILibLinkedList_AddTail(GlobalCallbackList, data);
		ILibLinkedList_UnLock(GlobalCallbackList);
		duk_push_true(ctx);
		duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_GlobalSet);
	}
	
	void *ptr = NULL;
	switch (numParms)
	{
		case 0:
			ptr = (void*)(R0)ILibDuktape_GlobalGenericCallback0;
			break;
		case 1:
			ptr = (void*)(R1)ILibDuktape_GlobalGenericCallback1;
			break;
		case 2:
			ptr = (void*)(R2)ILibDuktape_GlobalGenericCallback2;
			break;
		case 3:
			ptr = (void*)(R3)ILibDuktape_GlobalGenericCallback3;
			break;
		case 4:
			ptr = (void*)(R4)ILibDuktape_GlobalGenericCallback4;
			break;
		case 5:
			ptr = (void*)(R5)ILibDuktape_GlobalGenericCallback5;
			break;
		case 6:
			ptr = (void*)(R6)ILibDuktape_GlobalGenericCallback6;
			break;
		case 7:
			ptr = (void*)(R7)ILibDuktape_GlobalGenericCallback7;
			break;
		case 8:
			ptr = (void*)(R8)ILibDuktape_GlobalGenericCallback8;
			break;
		default:
			return(ILibDuktape_Error(ctx, "%d callback parameters not currently supported. Max 8", numParms));
	}
	ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptr, (int)sizeof(void*));								// [GenericMarshal][Variable]
	ILibDuktape_EventEmitter *varEmitter = ILibDuktape_EventEmitter_Create(ctx);
	ILibDuktape_EventEmitter_CreateEventEx(varEmitter, "GlobalCallback");
	duk_push_int(ctx, numParms); duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_Variable_Parms);

	duk_get_prop_string(ctx, -2, "on");																	// [GenericMarshal][Variable][on]
	duk_dup(ctx, -3);																					// [GenericMarshal][Variable][on][this/GM]
	duk_push_string(ctx, "GlobalCallback");																// [GenericMarshal][Variable][on][this/GM][GlobalCallback]
	duk_push_c_function(ctx, ILibDuktape_GenericMarshal_GlobalGenericCallback_EventSink, DUK_VARARGS);	// [GenericMarshal][Variable][on][this/GM][GlobalCallback][func]
	duk_dup(ctx, -5);																					// [GenericMarshal][Variable][on][this/GM][GlobalCallback][func][Variable]
	duk_put_prop_string(ctx, -2, "self");																// [GenericMarshal][Variable][on][this/GM][GlobalCallback][func]
	duk_call_method(ctx, 2); duk_pop(ctx);																// [GenericMarshal][Variable]

	ILibDuktape_CreateInstanceMethod(ctx, "ObjectToPtr_Verify", ILibDuktape_GenericMarshal_ObjectToPtr_Verify, 2);
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_Finalizer(duk_context *ctx)
{
	void *tmp = NULL;
	if (GlobalCallbackList != NULL)
	{
		ILibLinkedList_Lock(GlobalCallbackList);
		void *node = ILibLinkedList_GetNode_Head(GlobalCallbackList);
		while (node != NULL)
		{
			Duktape_GlobalGeneric_Data *data = (Duktape_GlobalGeneric_Data*)ILibLinkedList_GetDataFromNode(node);
			if (data->chain == Duktape_GetChain(ctx))
			{
				free(data);
				void *next = ILibLinkedList_GetNextNode(node);
				ILibLinkedList_Remove(node);
				node = next;
			}
			else
			{
				node = ILibLinkedList_GetNextNode(node);
			}
		}
		if (ILibLinkedList_GetCount(GlobalCallbackList) == 0) { tmp = GlobalCallbackList; }
		ILibLinkedList_UnLock(GlobalCallbackList);
		if (tmp != NULL)
		{
			GlobalCallbackList = NULL;
		}
	}
	if (tmp != NULL) { ILibLinkedList_Destroy(tmp); }
	return(0);
}
duk_ret_t ILibDuktape_GenericMarshal_WrapObject(duk_context *ctx)
{
	void *hptr = duk_require_heapptr(ctx, 0);

	duk_push_heap_stash(ctx);														// [stash]
	duk_dup(ctx, 0);																// [stash][obj]
	duk_put_prop_string(ctx, -2, Duktape_GetStashKey(duk_get_heapptr(ctx, -1)));	// [stash]
	duk_push_fixed_buffer(ctx, sizeof(Duktape_MarshalledObject));
	Duktape_MarshalledObject *marshalled = (Duktape_MarshalledObject*)Duktape_GetBuffer(ctx, -1, NULL);
	duk_push_buffer_object(ctx, -1, 0, sizeof(Duktape_MarshalledObject), DUK_BUFOBJ_NODEJS_BUFFER);
	marshalled->ctx = ctx;
	marshalled->heapptr = hptr;
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_UnWrapObject(duk_context *ctx)
{
	duk_size_t len;
	Duktape_MarshalledObject *marshalled = (Duktape_MarshalledObject*)Duktape_GetBuffer(ctx, 0, &len);
	if (len != sizeof(Duktape_MarshalledObject) || marshalled->ctx != ctx)
	{
		return(ILibDuktape_Error(ctx, "Invalid WrappedObject, cannot UnWrapObject()"));
	}
	duk_push_heapptr(ctx, marshalled->heapptr);
	duk_push_heap_stash(ctx);
	duk_del_prop_string(ctx, -1, Duktape_GetStashKey(marshalled->heapptr));
	duk_pop(ctx);
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_StashObject(duk_context *ctx)
{
	void *ptr = duk_require_heapptr(ctx, 0);

	duk_push_this(ctx);															// [Marshal]
	duk_get_prop_string(ctx, -1, ILibDuktape_GenericMarshal_StashTable);		// [Marshal][StashTable]
	duk_dup(ctx, 0); duk_put_prop_string(ctx, -2, Duktape_GetStashKey(ptr));	// [Marshal][StashTable]
	ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptr, sizeof(void*));
	ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, -1);
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_UnstashObject(duk_context *ctx)
{
	void *ptr = Duktape_GetPointerProperty(ctx, 0, "_ptr");
	if (ptr == NULL) 
	{ 
		duk_push_null(ctx); 
	}
	else
	{
		duk_push_this(ctx);															// [Marshal]
		duk_get_prop_string(ctx, -1, ILibDuktape_GenericMarshal_StashTable);		// [Marshal][StashTable]
		char *key = Duktape_GetStashKey(ptr);
		if (duk_has_prop_string(ctx, -1, key))
		{
			duk_get_prop_string(ctx, -1, key);										// [Marshal][StashTable][obj]
			if (duk_is_boolean(ctx, 1) && duk_get_boolean(ctx, 1))
			{
				duk_del_prop_string(ctx, -2, key);									// [Marshal][StashTable][obj]
			}
		}
		else
		{
			duk_push_null(ctx);
		}
	}
	return(1);
}
duk_ret_t ILibDuktape_GenericMarshal_ObjectToPtr(duk_context *ctx)
{
	void *ptr = duk_require_heapptr(ctx, 0);

	ILibDuktape_GenericMarshal_Variable_PUSH(ctx, ptr, sizeof(void*));		// [var]
	ILibDuktape_GenericMarshal_Variable_DisableAutoFree(ctx, -1);
	return(1);
}

void ILibDuktape_GenericMarshal_Push(duk_context *ctx, void *chain)
{
	duk_push_object(ctx);												// [obj]
	duk_push_object(ctx);												// [obj][stashTable]
	duk_put_prop_string(ctx, -2, ILibDuktape_GenericMarshal_StashTable);// [obj]

	ILibDuktape_CreateInstanceMethod(ctx, "CreateVariable", ILibDuktape_GenericMarshal_CreateVariable, DUK_VARARGS);
	ILibDuktape_CreateInstanceMethod(ctx, "CreateCallbackProxy", ILibDuktape_GenericMarshal_CreateCallbackProxy, 2);
	ILibDuktape_CreateInstanceMethod(ctx, "CreateNativeProxy", ILibDuktape_GenericMarshal_CreateNativeProxy, DUK_VARARGS);
	ILibDuktape_CreateInstanceMethod(ctx, "GetGenericGlobalCallback", ILibDuktape_GenericMarshal_GetGlobalGenericCallback, 1);
	ILibDuktape_CreateInstanceMethod(ctx, "WrapObject", ILibDuktape_GenericMarshal_WrapObject, 1);
	ILibDuktape_CreateInstanceMethod(ctx, "UnWrapObject", ILibDuktape_GenericMarshal_UnWrapObject, 1);
	ILibDuktape_CreateInstanceMethod(ctx, "StashObject", ILibDuktape_GenericMarshal_StashObject, 1);
	ILibDuktape_CreateInstanceMethod(ctx, "UnstashObject", ILibDuktape_GenericMarshal_UnstashObject, DUK_VARARGS);
	ILibDuktape_CreateInstanceMethod(ctx, "ObjectToPtr", ILibDuktape_GenericMarshal_ObjectToPtr, 1);

	ILibDuktape_CreateInstanceMethodWithIntProperty(ctx, "_VarSize", 4, "CreateInteger", ILibDuktape_GenericMarshal_CreateVariableEx, 0);
	ILibDuktape_CreateInstanceMethodWithIntProperty(ctx, "_VarSize", ((int)sizeof(void*)), "CreatePointer", ILibDuktape_GenericMarshal_CreateVariableEx, DUK_VARARGS);
	ILibDuktape_CreateInstanceMethodWithIntProperty(ctx, "_VarSize", 2, "CreateShort", ILibDuktape_GenericMarshal_CreateVariableEx, 0);
	ILibDuktape_CreateFinalizer(ctx, ILibDuktape_GenericMarshal_Finalizer);

	duk_push_int(ctx, sizeof(Duktape_MarshalledObject));
	ILibDuktape_CreateReadonlyProperty(ctx, "WrappedObjectLength");
	duk_push_int(ctx, sizeof(void*));
	ILibDuktape_CreateReadonlyProperty(ctx, "PointerSize");
}

void ILibDuktape_GenericMarshal_init(duk_context *ctx)
{
	ILibDuktape_ModSearch_AddHandler(ctx, "_GenericMarshal", ILibDuktape_GenericMarshal_Push);
}

#ifdef __DOXY__
/*!
\brief Helper class, similar to <b>'FFI'</b> that allows JavaScript to make native calls. <b>Note:</b> To use, must <b>require('_GenericMarshal')</b>
*/
class _GenericMarshal
{
public:
	/*!
	\brief Creates a NativeProxy object for the given <b>dll</b> or <b>.so</b> file.
	\param path \<String\> The <b>dll</b> or <b>.so</b> file name or path. For example, 'wlanapi.dll'
	\return <NativeProxy> NativeProxy abstraction object to use for method invocation.
	*/
	NativeProxy CreateNativeProxy(path);
	/*!
	\brief Create a NativeCallbackProxy object that can be used with a NativeProxy object for callback reception. <b>Note:</b> The last parameter of the native callback must be an opaque user state object/ptr.
	\param func The function to dispatch on callback
	\param numVars The number of parameters in the native callback function signature
	\return <NativeCallbackProxy> NativeCallbackProxy object to use for native callback reception.
	*/
	NativeCallbackProxy CreateCallbackProxy(func, numVars);

	/*!
	\brief Initializes a proxy variable to use with a NativeProxy object.
	\param sz <Integer> Amount of memory in bytes to initialize
	\return <NativeVariable> NativeVariable object to use with NativeProxy object method calls.
	*/
	NativeVariable CreateVariable(sz);
	/*!
	\brief Initializes an integer (4 bytes) as a proxy variable to use with a NativeProxy object.
	\return <NativeVariable> NativeVariable object to use with NativeProxy object method calls.
	*/
	NativeVariable CreateInteger();
	/*!
	\brief Initializes an short (2 bytes) as a proxy variable to use with a NativeProxy object.
	\return <NativeVariable> NativeVariable object to use with NativeProxy object method calls.
	*/
	NativeVariable CreateShort();
	/*!
	\brief Initializes a pointer as a proxy variable to use with a NativeProxy object. The size used is the actual size of a void* pointer.
	\return <NativeVariable> NativeVariable object to use with NativeProxy object method calls.
	*/
	NativeVariable CreatePointer();


	/*!
	\brief Variable abstraction used by NativeProxy and NativeCallbackProxy
	*/
	class NativeVariable
	{
	public:
		/*!
		\brief GET/SET property. 64 bit values are only supported on 64 bit builds. 2 and 4 bytes values are passed as int. 8 byte value passes as a pointer.
		*/
		void Val;
		/*!
		\brief GET/SET 4 byte integer property.
		*/
		void IntVal;
		/*!
		\brief GET/SET 2 byte short property.
		*/
		void ShortVal;

		/*!
		\brief Dereferences the specified memory region
		*
		NativeVariable Deref([offset], length);
		\param offset <integer> Optional offset, specifying where to start dereferencing. 0 if not specified.
		\param length <integer> The number of bytes to dereference
		\return <NativeVariable> NativeVariable object representing the dereferenced memory.
		*/
		NativeVariable Deref([offset], length);

		/*!
		\brief Property GET that returns memory as a simple string
		\return \<String\>
		*/
		object String;
		/*!
		\brief Property GET that returns memory as a simple string. <b>Note:</b> On Win32, this will perform a Wide to Ansi conversion.
		\return \<String\>
		*/
		object AnsiString;
		/*!
		\brief Property GET that returns memory as a Hex String of bytes (ie AABBCCDD)
		\return \<String\>
		*/
		object HexString;
		/*!
		\brief Property GET that returns memory as a Colon delimited Hex String of bytes (ie AA:BB:CC:DD)
		\return \<String\>
		*/
		object HexString2;
	};
	/*!
	\brief JavaScript abstraction object for a Native library. <b>Note:</b> Must call 'CreateMethod' to add instance methods, that will dispatch into the native library.
	*/
	class NativeProxy
	{
	public:
		/*!
		\brief Adds an instance method, that will proxy method calls into Native.
		\param methodName \<String\> The name of the exposed method to proxy
		\param newMethodName \<String\> The name of the instance method to add to the NativeProxy object. If not specified, the name specified by 'methodName' will be used.
		*/
		void CreateMethod(methodName[, newMethodName]);
	};
	/*!
	\brief JavaScript abstraction to proxy callbacks between Native and JavaScript, using NativeProxy
	*/
	class NativeCallbackProxy
	{
	public:
		/*!
		\brief JavaScript dispatcher that should be passed as the callback function pointer when invoking a native method using NativeProxy
		\return NativeVariable object that proxies the callback, which should be passed as the callback function when invoking a method with NativeProxy
		*/
		NativeVariable Callback;
		/*!
		\brief NativeVariable encapsulation of JavaScript dispatcher data, that must be passed as 'opaque user data' when invoking a method with NativeProxy <b>Note:</b> The callback signature must return this opaque data as the last parameter
		\return NativeVariable object passed as 'opaque user data' that is used by the callback dispatcher.
		*/
		NativeVariable State;
	};
};

#endif
