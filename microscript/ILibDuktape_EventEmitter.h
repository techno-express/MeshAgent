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

#ifndef __ILibDuktape_EVENT_EMITTER__
#define __ILibDuktape_EVENT_EMITTER__

#include "duktape.h"
#include "microstack/ILibParsers.h"

typedef void(*ILibDuktape_EventEmitter_Handler)(duk_context *ctx, void *object, char *eventName, void *duk_eventArgs);
typedef struct ILibDuktape_EventEmitter
{
	duk_context *ctx;
	void *object;
	void *tmpObject;
	unsigned int *totalListeners;
	ILibHashtable eventTable;
}ILibDuktape_EventEmitter;
typedef void(*ILibDuktape_EventEmitter_HookHandler)(ILibDuktape_EventEmitter *sender, char *eventName, void *hookedCallback);

ILibDuktape_EventEmitter* ILibDuktape_EventEmitter_Create(duk_context *ctx);
ILibDuktape_EventEmitter* ILibDuktape_EventEmitter_GetEmitter(duk_context *ctx, duk_idx_t i);
ILibDuktape_EventEmitter* ILibDuktape_EventEmitter_GetEmitter_fromThis(duk_context *ctx);
#define ILibDuktape_EventEmitter_GetEmitter_fromCurrent(ctx) ILibDuktape_EventEmitter_GetEmitter(ctx, -1)
ILibDuktape_EventEmitter* ILibDuktape_EventEmitter_GetEmitter_fromObject(duk_context *ctx, void *objHeapptr);

void ILibDuktape_EventEmitter_Init(duk_context *ctx);
int ILibDuktape_EventEmitter_GetEventCount(ILibDuktape_EventEmitter *emitter);
void ILibDuktape_EventEmitter_RemoveAllListeners(ILibDuktape_EventEmitter *emitter, char *eventName);								// Invokes JavaScript method EventEmitter.removeAllListeners()
void ILibDuktape_EventEmitter_CreateEventEx(ILibDuktape_EventEmitter *emitter, char *eventName);									// Create Event with virtual dispatcher
int ILibDuktape_EventEmitter_AddOnce(ILibDuktape_EventEmitter *emitter, char *eventName, void *heapptr);							// Add native event handler 'once'
int ILibDuktape_EventEmitter_AddOnceEx(ILibDuktape_EventEmitter *emitter, char *eventName, duk_c_function func, duk_idx_t funcArgs);
int ILibDuktape_EventEmitter_AddOnceEx3(duk_context *ctx, duk_idx_t idx, char *eventName, duk_c_function func);
int ILibDuktape_EventEmitter_PrependOnce(duk_context *ctx, duk_idx_t i, char *eventName, duk_c_function func);
int ILibDuktape_EventEmitter_HasListeners(ILibDuktape_EventEmitter *emitter, char *eventName);	
#define ILibDuktape_EventEmitter_HasListenersEx(ctx, idx, eventName) ILibDuktape_EventEmitter_HasListeners(ILibDuktape_EventEmitter_GetEmitter(ctx, idx), eventName)

#define ILibDuktape_EventEmitter_AddOnceEx2(ctx, idx, eventName, func, argCount) ILibDuktape_EventEmitter_AddOnceEx3(ctx, idx, eventName, func)
#define ILibDuktape_EventEmitter_SetupEmit(ctx, heapptr, eventName) duk_push_heapptr((ctx), heapptr);duk_get_prop_string((ctx), -1, "emit");duk_swap_top((ctx), -2);duk_push_string((ctx), eventName)

int ILibDuktape_EventEmitter_AddOn(ILibDuktape_EventEmitter *emitter, char *eventName, void *heapptr);								// Add native event handler
int ILibDuktape_EventEmitter_AddOnEx(duk_context *ctx, duk_idx_t idx, char *eventName, duk_c_function func);

void ILibDuktape_EventEmitter_AddHook(ILibDuktape_EventEmitter *emitter, char *eventName, ILibDuktape_EventEmitter_HookHandler handler);
void ILibDuktape_EventEmitter_ForwardEvent(duk_context *ctx, duk_idx_t eventSourceIndex, char *sourceEventName, duk_idx_t eventTargetIndex, char *targetEventName);

#endif
