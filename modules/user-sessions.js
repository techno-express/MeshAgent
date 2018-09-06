/*
Copyright 2018 Intel Corporation

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

var NOTIFY_FOR_THIS_SESSION = 0;
var NOTIFY_FOR_ALL_SESSIONS = 1;
var WM_WTSSESSION_CHANGE = 0x02B1;
var WM_POWERBROADCAST = 0x218;
var PBT_POWERSETTINGCHANGE = 0x8013;
var PBT_APMSUSPEND = 0x4;
var PBT_APMRESUMESUSPEND = 0x7;
var PBT_APMRESUMEAUTOMATIC = 0x12;
var PBT_APMPOWERSTATUSCHANGE = 0xA;

var WTS_CONSOLE_CONNECT         = (0x1);
var WTS_CONSOLE_DISCONNECT      = (0x2);
var WTS_REMOTE_CONNECT          = (0x3);
var WTS_REMOTE_DISCONNECT       = (0x4);
var WTS_SESSION_LOGON           = (0x5);
var WTS_SESSION_LOGOFF          = (0x6);
var WTS_SESSION_LOCK            = (0x7);
var WTS_SESSION_UNLOCK          = (0x8);
var WTS_SESSION_REMOTE_CONTROL  = (0x9);
var WTS_SESSION_CREATE          = (0xA);
var WTS_SESSION_TERMINATE       = (0xB);

var GUID_ACDC_POWER_SOURCE;
var GUID_BATTERY_PERCENTAGE_REMAINING;
var GUID_CONSOLE_DISPLAY_STATE;

function UserSessions()
{
    this._ObjectID = 'user-sessions';
    require('events').EventEmitter.call(this, true)
        .createEvent('changed')
        .createEvent('locked')
        .createEvent('unlocked');

    this.enumerateUsers = function enumerateUsers()
    {
        var promise = require('promise');
        var p = new promise(function (res, rej)
        {
            this.__resolver = res;
            this.__rejector = rej;
        });
        p.__handler = function __handler(users)
        {
            p.__resolver(users);
        };
        this.Current(p.__handler);
        p.parent = this;
        return (p);
    }

    if (process.platform == 'win32')
    {
        this._serviceHooked = false;
        this._marshal = require('_GenericMarshal');
        this._kernel32 = this._marshal.CreateNativeProxy('Kernel32.dll');
        this._kernel32.CreateMethod('GetLastError');
        this._wts = this._marshal.CreateNativeProxy('Wtsapi32.dll');
        this._wts.CreateMethod('WTSEnumerateSessionsA');
        this._wts.CreateMethod('WTSQuerySessionInformationA');
        this._wts.CreateMethod('WTSRegisterSessionNotification');
        this._wts.CreateMethod('WTSUnRegisterSessionNotification');
        this._wts.CreateMethod('WTSFreeMemory');
        this._user32 = this._marshal.CreateNativeProxy('user32.dll');
        this._user32.CreateMethod('RegisterPowerSettingNotification');
        this._user32.CreateMethod('UnregisterPowerSettingNotification');
        this._rpcrt = this._marshal.CreateNativeProxy('Rpcrt4.dll');
        this._rpcrt.CreateMethod('UuidFromStringA');
        this._rpcrt.StringToUUID = function StringToUUID(guid)
        {
            var retVal = StringToUUID.us._marshal.CreateVariable(16);
            if(StringToUUID.us._rpcrt.UuidFromStringA(StringToUUID.us._marshal.CreateVariable(guid), retVal).Val == 0)
            {
                return (retVal);
            }
            else
            {
                throw ('Could not convert string to UUID');
            }
        }
        this._rpcrt.StringToUUID.us = this;

        GUID_ACDC_POWER_SOURCE = this._rpcrt.StringToUUID('5d3e9a59-e9D5-4b00-a6bd-ff34ff516548');
        GUID_BATTERY_PERCENTAGE_REMAINING = this._rpcrt.StringToUUID('a7ad8041-b45a-4cae-87a3-eecbb468a9e1');
        GUID_CONSOLE_DISPLAY_STATE = this._rpcrt.StringToUUID('6fe69556-704a-47a0-8f24-c28d936fda47');

        this.SessionStates = ['Active', 'Connected', 'ConnectQuery', 'Shadow', 'Disconnected', 'Idle', 'Listening', 'Reset', 'Down', 'Init'];
        this.InfoClass =
            {
                'WTSInitialProgram': 0,
                'WTSApplicationName': 1,
                'WTSWorkingDirectory': 2,
                'WTSOEMId': 3,
                'WTSSessionId': 4,
                'WTSUserName': 5,
                'WTSWinStationName': 6,
                'WTSDomainName': 7,
                'WTSConnectState': 8,
                'WTSClientBuildNumber': 9,
                'WTSClientName': 10,
                'WTSClientDirectory': 11,
                'WTSClientProductId': 12,
                'WTSClientHardwareId': 13,
                'WTSClientAddress': 14,
                'WTSClientDisplay': 15,
                'WTSClientProtocolType': 16,
                'WTSIdleTime': 17,
                'WTSLogonTime': 18,
                'WTSIncomingBytes': 19,
                'WTSOutgoingBytes': 20,
                'WTSIncomingFrames': 21,
                'WTSOutgoingFrames': 22,
                'WTSClientInfo': 23,
                'WTSSessionInfo': 24,
                'WTSSessionInfoEx': 25,
                'WTSConfigInfo': 26,
                'WTSValidationInfo': 27,
                'WTSSessionAddressV4': 28,
                'WTSIsRemoteSession': 29
            };

        this.getSessionAttribute = function getSessionAttribute(sessionId, attr)
        {
            var buffer = this._marshal.CreatePointer();
            var bytesReturned = this._marshal.CreateVariable(4);

            if (this._wts.WTSQuerySessionInformationA(0, sessionId, attr, buffer, bytesReturned).Val == 0)
            {
                throw ('Error calling WTSQuerySessionInformation: ' + this._kernel32.GetLastError.Val);
            }

            var retVal = buffer.Deref().String;

            this._wts.WTSFreeMemory(buffer.Deref());
            return (retVal);
        };

        this.Current = function Current(cb)
        {
            var retVal = {};
            var pinfo = this._marshal.CreatePointer();
            var count = this._marshal.CreateVariable(4);
            if (this._wts.WTSEnumerateSessionsA(0, 0, 1, pinfo, count).Val == 0)
            {
                throw ('Error calling WTSEnumerateSessionsA: ' + this._kernel32.GetLastError().Val);
            }

            for (var i = 0; i < count.toBuffer().readUInt32LE() ; ++i)
            {
                var info = pinfo.Deref().Deref(i * (this._marshal.PointerSize == 4 ? 12 : 24), this._marshal.PointerSize == 4 ? 12 : 24);
                var j = { SessionId: info.toBuffer().readUInt32LE() };
                j.StationName = info.Deref(this._marshal.PointerSize == 4 ? 4 : 8, this._marshal.PointerSize).Deref().String;
                j.State = this.SessionStates[info.Deref(this._marshal.PointerSize == 4 ? 8 : 16, 4).toBuffer().readUInt32LE()];
                if (j.State == 'Active') {
                    j.Username = this.getSessionAttribute(j.SessionId, this.InfoClass.WTSUserName);
                    j.Domain = this.getSessionAttribute(j.SessionId, this.InfoClass.WTSDomainName);
                }
                retVal[j.SessionId] = j;
            }

            this._wts.WTSFreeMemory(pinfo.Deref());

            Object.defineProperty(retVal, 'Active', { value: showActiveOnly(retVal) });
            if (cb) { cb(retVal); }
            return (retVal);
        };


        // We need to spin up a message pump, and fetch a window handle
        var message_pump = require('win-message-pump');
        this._messagepump = new message_pump({ filter: WM_WTSSESSION_CHANGE }); this._messagepump.parent = this;     
        this._messagepump.on('exit', function (code) { this.parent._wts.WTSUnRegisterSessionNotification(this.parent.hwnd); });
        this._messagepump.on('hwnd', function (h)
        {
            this.parent.hwnd = h;

            // Now that we have a window handle, we can register it to receive Windows Messages
            this.parent._wts.WTSRegisterSessionNotification(this.parent.hwnd, NOTIFY_FOR_ALL_SESSIONS);
            this.parent._user32.ACDC_H = this.parent._user32.RegisterPowerSettingNotification(this.parent.hwnd, GUID_ACDC_POWER_SOURCE, 0);
            this.parent._user32.BATT_H = this.parent._user32.RegisterPowerSettingNotification(this.parent.hwnd, GUID_BATTERY_PERCENTAGE_REMAINING, 0);
            this.parent._user32.DISP_H = this.parent._user32.RegisterPowerSettingNotification(this.parent.hwnd, GUID_CONSOLE_DISPLAY_STATE, 0);
        });
        this._messagepump.on('message', function (msg)
        {
            switch(msg.message)
            {
                case WM_WTSSESSION_CHANGE:
                    switch(msg.wparam)
                    {
                        case WTS_SESSION_LOCK:
                            this.parent.enumerateUsers().then(function (users)
                            {
                                if (users[msg.lparam]) { this.parent.emit('locked', users[msg.lparam]); }
                            });
                            break;
                        case WTS_SESSION_UNLOCK:
                            this.parent.enumerateUsers().then(function (users)
                            {
                                if (users[msg.lparam]) { this.parent.emit('unlocked', users[msg.lparam]); }
                            });
                            break;
                        case WTS_SESSION_LOGON:
                        case WTS_SESSION_LOGOFF:
                            this.parent.emit('changed');
                            break;
                    }
                    break;
                case WM_POWERBROADCAST:
                    switch(msg.wparam)
                    {
                        default:
                            console.log('WM_POWERBROADCAST [UNKNOWN wparam]: ' + msg.wparam);
                            break;
                        case PBT_APMSUSPEND:
                            require('power-monitor').emit('sx', 'SLEEP');
                            break;
                        case PBT_APMRESUMEAUTOMATIC:
                            require('power-monitor').emit('sx', 'RESUME_NON_INTERACTIVE');
                            break;
                        case PBT_APMRESUMESUSPEND:
                            require('power-monitor').emit('sx', 'RESUME_INTERACTIVE');
                            break;
                        case PBT_APMPOWERSTATUSCHANGE:
                            require('power-monitor').emit('changed');
                            break;
                        case PBT_POWERSETTINGCHANGE:
                            var lparam = this.parent._marshal.CreatePointer(Buffer.from(msg.lparam_hex, 'hex'));
                            var data = lparam.Deref(20, lparam.Deref(16, 4).toBuffer().readUInt32LE(0)).toBuffer();
                            switch(lparam.Deref(0, 16).toBuffer().toString('hex'))
                            {
                                case GUID_ACDC_POWER_SOURCE.Deref(0, 16).toBuffer().toString('hex'):
                                    switch(data.readUInt32LE(0))
                                    {
                                        case 0:
                                            require('power-monitor').emit('acdc', 'AC');
                                            break;
                                        case 1:
                                            require('power-monitor').emit('acdc', 'BATTERY');
                                            break;
                                        case 2:
                                            require('power-monitor').emit('acdc', 'HOT');
                                            break;
                                    }
                                    break;
                                case GUID_BATTERY_PERCENTAGE_REMAINING.Deref(0, 16).toBuffer().toString('hex'):
                                    require('power-monitor').emit('batteryLevel', data.readUInt32LE(0));
                                    break;
                                case GUID_CONSOLE_DISPLAY_STATE.Deref(0, 16).toBuffer().toString('hex'):
                                    switch(data.readUInt32LE(0))
                                    {
                                        case 0:
                                            require('power-monitor').emit('display', 'OFF');
                                            break;
                                        case 1:
                                            require('power-monitor').emit('display', 'ON');
                                            break;
                                        case 2:
                                            require('power-monitor').emit('display', 'DIMMED');
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                default:
                    break;
            }
        });
    }
    else
    {
        var dbus = require('linux-dbus');
        this._linuxWatcher = require('fs').watch('/var/run/utmp');
        this._linuxWatcher.user_session = this;
        this._linuxWatcher.on('change', function (a, b)
        {
            this.user_session.emit('changed');
        });
        this.Self = function Self()
        {
            var promise = require('promise');
            var p = new promise(function (res, rej)
            {
                this.__resolver = res; this.__rejector = rej;
                this.__child = require('child_process').execFile('/usr/bin/id', ['id', '-u']);
                this.__child.promise = this;
                this.__child.stdout._txt = '';
                this.__child.stdout.on('data', function (chunk) { this._txt += chunk.toString(); });
                this.__child.on('exit', function (code)
                {
                    try
                    {
                        parseInt(this.stdout._txt);
                    }
                    catch (e)
                    {
                        this.promise.__rejector('invalid uid');
                        return;
                    }

                    var id = parseInt(this.stdout._txt);
                    this.promise.__resolver(id);
                });
            });
            return (p);
        };
        this.Current = function Current(cb)
        {
            var retVal = {};
            retVal._ObjectID = 'UserSession'
            Object.defineProperty(retVal, '_callback', { value: cb });
            Object.defineProperty(retVal, '_child', { value: require('child_process').execFile('/usr/bin/last', ['last', '-f', '/var/run/utmp']) });

            retVal._child.Parent = retVal;
            retVal._child._txt = '';
            retVal._child.on('exit', function (code)
            {
                var lines = this._txt.split('\n');
                var sessions = [];
                var users = {};

                for(var i in lines)
                {
                    if (lines[i])
                    {
                        var tokens = getTokens(lines[i]);
                        var s = { Username: tokens[0], SessionId: tokens[1] }
                        if (tokens[3].includes('still logged in'))
                        {
                            s.State = 'Active';
                        }
                        else
                        {
                            s.LastActive = tokens[3];
                        }

                        sessions.push(s);
                    }
                }
                sessions.pop();


                var usernames = {};
                var promises = [];

                for (var i in sessions)
                {
                    if (sessions[i].Username != 'reboot')
                    {
                        users[sessions[i].SessionId] = sessions[i];
                        if(usernames[sessions[i].Username] == null)
                        {
                            usernames[sessions[i].Username] = -1;
                        }
                    }
                }

                try
                {
                    require('promise');
                }
                catch(e)
                {
                    Object.defineProperty(users, 'Active', { value: showActiveOnly(users) });
                    if (this.Parent._callback) { this.Parent._callback.call(this.Parent, users); }
                    return;
                }

                var promise = require('promise');
                for (var n in usernames)
                {
                    var p = new promise(function (res, rej)
                    {
                        this.__username = n;
                        this.__resolver = res; this.__rejector = rej;
                        this.__child = require('child_process').execFile('/usr/bin/id', ['id', '-u', n]);
                        this.__child.promise = this;
                        this.__child.stdout._txt = '';
                        this.__child.stdout.on('data', function (chunk) { this._txt += chunk.toString(); });
                        this.__child.on('exit', function (code)
                        {
                            try
                            {
                                parseInt(this.stdout._txt);
                            }
                            catch(e)
                            {
                                this.promise.__rejector('invalid uid');
                                return;
                            }

                            var id = parseInt(this.stdout._txt);
                            this.promise.__resolver(id);
                        });
                    });
                    promises.push(p);
                }
                promise.all(promises).then(function (plist)
                {
                    // Done
                    var table = {};
                    for(var i in plist)
                    {
                        table[plist[i].__username] = plist[i]._internal.completedArgs[0];
                    }
                    for(var i in users)
                    {
                        users[i].uid = table[users[i].Username];
                    }
                    Object.defineProperty(users, 'Active', { value: showActiveOnly(users) });
                    if (retVal._callback) { retVal._callback.call(retVal, users); }
                }, function (reason)
                {
                    // Failed
                    Object.defineProperty(users, 'Active', { value: showActiveOnly(users) });
                    if (retVal._callback) { retVal._callback.call(retVal, users); }
                });
            });
            retVal._child.stdout.Parent = retVal._child;
            retVal._child.stdout.on('data', function (chunk) { this.Parent._txt += chunk.toString(); });

            return (retVal);
        }
        this._recheckLoggedInUsers = function _recheckLoggedInUsers()
        {
            this.enumerateUsers().then(function (u)
            {

                if (u.Active.length > 0)
                {
                    // There is already a user logged in, so we can monitor DBUS for lock/unlock
                    if (this.parent._linux_lock_watcher != null && this.parent._linux_lock_watcher.uid != u.Active[0].uid)
                    {
                        delete this.parent._linux_lock_watcher;
                    }
                    this.parent._linux_lock_watcher = new dbus(process.env['XDG_CURRENT_DESKTOP'] == 'Unity' ? 'com.ubuntu.Upstart0_6' : 'org.gnome.ScreenSaver', u.Active[0].uid);
                    this.parent._linux_lock_watcher.user_session = this.parent;
                    this.parent._linux_lock_watcher.on('signal', function (s)
                    {
                        var p = this.user_session.enumerateUsers();
                        p.signalData = s.data[0];
                        p.then(function (u)
                        {
                            switch (this.signalData)
                            {
                                case true:
                                case 'desktop-lock':
                                    this.parent.emit('locked', u.Active[0]);
                                    break;
                                case false:
                                case 'desktop-unlock':
                                    this.parent.emit('unlocked', u.Active[0]);
                                    break;
                            }
                        });
                    });
                }
                else if (this.parent._linux_lock_watcher != null)
                {
                    delete this.parent._linux_lock_watcher;
                }
            });

        };
        this.on('changed', this._recheckLoggedInUsers); // For linux Lock/Unlock monitoring, we need to watch for LogOn/LogOff, and keep track of the UID.

        
        // First step, is to see if there is a user logged in:
        this._recheckLoggedInUsers();
    }
}
function showActiveOnly(source)
{
    var retVal = [];
    var unique = {};
    var usernames = [];
    var tmp;

    for (var i in source)
    {
        if (source[i].State == 'Active')
        {
            retVal.push(source[i]);
            tmp = (source[i].Domain ? (source[i].Domain + '\\') : '') + source[i].Username;
            if (!unique[tmp]) { unique[tmp] = tmp;}
        }
    }

    for (var i in unique)
    {
        usernames.push(i);
    }

    Object.defineProperty(retVal, 'usernames', { value: usernames });
    return (retVal);
}
function getTokens(str)
{
    var columns = [];
    var i;

    columns.push(str.substring(0, (i=str.indexOf(' '))));
    while (str[++i] == ' ');
    columns.push(str.substring(i, (i=str.substring(i).indexOf(' ') + i)));
    while (str[++i] == ' ');
    columns.push(str.substring(i, (i=str.substring(i).indexOf(' ') + i)));
    while (str[++i] == ' ');
    var status = str.substring(i).trim();
    columns.push(status);

    return (columns);
}

module.exports = new UserSessions();