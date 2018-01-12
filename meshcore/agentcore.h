/*
Copyright 2006 - 2017 Intel Corporation

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

#ifndef __AGENTCORE__
#define __AGENTCORE__


typedef char JS_ENGINE_CONTEXT[16];

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#include <Windows.h>
#include <WinBase.h>
#endif

#include "microstack/ILibParsers.h"
#include "microstack/ILibWebServer.h"
#include "microstack/ILibWebClient.h"
#include "microstack/ILibSimpleDataStore.h"
#include "microstack/ILibRemoteLogging.h"
#include "microstack/ILibProcessPipe.h"
#include "microscript/duktape.h"
#include "microscript/ILibWebServer_Duktape.h"
#include "microscript/ILibParsers_Duktape.h"
#include "microscript/ILibWebClient_Duktape.h"
#include "microscript/ILibAsyncSocket_Duktape.h"
#include "microscript/ILibDuktape_WebRTC.h"
#include "microscript/ILibDuktape_http.h"
#include "microscript/ILibDuktape_net.h"
#include "microscript/ILibDuktapeModSearch.h"
#include "microscript/ILibDuktape_GenericMarshal.h"
#include "microscript/ILibDuktape_SimpleDataStore.h"
#include "microscript/ILibDuktape_fs.h"
#include "microstack/ILibProcessPipe.h"
#include "microstack/ILibCrypto.h"

typedef enum AgentIdentifiers
{
	AGENTID_UNKNOWN = 0,  //!< Self-update not supported
	AGENTID_WIN32_CONSOLE = 1,  //!< Windows x86 console app
	AGENTID_WIN64_CONSOLE = 2,  //!< Windows x86-64 console app
	AGENTID_WIN32_SERVICE = 3,  //!< Windows x86 service
	AGENTID_WIN64_SERVICE = 4,  //!< Windows x86-64 service
	AGENTID_LINUX32 = 5,  //!< Linux x86
	AGENTID_LINUX64 = 6,  //!< Linux x64
	AGENTID_MIPS = 7,  //!< MIPS (used for home routers)
	AGENTID_XEN_x86 = 8,  //!< XEN Virtualization
	AGENTID_ANDROID_ARM = 9,  //!< Android ARM devices
	AGENTID_LINUX_ARM = 10, //!< Linux/ARM, used for PlugPC
	AGENTID_MAC_OSX = 11, //!< Apple MAC OS X
	AGENTID_ANDROID_x86 = 12, //!< Android/x86
	AGENTID_POGOPLUG_ARM = 13, //!< PogoPlug/ARM
	AGENTID_ANDROID_APK = 14, //!< Android Market
	AGENTID_LINUX_POKY_X86 = 15, //!< Intel Galileo
	AGENTID_MAC_OSX64 = 16, //!< Apple MAC OSX (64 bit)
	AGENTID_CHROMEOS = 17, //!< ChromeOS
	AGENTID_LINUXPOKY64 = 18, //!< Linux Poky (64 bit)
	AGENTID_LINUX_NOKVM_X86 = 19, //!< Linux NOKVM (32 bit)
	AGENTID_LINUX_NOKVM_X64 = 20, //!< Linux NOKVM (64 bit)
	AGENTID_WIN32_MIN_CONSOLE = 21, //!< Windows MinCore Console (32 bit)
	AGENTID_WIN32_MIN_SERVICE = 22,				//!< Windows MinCore Service (32 bit)
	AGENTID_NODEJS = 23							//!< NodeJS agent
}AgentIdentifiers;

// MeshServer to/from MeshAgent commands.
// Commands 0 to 9 are reserved for client/server authentication, once authenticated they can't be used and must not be processed.
// Commands 10 and above must only be processed if the server is authenticated. All these commands have 2 bytes commandid + 2 bytes requestid.
// Commands with an id that starts with '{' (123, 0x7B) are reserved for JSON commands, that is commands 31488 (0x7B00) to 31743 (0x7BFF)
typedef enum MeshCommands_Binary
{
	MeshCommand_AuthRequest				= 1,    // Server web certificate public key sha384 hash + agent or server nonce
	MeshCommand_AuthVerify				= 2,    // Agent or server signature
	MeshCommand_AuthInfo				= 3,	// Agent information
	MeshCommand_AuthConfirm             = 4,	// Server confirm to the agent that is it authenticated
	MeshCommand_ServerId				= 5,	// Optional, agent sends the expected serverid to the server. Useful if the server has many server certificates.
	MeshCommand_CoreModule				= 10,	// New core modules to be used instead of the old one, if empty, remove the core module
	MeshCommand_CoreModuleHash			= 11,	// Request/return the SHA384 hash of the core module
	MeshCommand_AgentHash				= 12,	// Request/return the SHA384 hash of the agent executable
	MeshCommand_AgentUpdate				= 13,   // Indicate the start and end of the mesh agent binary transfer
	MeshCommand_AgentUpdateBlock		= 14,   // Part of the mesh agent sent from the server to the agent, confirmation/flowcontrol from agent to server
	MeshCommand_AgentTag				= 15	// Send the mesh agent tag to the server
} MeshCommands_Binary;

#pragma pack(push,1)
typedef struct MeshAgent_Commands_SCRIPT_CreateContext
{
	unsigned short CommandType;
	JS_ENGINE_CONTEXT ContextGUID;
	unsigned int SecurityFlags;
	unsigned int ExecutionTimeout;
}MeshAgent_Commands_SCRIPT_CreateContext;
typedef struct MeshAgent_Commands_SCRIPT_DeleteContext
{
	unsigned short CommandType;
	JS_ENGINE_CONTEXT ContextGUID;
}MeshAgent_Commands_SCRIPT_DeleteContext;
typedef struct MeshAgent_Commands_SCRIPT_ExecuteString
{
	unsigned short CommandType;
	JS_ENGINE_CONTEXT ContextGUID;
	char script[];
}MeshAgent_Commands_SCRIPT_ExecuteString;
#pragma pack(pop)

typedef struct MeshAgentDuktapePtrs
{
	struct MeshAgentDuktapePtrs *Next;
	duk_context *ctx;
	void *MeshAgentObject;
	void *OnCommand;
	void *OnConnect;
	void *OnReady;
}MeshAgentDuktapePtrs;

typedef enum MeshAgentHost_BatteryInfo
{
	MeshAgentHost_BatteryInfo_HIGH			= 1,
	MeshAgentHost_BatteryInfo_LOW			= 2,
	MeshAgentHost_BatteryInfo_CRITICAL		= 4,
	MeshAgentHost_BatteryInfo_CHARGING		= 8,
	MeshAgentHost_BatteryInfo_NONE			= 3,
	MeshAgentHost_BatteryInfo_UNKNOWN		= 0,
}MeshAgentHost_BatteryInfo;

extern char* MeshAgentHost_BatteryInfo_STRINGS[];

typedef struct MeshAgentHostContainer
{
	void* chain;
	int slaveMode;

	duk_context *meshCoreCtx;
	char *meshCoreCtx_embeddedScript;
	int meshCoreCtx_embeddedScriptLen;
	MeshAgentDuktapePtrs *DuktapeMeshBindings;
	ILibProcessPipe_Manager *pipeManager;

	char* exePath;
	ILibWebClient_RequestManager httpClientManager;
	ILibSimpleDataStore masterDb;
	ILibWebClient_StateObject controlChannel;
	struct sockaddr_in6* proxyServer;
	
	int localScript;
	int version;
	char hostname[255];
	char serveruri[1024];
	AgentIdentifiers agentID;
	int serverIndex;
	int retryTime;
	MeshAgentHost_BatteryInfo batteryState;
	char meshId[UTIL_SHA384_HASHSIZE];
	int performSelfUpdate;
	int disableUpdate;

	char agentHash[UTIL_SHA384_HASHSIZE];
	char serverHash[UTIL_SHA384_HASHSIZE];
#ifndef MICROSTACK_NOTLS
	struct util_cert selfcert;
	struct util_cert selftlscert;
	struct util_cert selftlsclientcert;
	char serverWebHash[UTIL_SHA384_HASHSIZE];
#endif

	char serverNonce[UTIL_SHA384_HASHSIZE];
	char agentNonce[UTIL_SHA384_HASHSIZE];
	int serverAuthState;

	int controlChannel_idleTimeout_seconds;
	char g_selfid[UTIL_SHA384_HASHSIZE];
	void* microLMS;
	void* multicastDiscovery;
	char* multicastServerUrl;
	int serverConnectionState;
	int exitCode;
#ifndef WIN32
	int dbRetryCount;
#endif
}MeshAgentHostContainer;

MeshAgentHostContainer* MeshAgent_Create();
void MeshAgent_Destroy(MeshAgentHostContainer* agent);

int MeshAgent_Start(MeshAgentHostContainer *agent, int argc, char **argv);
void MeshAgent_Stop(MeshAgentHostContainer *agent);
void MeshAgent_PerformSelfUpdate(char* selfpath, char* exepath, int argc, char **argv);
char* MeshAgent_MakeAbsolutePath(char *basePath, char *localPath);

#endif
