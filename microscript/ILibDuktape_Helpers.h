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

#ifndef ___ILIBDUKTAPE_HELPERS___
#define ___ILIBDUKTAPE_HELPERS___

#include "duktape.h"
#ifndef MICROSTACK_NOTLS
#include <openssl/ssl.h>
#endif

typedef void(*ILibDuktape_HelperEvent)(duk_context *ctx, void *user);

#define ILibDuktape_NativeUncaughtExceptionPtr				"\xFF_NativeUncaughtExceptionPtr"
#define ILibDuktape_NativeUncaughtExceptionUserPtr			"\xFF_NativeUncaughtExceptionUserPtr"

#define ILibDuktape_MeshAgent_Cert_NonLeaf					"\xFF_selfcert"		
#define ILibDuktape_MeshAgent_Cert_Client					"\xFF_selftlsclientcert"
#define ILibDuktape_MeshAgent_Cert_Server					"\xFF_selftlscert"
#define CONTEXT_GUID_BUFFER									"_CONTEXT_GUID"
#define ILibDuktape_Context_Chain							"\xFF_chainptr"
#define ILibDuktape_OBJID									"_ObjectID"

#define ILibDuktape_CR2HTTP									"\xFF_CR2HTTP"
#define ILibDuktape_CR2Options								"\xFF_CR2Options"

char* Duktape_GetContextGuidHex(duk_context *ctx);
void *Duktape_GetChain(duk_context *ctx);
char *Duktape_GetStashKey(void* value);
char* Duktape_GetBuffer(duk_context *ctx, duk_idx_t i, duk_size_t *bufLen);
void Duktape_CreateEnum(duk_context *ctx, char* enumName, char** fieldNames, int * fieldValues, int numFields);
char* Duktape_GetStringPropertyValueEx(duk_context *ctx, duk_idx_t i, char* propertyName, char* defaultValue, duk_size_t *len);
#define Duktape_GetStringPropertyValue(ctx, i, propertyName, defaultValue) Duktape_GetStringPropertyValueEx(ctx, i, propertyName, defaultValue, NULL)
int Duktape_GetIntPropertyValue(duk_context *ctx, duk_idx_t i, char* propertyName, int defaultValue);
void *Duktape_GetPointerProperty(duk_context *ctx, duk_idx_t i, char* propertyName);
void *Duktape_GetHeapptrProperty(duk_context *ctx, duk_idx_t i, char* propertyName);
int Duktape_GetBooleanProperty(duk_context *ctx, duk_idx_t i, char *propertyName, int defaultValue);
struct sockaddr_in6* Duktape_IPAddress4_FromString(char* address, unsigned short port);
struct sockaddr_in6* Duktape_IPAddress6_FromString(char* address, unsigned short port);
void ILibDuktape_SockAddrToOptions(duk_context *ctx, struct sockaddr_in6 *addr);
void *ILibDuktape_GetProcessObject(duk_context *ctx);

typedef void(*ILibDuktape_NativeUncaughtExceptionHandler)(duk_context *ctx, char *msg, void *user);
void ILibDuktape_SetNativeUncaughtExceptionHandler(duk_context *ctx, ILibDuktape_NativeUncaughtExceptionHandler handler, void *user);

void ILibDuktape_Process_UncaughtException(duk_context *ctx);
void ILibDuktape_Process_UncaughtExceptionEx(duk_context *ctx, char *format, ...);

duk_ret_t ILibDuktape_Error(duk_context *ctx, char *format, ...);
typedef void(*ILibDuktape_IndependentFinalizerHandler)(duk_context *ctx, void *object);
int ILibDuktape_Process_GetExitCode(duk_context *ctx);

