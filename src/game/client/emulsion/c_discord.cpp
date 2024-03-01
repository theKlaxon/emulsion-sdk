#include "cbase.h"
#include "c_discord.h"
#include "cdll_client_int.h"
#include "discord-rpc/discord_register.h"
#include "discord-rpc/discord_rpc.h"

#include <string>

static ConVar cl_discord_appid("cl_discord_appid", "675151519565217794", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT);
static int64_t startTimestamp = time_t(0);

const int64 discord_appid = 675151519565217794;

static C_DiscRPC* g_pDiscord = new C_DiscRPC();
C_DiscRPC::C_DiscRPCHandler* g_pDiscordHandlers;

void ReadyTest(const DiscordUser* p) {
	Msg("Hello Discord!\n");
}

C_DiscRPC::C_DiscRPC() {
	g_pDiscordHandlers = new C_DiscRPC::C_DiscRPCHandler();
	g_pDiscordHandlers->DiscordReady = CEventHandler<D_UserEvent, const DiscordUser*>();
	g_pDiscordHandlers->DiscordJoinReq = CEventHandler<D_UserEvent, const DiscordUser*>();
	g_pDiscordHandlers->DiscordDiscon = CEventHandler<D_DisconEvent, D_DisconInfo>();
	g_pDiscordHandlers->DiscordError = CEventHandler<D_DisconEvent, D_DisconInfo>();
	g_pDiscordHandlers->DiscordJoin = CEventHandler<D_JoinEvent, const char*>();
	g_pDiscordHandlers->DiscordSpec = CEventHandler<D_JoinEvent, const char*>();

	g_pDiscordHandlers->DiscordReady += ReadyTest;
}

//
// standard discord rpc stuff
// 

void C_DiscRPC::Init() {
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));

	handlers.ready = OnReady;
	handlers.disconnected = OnDiscon;
	handlers.errored = OnError;
	handlers.joinGame = OnJoin;
	handlers.spectateGame = OnSpectate;
	handlers.joinRequest = OnJoinReq;

	char appid[255];
	sprintf(appid, "%d", engine->GetAppID());

	std::string discstrstr = std::to_string(discord_appid);
	const char* discstr = discstrstr.c_str();

	Discord_Initialize(discstr, &handlers, 1, appid);

	if (!g_bTextMode)
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));

		discordPresence.state = "In-Game";
		discordPresence.details = "Main Menu";
		discordPresence.startTimestamp = int64_t(0);// startTimestamp;
		discordPresence.largeImageKey = "emulsion_icon";
		Discord_UpdatePresence(&discordPresence);
	}
}

void C_DiscRPC::Shutdown() {
	Discord_Shutdown();
}

void C_DiscRPC::LevelInitPreEntity(const char* pszMapName) {
	if (!g_bTextMode)
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));

		char buffer[256];
		discordPresence.state = "In-Game";
		sprintf(buffer, "Map: %s", pszMapName);
		discordPresence.details = buffer;
		discordPresence.largeImageKey = "final";
		Discord_UpdatePresence(&discordPresence);
	}
}

void C_DiscRPC::LevelShutdown() {
	if (!g_bTextMode)
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));

		discordPresence.state = "In-Game";
		discordPresence.details = "Main Menu";
		discordPresence.startTimestamp = startTimestamp;
		discordPresence.largeImageKey = "final";
		Discord_UpdatePresence(&discordPresence);
	}
}

// 
// discord rpc events
// 

void C_DiscRPC::OnReady(const DiscordUser* connectedUser) {
	g_pDiscordHandlers->DiscordReady.Fire(connectedUser);
}

void C_DiscRPC::OnJoinReq(const DiscordUser* request) {
	g_pDiscordHandlers->DiscordJoinReq.Fire(request);
}

void C_DiscRPC::OnDiscon(int errCode, const char* msg) {
	g_pDiscordHandlers->DiscordDiscon.Fire(D_DisconInfo(errCode, msg));
}

void C_DiscRPC::OnError(int errCode, const char* msg) {
	g_pDiscordHandlers->DiscordError.Fire(D_DisconInfo(errCode, msg));
}

void C_DiscRPC::OnJoin(const char* secret) {
	g_pDiscordHandlers->DiscordJoin.Fire(secret);
}

void C_DiscRPC::OnSpectate(const char* secret) {
	g_pDiscordHandlers->DiscordSpec.Fire(secret);
}