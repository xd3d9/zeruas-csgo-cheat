#include "DiscordRpc.h"
#include "DiscordRPCSDK/Includes/discord_rpc.h"
#include "sdk/interfaces/IVEngineClient.hpp"
#include <IfaceMngr.hpp>
IVEngineClient* g_EngineClient = nullptr;

void Discord::Initialize()
{
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));
    Discord_Initialize("1093267005668339852", &Handle, 1, NULL);
}

void Discord::Update()
{
    g_EngineClient = Iface::IfaceMngr::getIface<IVEngineClient>("engine.dll", "VEngineClient");
    player_info_t playerinfo;
    DiscordRichPresence discord;
    memset(&discord, 0, sizeof(discord));
    discord.details = "Tapping with ZeruaS";
    discord.state = "v1.4.6";
    discord.largeImageKey = "icon";
    discord.startTimestamp = time(0);
    Discord_UpdatePresence(&discord);
}