void ILibDuktape_CreateEventWithGetter(duk_context *ctx, char *propName, duk_c_function getterMethod);
void ILibDuktape_CreateEventWithGetterEx(duk_context *ctx, char *propName, void *heapptr);
void ILibDuktape_CreateEventWithGetterAndCustomProperty(duk_context *ctx, char *customPropName, char *propName, duk_c_function getterMethod);
void ILibDuktape_CreateEventWithSetter(duk_context *ctx, char *propName, char *propNamePtr, void **hptr);
void ILibDuktape_CreateEventWithSetterEx(duk_context *ctx, char *propName, duk_c_function setterMethod);
void ILibDuktape_CreateEventWithGetterAndSetter(duk_context *ctx, char *propName, char *propNamePtr, void **hptr, duk_c_function getterMethod);
void ILibDuktape_CreateEventWithGetterAndSetterEx(duk_context *ctx, char *propName, duk_c_function getterMethod, duk_c_function setterMethod);
void ILibDuktape_CreateEventWithGetterAndSetterWithMetaData(duk_context *ctx, char* metaDataPropName, duk_idx_t metaDataPropIndex, char *propName, duk_c_function getterMethod, duk_c_function setterMethod);
#define ILibDuktape_CreateEventWithGetterAndSetterWithIntMetaData(context, metaDataPropName, metaDataIntValue, propName, getterMethod, setterMethod) duk_push_int(context, metaDataIntValue);ILibDuktape_CreateEventWithGetterAndSetterWithMetaData(context, metaDataPropName, -1, propName, getterMethod, setterMethod);
#define ILibDuktape_CreateInstanceMethod(context, methodName, funcImpl, numArgs) duk_push_c_function(context, funcImpl, numArgs); duk_put_prop_string(context, -2, methodName)
#define ILibDuktape_CreateInstanceMethodWithPropertyEx(context, propName, propIndex, methodName, funcImpl, numArgs) duk_dup(context, propIndex);if(propIndex<0){duk_remove(context, propIndex - 1);};duk_push_c_function(context, funcImpl, numArgs);duk_swap_top(context, -2);duk_put_prop_string(context, -2, propName);duk_put_prop_string(context, -2, methodName);
#define ILibDuktape_CreateInstanceMethodWithStringProperty(context, propName, propValue, methodName, funcImpl, numArgs) duk_push_string(context, propValue);ILibDuktape_CreateInstanceMethodWithPropertyEx(context, propName, -1, methodName, funcImpl, numArgs);
#define ILibDuktape_CreateInstanceMethodWithBooleanProperty(context, propName, propValue, methodName, funcImpl, numArgs) duk_push_c_function(context, funcImpl, numArgs);duk_push_boolean(context, propValue);duk_put_prop_string(ctx, -2, propName);duk_put_prop_string(ctx, -2, methodName);
#define ILibDuktape_CreateInstanceMethodWithIntProperty(context, propName, propValue, methodName, funcImpl, numArgs) duk_push_c_function(context, funcImpl, numArgs);duk_push_int(context, propValue);duk_put_prop_string(ctx, -2, propName);duk_put_prop_string(ctx, -2, methodName);
#define ILibDuktape_CreateInstanceMethodWithNumberProperty(context, propName, propValue, methodName, funcImpl, numArgs) duk_push_c_function(context, funcImpl, numArgs);duk_push_number(context, (propValue));duk_put_prop_string(ctx, -2, propName);duk_put_prop_string(ctx, -2, methodName);
void ILibDuktape_CreateInstanceMethodWithProperties(duk_context *ctx, char *funcName, duk_c_function funcImpl, duk_idx_t numArgs, unsigned int propertyCount, ...);
duk_idx_t duk_push_int_ex(duk_context *ctx, duk_int_t val);

void ILibDuktape_CreateProperty_InstanceMethod(duk_context *ctx, char *methodName, duk_c_function impl, duk_idx_t argCount);
void ILibDuktape_CreateProperty_InstanceMethodEx(duk_context *ctx, char *methodName, void *funcHeapPtr);
void ILibDuktape_CreateReadonlyProperty(duk_context *ctx, char *propName);
#define ILibDuktape_CreateReadonlyProperty_int(ctx, propName, propValue) duk_push_int(ctx, propValue);ILibDuktape_CreateReadonlyProperty(ctx, propName)
void ILibDuktape_CreateFinalizer(duk_context *ctx, duk_c_function func);

void *ILibDuktape_Memory_Alloc(duk_context *ctx, duk_size_t size);
void *ILibDuktape_Memory_AllocEx(duk_context *ctx, duk_idx_t index, duk_size_t size);
void ILibDuktape_Helper_AddHeapFinalizer(duk_context *ctx, ILibDuktape_HelperEvent handler, void *user);

void ILibDuktape_Push_ObjectStash(duk_context *ctx);

void ILibDuktape_PointerValidation_Init(duk_context *ctx);
void ILibDuktape_ValidatePointer(void *chain, void *ptr);
void ILibDuktape_InValidatePointer(void *chain, void *ptr);
int ILibDuktape_IsPointerValid(void *chain, void *ptr);
#define ILibDuktape_ValidateHeapPointer(ctx, objIdx) ILibDuktape_ValidatePointer(Duktape_GetChain(ctx), duk_get_heapptr(ctx, objIdx))
#define ILibDuktape_InValidateHeapPointer(ctx, objIdx) ILibDuktape_InValidatePointer(Duktape_GetChain(ctx), duk_get_heapptr(ctx, objIdx))

typedef void(*ILibDuktape_ImmediateHandler)(duk_context *ctx, void ** args, int argsLen);
void* ILibDuktape_Immediate(duk_context *ctx, void ** args, int argsLen, ILibDuktape_ImmediateHandler callback);
int ILibDuktape_GetReferenceCount(duk_context *ctx, duk_idx_t i);

#define ILibDuktape_WriteID(ctx, id) duk_push_string(ctx, id);duk_put_prop_string(ctx, -2, ILibDuktape_OBJID)

#endif