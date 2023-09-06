#pragma once
#include "discord-rpc/discord_register.h"
#include "discord-rpc/discord_rpc.h"

struct D_DisconInfo {
	D_DisconInfo(int _errCode, const char* _msg) {
		errCode = _errCode;
		msg = _msg;
	}
	int errCode;
	const char* msg;
};

// discord event types
typedef void(*D_UserEvent)(const DiscordUser*);				// Discord Ready / Join Request
typedef void(*D_DisconEvent)(D_DisconInfo);					// Disconnected / Error
typedef void(*D_JoinEvent)(const char* secret);				// Join / Spectate

// like c# delegates 
template<typename t, typename p>
class CEventHandler {
public:

	CEventHandler() {
		handlers = CUtlVector<t>();
	}

	void operator+=(t handler) {
		handlers.AddToTail(handler);
	}

	void Clear() {
		handlers.Purge();
	}

	void Fire(p info) {
		for (int i = 0; i < handlers.Count(); i++)
			handlers[i](info);
	}

private:
	CUtlVector<t> handlers;
};

// hanlders all interactions with discord
class C_DiscRPC {
public:

	C_DiscRPC();

	static void Init();
	static void Shutdown();
	static void LevelInitPreEntity(const char* pszMapName);
	static void LevelShutdown();

	class C_DiscRPCHandler {
	public:
		CEventHandler<D_UserEvent, const DiscordUser*>	DiscordReady;	// discord ready
		CEventHandler<D_UserEvent, const DiscordUser*>	DiscordJoinReq;	// join request
		CEventHandler<D_DisconEvent, D_DisconInfo>		DiscordDiscon;	// disconnect
		CEventHandler<D_DisconEvent, D_DisconInfo>		DiscordError;	// error
		CEventHandler<D_JoinEvent, const char*>			DiscordJoin;	// join
		CEventHandler<D_JoinEvent, const char*>			DiscordSpec;	// spectate
	};

protected:

	static void OnReady(const DiscordUser* connectedUser);
	static void OnJoinReq(const DiscordUser* connectedUser);
	static void OnDiscon(int errCode, const char* msg);
	static void OnError(int errCode, const char* msg);
	static void OnJoin(const char* secret);
	static void OnSpectate(const char* secret);

};

extern C_DiscRPC* g_pDiscord;
extern C_DiscRPC::C_DiscRPCHandler* g_pDiscordHandlers;