// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "misc.h"
#include "fakelag.h"
#include "..\ragebot\aim.h"
#include "..\visuals\world_esp.h"
#include "prediction_system.h"
#include "logs.h"
#include "..\visuals\hitchams.h"
#include "../menu_alpha.h"
#include "../Configuration/Config.h"
#include <IfaceMngr.hpp>

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

void misc::EnableHiddenCVars()
{
	auto p = **reinterpret_cast<ConCommandBase***>(reinterpret_cast<DWORD>(m_cvar()) + 0x34);

	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext) {
		c->m_nFlags &= ~FCVAR_DEVELOPMENTONLY; // FCVAR_DEVELOPMENTONLY
		c->m_nFlags &= ~FCVAR_HIDDEN; // FCVAR_HIDDEN
	}
}

void misc::pingspike()
{
	int value = c_config::get()->i["misc_pingspike_val"] / 2;
	ConVar* net_fakelag = m_cvar()->FindVar(crypt_str("net_fakelag"));
	if (c_config::get()->b["misc_ping_spike"] && c_config::get()->auto_check(c_config::get()->i["ping_spike_key"], c_config::get()->i["ping_spike_key_style"]))
		net_fakelag->SetValue(value);
	else
	{
		net_fakelag->SetValue(0);
	}
}

void misc::NoDuck(CUserCmd* cmd)
{
	if (!c_config::get()->b["inf_duck"])
		return;

	if (m_gamerules()->m_bIsValveDS())
		return;

	cmd->m_buttons |= IN_BULLRUSH;
}

void misc::AutoCrouch(CUserCmd* cmd)
{
	if (fakelag::get().condition)
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!c_config::get()->auto_check(c_config::get()->i["rage_fd_enabled"], c_config::get()->i["rage_fd_enabled_style"]))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!g_ctx.globals.fakeducking && m_clientstate()->iChokedCommands != 7)
		return;

	if (m_clientstate()->iChokedCommands >= 7)
		cmd->m_buttons |= IN_DUCK;
	else
		cmd->m_buttons &= ~IN_DUCK;

	g_ctx.globals.fakeducking = true;
}

std::string GetTimeStringx()
{
	time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}
int getfpsx()
{
	IBaseClientDLL* g_CHLClient = Iface::IfaceMngr::getIface<IBaseClientDLL>("client.dll", "VClient0");
	CGlobalVarsBase* g_GlobalVars = **(CGlobalVarsBase***)((*(DWORD**)(g_CHLClient))[0] + 0x1F);
	return static_cast<int>(1.f / g_GlobalVars->m_frametime);
}

void misc::watermark()
{
	int alpha;
	int centerW, centerH;
	int monstw, monsth;
	int w, h;
	m_engine()->GetScreenSize(w, h);
	centerW = w / 2;
	centerH = h / 2;
	monstw = w;
	monsth = h;
	alpha = 255;

	Color wmline = Color(238, 75, 181, 255);

	/// PING MEASURE
	INetChannelInfo* ncis = m_engine()->GetNetChannelInfo();
	std::string incoming = g_ctx.local() ? std::to_string((int)(ncis->GetLatency(FLOW_INCOMING) * 1000)) : "0";
	std::string outgoing = g_ctx.local() ? std::to_string((int)(ncis->GetLatency(FLOW_OUTGOING) * 1000)) : "0";

	/// RECT ///
	/*g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 310, 9, (centerW * 2) - 310 + 158, 9 + 99);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 309, 10, (centerW * 2) - 309 + 156, 10 + 97);
	g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 308, 11, (centerW * 2) - 308 + 154, 11 + 95);
	g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 305, 14, (centerW * 2) - 305 + 148, 14 + 89);
	g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
	g_VGuiSurface->DrawFilledRect((centerW * 2) - 304, 15, (centerW * 2) - 304 + 146, 15 + 87);
*/
/*
long curTime;
curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

float hue = curTime / 100;

xuifloatcolor temp = xuicolor::HSV(hue / 360.f, 1, 1);
Color color = Color(temp.x, temp.y, temp.z);
*/

	m_surface()->DrawSetColor(Color(0, 0, 0));
	m_surface()->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
	m_surface()->DrawSetColor(Color(60, 60, 60));
	m_surface()->DrawFilledRect((centerW * 2) - 199, centerH + 2, (centerW * 2) - 199 + 190, (centerH + 2) + 137);
	m_surface()->DrawSetColor(Color(38, 38, 38));
	m_surface()->DrawFilledRect((centerW * 2) - 198, centerH + 3, (centerW * 2) - 198 + 188, (centerH + 3) + 135);
	m_surface()->DrawSetColor(Color(60, 60, 60));
	m_surface()->DrawFilledRect((centerW * 2) - 195, centerH + 6, (centerW * 2) - 195 + 182, (centerH + 6) + 129);
	m_surface()->DrawSetColor(Color(28, 28, 28));
	m_surface()->DrawFilledRect((centerW * 2) - 194, centerH + 7, (centerW * 2) - 194 + 180, (centerH + 7) + 127);

	m_surface()->DrawSetColor(Color(wmline));
	m_surface()->DrawFilledRect((centerW * 2) - 192, centerH + 22, (centerW * 2) - 193 + 178, (centerH + 8) + 15);
	render::get().text(fonts[NAME], (centerW * 2) - 120, centerH + 8, Color(255, 255, 255, 255), HFONT_CENTERED_NONE, ("ZeruaS"));

	/// LINE ON TOP OF WATERMARK ///
	/*
	m_surface()->DrawSetColor(Color(238, 75, 181));
	m_surface()->DrawFilledRect((centerW * 2) - 303, 16, (centerW * 2) - 303 + 144, 16 + 1);
	*/
	// GRADIENT COLORS: LEFT(30, 87, 153)  MIDDLE(243, 0, 255)  RIGHT(224, 255, 0)


	/// TEXT ///
	render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 37, Color(240, 240, 240, 255), HFONT_CENTERED_NONE, ("Current Time: %s"), GetTimeStringx().c_str());

	if (getfpsx() < 50)
	{
		render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 53, Color(255, 0, 0, 255), HFONT_CENTERED_NONE, ("Fps: %d"), getfpsx());
	}
	else
	{
		render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 53, Color(240, 240, 240, 255), HFONT_CENTERED_NONE, ("Fps: %d"), getfpsx());
	}
	// bing
	render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 69, Color(240, 240, 240, 255), HFONT_CENTERED_NONE, ("Download: %s ms"), incoming.c_str());
	render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 85, Color(240, 240, 240, 255), HFONT_CENTERED_NONE, ("Upload: %s ms"), outgoing.c_str());


	render::get().text(fonts[NAME], (centerW * 2) - 180, centerH + 117, Color(240, 240, 240, 255), HFONT_CENTERED_NONE, ("Expect The Unexpected"));


	/// SPECTATOR LIST ///

	/*  temporarily disabled.
	if (g_Options.misc_spectatorlist)
	{
		g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 200, centerH + 1, (centerW * 2) - 200 + 192, (centerH + 1) + 139);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 199, centerH + 2, (centerW * 2) - 199 + 190, (centerH + 2) + 137);
		g_VGuiSurface->DrawSetColor(Color(38, 38, 38));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 198, centerH + 3, (centerW * 2) - 198 + 188, (centerH + 3) + 135);
		g_VGuiSurface->DrawSetColor(Color(60, 60, 60));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 195, centerH + 6, (centerW * 2) - 195 + 182, (centerH + 6) + 129);
		g_VGuiSurface->DrawSetColor(Color(28, 28, 28));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 194, centerH + 7, (centerW * 2) - 194 + 180, (centerH + 7) + 127);
		g_VGuiSurface->DrawSetColor(Color(149, 184, 6));
		g_VGuiSurface->DrawFilledRect((centerW * 2) - 192, centerH + 21, (centerW * 2) - 193 + 178, (centerH + 7) + 15);
		DrawString(ui_font, (centerW * 2) - 140, centerH + 14, Color(255, 255, 255, 255), FONT_LEFT, ("Spectator List"));
	}
	*/
}

void misc::SlideWalk(CUserCmd* cmd)
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	if (antiaim::get().condition(cmd, true) && (g_cfg.misc.legs_movement == 2 && math::random_int(0, 2) == 0 || g_cfg.misc.legs_movement == 1))
	{
		if (cmd->m_forwardmove > 0.0f)
		{
			cmd->m_buttons |= IN_BACK;
			cmd->m_buttons &= ~IN_FORWARD;
		}
		else if (cmd->m_forwardmove < 0.0f)
		{
			cmd->m_buttons |= IN_FORWARD;
			cmd->m_buttons &= ~IN_BACK;
		}

		if (cmd->m_sidemove > 0.0f)
		{
			cmd->m_buttons |= IN_MOVELEFT;
			cmd->m_buttons &= ~IN_MOVERIGHT;
		}
		else if (cmd->m_sidemove < 0.0f)
		{
			cmd->m_buttons |= IN_MOVERIGHT;
			cmd->m_buttons &= ~IN_MOVELEFT;
		}
	}
	else
	{
		auto buttons = cmd->m_buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

		if (c_config::get()->i["leg_movementtype"] == 1 && math::random_int(0, 1)) //|| c_config::get()->i["leg_movementtype"] == 1)
		{
			if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
				return;

			if (cmd->m_forwardmove <= 0.0f)
				buttons |= IN_BACK;
			else
				buttons |= IN_FORWARD;

			if (cmd->m_sidemove > 0.0f)
				goto LABEL_15;
			else if (cmd->m_sidemove >= 0.0f)
				goto LABEL_18;

			goto LABEL_17;
		}
		else
			goto LABEL_18;

		if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
			return;

		if (cmd->m_forwardmove <= 0.0f) //-V779
			buttons |= IN_FORWARD;
		else
			buttons |= IN_BACK;

		if (cmd->m_sidemove > 0.0f)
		{
		LABEL_17:
			buttons |= IN_MOVELEFT;
			goto LABEL_18;
		}

		if (cmd->m_sidemove < 0.0f)
			LABEL_15:

		buttons |= IN_MOVERIGHT;

	LABEL_18:
		cmd->m_buttons = buttons;
	}
}

void misc::automatic_peek(CUserCmd* cmd, float wish_yaw)
{
	if (!g_ctx.globals.weapon->is_non_aim() && c_config::get()->b["rage_quick_peek_assist"] && c_config::get()->auto_check(c_config::get()->i["rage_quickpeek_enabled"], c_config::get()->i["rage_quickpeek_enabled_style"]))
	{
		if (g_ctx.globals.start_position.IsZero())
		{
			g_ctx.globals.start_position = g_ctx.local()->GetAbsOrigin();

			if (!(engineprediction::get().backup_data.flags & FL_ONGROUND))
			{
				Ray_t ray;
				CTraceFilterWorldAndPropsOnly filter;
				CGameTrace trace;

				ray.Init(g_ctx.globals.start_position, g_ctx.globals.start_position - Vector(0.0f, 0.0f, 8000.0f));
				m_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

				if (trace.fraction < 1.0f)
					g_ctx.globals.start_position = trace.endpos + Vector(0.0f, 0.0f, 3.0f);
			}
		}
		else
		{
			auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (cmd->m_buttons & IN_ATTACK || cmd->m_buttons & IN_ATTACK2);

			if (cmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
				g_ctx.globals.fired_shot = true;

			if (g_ctx.globals.fired_shot)
			{
				auto current_position = g_ctx.local()->GetAbsOrigin();
				auto difference = current_position - g_ctx.globals.start_position;

				if (difference.Length2D() > 1.0f)
				{
					auto velocity = Vector(difference.x * cos(wish_yaw / 180.0f * M_PI) + difference.y * sin(wish_yaw / 180.0f * M_PI), difference.y * cos(wish_yaw / 180.0f * M_PI) - difference.x * sin(wish_yaw / 180.0f * M_PI), difference.z);

					cmd->m_forwardmove = -velocity.x * 20;
					cmd->m_sidemove = velocity.y * 20;
				}
				else
				{
					g_ctx.globals.fired_shot = false;
					g_ctx.globals.start_position.Zero();
				}
			}
		}
	}
	else
	{
		g_ctx.globals.fired_shot = false;
		g_ctx.globals.start_position.Zero();
	}
}

void misc::ViewModel()
{
	if (g_cfg.esp.viewmodel_fov)
	{
		auto viewFOV = (float)g_cfg.esp.viewmodel_fov + 68.0f;
		static auto viewFOVcvar = m_cvar()->FindVar(crypt_str("viewmodel_fov"));

		if (viewFOVcvar->GetFloat() != viewFOV) //-V550
		{
			*(float*)((DWORD)&viewFOVcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewFOVcvar->SetValue(viewFOV);
		}
	}

	if (g_cfg.esp.viewmodel_x)
	{
		auto viewX = (float)g_cfg.esp.viewmodel_x / 2.0f;
		static auto viewXcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_x")); //-V807

		if (viewXcvar->GetFloat() != viewX) //-V550
		{
			*(float*)((DWORD)&viewXcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewXcvar->SetValue(viewX);
		}
	}

	if (g_cfg.esp.viewmodel_y)
	{
		auto viewY = (float)g_cfg.esp.viewmodel_y / 2.0f;
		static auto viewYcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_y"));

		if (viewYcvar->GetFloat() != viewY) //-V550
		{
			*(float*)((DWORD)&viewYcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewYcvar->SetValue(viewY);
		}
	}

	if (g_cfg.esp.viewmodel_z)
	{
		auto viewZ = (float)g_cfg.esp.viewmodel_z / 2.0f;
		static auto viewZcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_z"));

		if (viewZcvar->GetFloat() != viewZ) //-V550
		{
			*(float*)((DWORD)&viewZcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewZcvar->SetValue(viewZ);
		}
	}
}

void misc::FullBright()
{
	if (!c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]))
		return;

	static auto mat_fullbright = m_cvar()->FindVar(crypt_str("mat_fullbright"));

	if (mat_fullbright->GetBool() != c_config::get()->m["brightadj"][0])
		mat_fullbright->SetValue(c_config::get()->m["brightadj"][0]);
}

void misc::DrawGray()
{
	if (!c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]))
		return;

	static auto mat_drawgray = m_cvar()->FindVar(crypt_str("mat_drawgray"));

	if (mat_drawgray->GetBool() != g_cfg.esp.drawgray)
		mat_drawgray->SetValue(g_cfg.esp.drawgray);
}

void misc::PovArrows(player_t* e, Color color)
{
	auto cfg = c_config::get();
	auto isOnScreen = [](Vector origin, Vector& screen) -> bool
	{
		if (!math::world_to_screen(origin, screen))
			return false;

		static int iScreenWidth, iScreenHeight;
		m_engine()->GetScreenSize(iScreenWidth, iScreenHeight);

		auto xOk = iScreenWidth > screen.x;
		auto yOk = iScreenHeight > screen.y;

		return xOk && yOk;
	};

	Vector screenPos;

	if (isOnScreen(e->GetAbsOrigin(), screenPos))
		return;

	Vector viewAngles;
	m_engine()->GetViewAngles(viewAngles);

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
	auto angleYawRad = DEG2RAD(viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);

	auto radius = cfg->i["oofradius"];
	auto size = cfg->i["oofsize"];

	auto newPointX = screenCenter.x + ((((width - (size * 4)) * 0.5f) * (radius / 200.0f)/*(radius / 100.0f)*/) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
	auto newPointY = screenCenter.y + ((((height - (size * 4)) * 0.5f) * (radius / 200.0f)/*(radius / 100.0f)*/) * sin(angleYawRad));

	std::array <Vector2D, 3> points
	{
		Vector2D(newPointX - size, newPointY - size),
		Vector2D(newPointX + size, newPointY),
		Vector2D(newPointX - size, newPointY + size)
	};

	math::rotate_triangle(points, viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);
	render::get().triangle(points.at(0), points.at(1), points.at(2), color);
}

void misc::NightmodeFix()
{
	static auto in_game = false;

	if (m_engine()->IsInGame() && !in_game)
	{
		in_game = true;

		g_ctx.globals.change_materials = true;
		worldesp::get().changed = true;

		static auto skybox = m_cvar()->FindVar(crypt_str("sv_skyname"));
		worldesp::get().backup_skybox = skybox->GetString();
		return;
	}
	else if (!m_engine()->IsInGame() && in_game)
		in_game = false;

	static auto player_enable = c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]);

	if (player_enable != c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]))
	{
		player_enable = c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]);
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting = c_config::get()->m["brightadj"][1];

	if (setting != c_config::get()->m["brightadj"][1])
	{
		setting = c_config::get()->m["brightadj"][1];
		g_ctx.globals.change_materials = true;
		return;
	}

	Color wrldcol
	{
		c_config::get()->c["adjcol"][0],
		c_config::get()->c["adjcol"][1],
		c_config::get()->c["adjcol"][2],
		c_config::get()->c["adjcol"][3]
	};

	static auto setting_world = wrldcol;

	if (setting_world != wrldcol)
	{
		setting_world = wrldcol;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_props = g_cfg.esp.props_color;

	if (setting_props != g_cfg.esp.props_color)
	{
		setting_props = g_cfg.esp.props_color;
		g_ctx.globals.change_materials = true;
	}
}

void misc::aimbot_hitboxes()
{
	/*
    if (!g_cfg.player.enable)
        return;

    if (!g_cfg.player.lag_hitbox)
        return;
	*/
    auto player = (player_t*)m_entitylist()->GetClientEntity(aim::get().last_target_index);

    if (!player)
        return;

    auto model = player->GetModel();

    if (!model)
        return;

    auto studio_model = m_modelinfo()->GetStudioModel(model);

    if (!studio_model)
        return;

    auto hitbox_set = studio_model->pHitboxSet(player->m_nHitboxSet());

    if (!hitbox_set)
        return;

    hit_chams::get().add_matrix(player, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main);
}

void misc::halo()
{
	if (!c_config::get()->b["halo"])
		return;

	static auto model_index = m_modelinfo()->GetModelIndex(crypt_str("sprites/physbeam.vmt"));
	//static auto localplr = m_engine()->GetLocalPlayer();
	if (g_ctx.globals.should_update_beam_index)
		model_index = m_modelinfo()->GetModelIndex(crypt_str("sprites/physbeam.vmt"));
	std::vector<BeamInfo_t> new_beams;
	BeamInfo_t info;

	info.m_nType = TE_BEAMRINGPOINT;
	info.m_pszModelName = crypt_str("sprites/physbeam.vmt");
	info.m_nModelIndex = model_index;
	info.m_nHaloIndex = 1;
	info.m_flHaloScale = 3.0f;
	info.m_flLife = 0.05f;
	info.m_flWidth = 3.5f;
	info.m_flFadeLength = 0.0f;
	info.m_flAmplitude = 1.f;
	info.m_flRed = (float)c_config::get()->c["halo_c"][0];
	info.m_flGreen = (float)c_config::get()->c["halo_c"][1];
	info.m_flBlue = (float)c_config::get()->c["halo_c"][2];
	info.m_flBrightness = (float)c_config::get()->c["halo_c"][3];
	info.m_flSpeed = 0.0f;
	info.m_nStartFrame = 0.0f;
	info.m_flFrameRate = 0.0f;
	info.m_nSegments = -1;
	info.m_nFlags = FBEAM_FADEOUT;
	info.m_vecCenter = g_ctx.local()->GetAbsOrigin() + Vector(0.0f, 0.0f, 70.0f);
	info.m_flStartRadius = 35.0f;
	info.m_flEndRadius = 30.0f;
	info.m_bRenderable = true;

	auto beam_draw = m_viewrenderbeams()->CreateBeamRingPoint(info);

	if (beam_draw)
		m_viewrenderbeams()->DrawBeam(beam_draw);
}

void misc::trail()
{
	if (!c_config::get()->b["trail"]) {
		return;
	}
	static float rainbow;
	rainbow += 0.001f;
	if (rainbow > 1.f)
		rainbow = 0.f;

	auto rainbow_col = Color::FromHSB(rainbow, 1, 1);
	auto local_pos = g_ctx.local()->m_vecOrigin();
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0; //TE_BEAMPOINTS
	beamInfo.m_vecCenter = g_ctx.local()->GetAbsOrigin() + Vector(0.0f, 0.0f, -70.0f);
	beamInfo.m_pszModelName = "sprites/glow01.vmt";
	beamInfo.m_pszHaloName = "sprites/glow01.vmt";
	beamInfo.m_flHaloScale = 3.0;
	beamInfo.m_flWidth = 4.5f;
	beamInfo.m_flEndWidth = 4.5f;
	beamInfo.m_flFadeLength = 0.5f;
	beamInfo.m_flAmplitude = 0;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.0f;
	beamInfo.m_nStartFrame = 0.0;
	beamInfo.m_flFrameRate = 0.0;
	beamInfo.m_flRed = rainbow_col.r();
	beamInfo.m_flGreen = rainbow_col.g();
	beamInfo.m_flBlue = rainbow_col.b();
	beamInfo.m_nSegments = -1;
	beamInfo.m_bRenderable = true;
	beamInfo.m_flLife = 1;
	beamInfo.m_nFlags = FBEAM_FADEOUT; //FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM
	Beam_t* myBeam = m_viewrenderbeams()->CreateBeamPoints(beamInfo);
	if (myBeam)
		m_viewrenderbeams()->DrawBeam(myBeam);
}
void misc::rank_reveal()
{
	if (!c_config::get()->b["misc_rev_comp"])
		return;

	using RankReveal_t = bool(__cdecl*)(int*);
	static auto Fn = (RankReveal_t)(util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 A1 ? ? ? ? 85 C0 75 37")));

	int array[3] =
	{
		0,
		0,
		0
	};

	Fn(array);
}

void misc::fast_stop(CUserCmd* m_pcmd)
{
	if (!c_config::get()->b["rage_quickstop"])
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	auto pressed_move_key = m_pcmd->m_buttons & IN_FORWARD || m_pcmd->m_buttons & IN_MOVELEFT || m_pcmd->m_buttons & IN_BACK || m_pcmd->m_buttons & IN_MOVERIGHT || m_pcmd->m_buttons & IN_JUMP;

	if (pressed_move_key)
		return;

	if (!(g_cfg.antiaim.type[antiaim::get().type].desync && !g_cfg.antiaim.lby_type && (!g_ctx.globals.weapon->is_grenade() || g_cfg.esp.on_click && ~(m_pcmd->m_buttons & IN_ATTACK) && !(m_pcmd->m_buttons & IN_ATTACK2))) || antiaim::get().condition(m_pcmd)) //-V648
	{
		auto &velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
			m_engine()->GetViewAngles(real_view);

			direction.y = real_view.y - direction.y;

			Vector forward;
			math::angle_vectors(direction, forward);

			static auto cl_forwardspeed = m_cvar()->FindVar(crypt_str("cl_forwardspeed"));
			static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));

			auto negative_forward_speed = -cl_forwardspeed->GetFloat();
			auto negative_side_speed = -cl_sidespeed->GetFloat();

			auto negative_forward_direction = forward * negative_forward_speed;
			auto negative_side_direction = forward * negative_side_speed;

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;


		}
	}
	else
	{
		auto &velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
			m_engine()->GetViewAngles(real_view);

			direction.y = real_view.y - direction.y;

			Vector forward;
			math::angle_vectors(direction, forward);

			static auto cl_forwardspeed = m_cvar()->FindVar(crypt_str("cl_forwardspeed"));
			static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));

			auto negative_forward_speed = -cl_forwardspeed->GetFloat();
			auto negative_side_speed = -cl_sidespeed->GetFloat();

			auto negative_forward_direction = forward * negative_forward_speed;
			auto negative_side_direction = forward * negative_side_speed;

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;
		}
		else
		{
			auto speed = 1.01f;

			if (m_pcmd->m_buttons & IN_DUCK || g_ctx.globals.fakeducking)
				speed *= 2.94117647f;

			static auto switch_move = false;

			if (switch_move)
				m_pcmd->m_sidemove += speed;
			else
				m_pcmd->m_sidemove -= speed;

			switch_move = !switch_move;
		}
	}
}

void misc::spectators_list()
{
	if (!c_config::get()->b["spectators"])
		return;

	if (!g_ctx.local()->is_alive())
		return;

	std::vector <std::string> spectators;

	for (int i = 1; i < m_globals()->m_maxclients; i++)
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->is_alive())
			continue;

		if (e->IsDormant())
			continue;

		if (e->m_hObserverTarget().Get() != g_ctx.local())
			continue;

		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		spectators.push_back(player_info.szName);
	}

	for (auto i = 0; i < spectators.size(); i++)
	{
		int width, heigth;
		m_engine()->GetScreenSize(width, heigth);

		auto x = render::get().text_width(fonts[NAME], spectators.at(i).c_str()) + 6; //-V106
		auto y = i * 16;

		render::get().text(fonts[NAME], width - x, g_cfg.menu.watermark ? y + 30 : y + 6, Color::White, HFONT_CENTERED_NONE, spectators.at(i).c_str()); //-V106
	}
}

enum key_bind_num
{
	_AUTOFIRE,
	_LEGITBOT,
	_DOUBLETAP,
	_SAFEPOINT,
	_MIN_DAMAGE,
	_ANTI_BACKSHOT = 12,
	_M_BACK,
	_M_LEFT,
	_M_RIGHT,
	_DESYNC_FLIP,
	_THIRDPERSON,
	_AUTO_PEEK,
	_EDGE_JUMP,
	_FAKEDUCK,
	_SLOWWALK,
	_BODY_AIM,
	_RAGEBOT,
	_TRIGGERBOT,
	_L_RESOLVER_OVERRIDE,
	_FAKE_PEEK,
};

bool can_shift_shot(int ticks)
{
	if (!g_ctx.local() || !g_ctx.local()->m_hActiveWeapon())
		return false;

	auto tickbase = g_ctx.local()->m_nTickBase();
	auto curtime = m_globals()->m_intervalpertick * (tickbase - ticks);

	if (curtime < g_ctx.local()->m_flNextAttack())
		return false;

	if (curtime < g_ctx.local()->m_hActiveWeapon()->m_flNextPrimaryAttack())
		return false;

	return true;
}

void shift_cmd(CUserCmd* cmd, int amount)
{
	int cmd_number = cmd->m_command_number;
	int cnt = cmd_number - 150 * ((cmd_number + 1) / 150) + 1;
	auto new_cmd = &m_input()->m_pCommands[cnt];


	auto net_chan = m_clientstate()->pNetChannel;
	if (!new_cmd || !net_chan)
		return;

	std::memcpy(new_cmd, cmd, sizeof(CUserCmd));

	new_cmd->m_command_number = cmd->m_command_number + 1;
	new_cmd->m_buttons &= ~0x801u;


	for (int i = 0; i < amount; ++i)
	{
		int cmd_num = new_cmd->m_command_number + i;

		auto cmd_ = m_input()->GetUserCmd(cmd_num);
		auto verified_cmd = m_input()->GetVerifiedUserCmd(cmd_num);

		std::memcpy(cmd_, new_cmd, sizeof(CUserCmd));

		if (cmd_->m_tickcount != INT_MAX && m_clientstate()->iDeltaTick > 0)
			m_prediction()->Update(m_clientstate()->iDeltaTick, true, m_clientstate()->nLastCommandAck, m_clientstate()->nLastOutgoingCommand + m_clientstate()->iChokedCommands);

		cmd_->m_command_number = cmd_num;
		cmd_->m_predicted = cmd_->m_tickcount != INT_MAX;

		std::memcpy(verified_cmd, cmd_, sizeof(CUserCmd));
		verified_cmd->m_crc = cmd_->GetChecksum();

		++m_clientstate()->iChokedCommands;
		++net_chan->m_nChokedPackets;
		++net_chan->m_nOutSequenceNr;
	}

	*(int*)((uintptr_t)m_prediction() + 0xC) = -1;
	*(int*)((uintptr_t)m_prediction() + 0x1C) = 0;
}

void shift_on_peek(CUserCmd* m_pcmd)
{
	//static int shift_amount = 0;
	static int choke_timer = 0;
	//static bool inCycle = false;
	//g_ctx.globals.m_Peek.m_bIsPeeking = false;
	if (!m_clientstate()->pNetChannel)
		return;


	for (auto i = 1; i <= m_globals()->m_maxclients; i++)
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));
		if (!e->valid(true))
			continue;

		auto records = &player_records[i];
		if (records->empty())
			continue;

		auto record = &records->front();
		if (!record->valid())
			continue;

		// apply player animated data
		record->adjust_player();

		// look all ticks for get first hitable
		Vector predicted_eye_pos = g_ctx.globals.eye_pos + (engineprediction::get().backup_data.velocity * m_globals()->m_intervalpertick);
		const float predict_shift = 10;
		for (int next_shift = 1; next_shift < (predict_shift); ++next_shift)
		{
			predicted_eye_pos += (engineprediction::get().backup_data.velocity * m_globals()->m_intervalpertick);
			auto boxes = aim::get().get_hitboxes(record, true);
			for (auto box : boxes)
			{
				if (g_ctx.globals.m_Peek.m_bIsPeeking)
					break;
				auto points = aim::get().get_points(record, box);
				for (auto point : points)
				{
					auto fire_data = autowall::get().wall_penetration(predicted_eye_pos, point.point, e);
					if (fire_data.valid)
					{
						g_ctx.globals.m_Peek.m_bIsPeeking = true;

						break;
					}
				}
			}
		}
	}

	if (g_ctx.globals.m_Peek.m_bIsPeeking)
	{
		shift_cmd(m_pcmd, 2);
		g_ctx.globals.tickbase_shift = 2;
		g_ctx.globals.shift_command = m_pcmd->m_command_number;
		g_ctx.globals.shift_amount = 2;
		g_ctx.globals.m_Peek.m_bIsPeeking = false;
	}
}


bool misc::double_tap(CUserCmd* m_pcmd)
{
	if (c_config::get()->b["rage_dt"] && c_config::get()->auto_check(c_config::get()->i["rage_dt_key"], c_config::get()->i["rage_dt_key_style"]))
	{
		double_tap_enabled = true;

		static auto recharge_rapid_fire = false;
		static bool firing_dt = false;
		static auto lastdoubletaptime = 0;

		if (recharge_rapid_fire)
		{
			recharge_rapid_fire = false;
			recharging_double_tap = true;

			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.tickbase_shift = 0;
			return false;
		}

		auto max_tickbase_shift = g_ctx.globals.weapon->get_max_tickbase_shift();

		if (recharging_double_tap)
		{
			if (!(m_pcmd->m_buttons & IN_ATTACK) && g_ctx.globals.tocharge < 13 && g_ctx.globals.fixed_tickbase - lastdoubletaptime > TIME_TO_TICKS(0.4f))
			{
				recharging_double_tap = false;
				double_tap_key = true;
				firing_dt = false;
			}
			else if (m_pcmd->m_buttons & IN_ATTACK)
				firing_dt = true;
		}

		if (!c_config::get()->b["rage_enabled"])
		{
			double_tap_enabled = false;
			double_tap_key = false;

			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.tickbase_shift = 0;
			return false;
		}

		if (!c_config::get()->auto_check(c_config::get()->i["rage_dt_key"], c_config::get()->i["rage_dt_key_style"]))
		{
			double_tap_enabled = false;
			double_tap_key = false;

			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.trigger_teleport = true;
			g_ctx.globals.tickbase_shift = 0;
			return false;
		}

		static bool was_in_dt = false;

		if (c_config::get()->auto_check(c_config::get()->i["rage_dt_key"], c_config::get()->i["rage_dt_key_style"]) <= KEY_NONE || c_config::get()->auto_check(c_config::get()->i["rage_dt_key"], c_config::get()->i["rage_dt_key_style"]) >= KEY_MAX)
		{
			double_tap_enabled = false;
			double_tap_key = false;

			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.trigger_teleport = true;
			g_ctx.globals.tickbase_shift = 0;
			return false;
		}

		if (double_tap_key && c_config::get()->auto_check(c_config::get()->i["rage_dt_key"], c_config::get()->i["rage_dt_key_style"]) != g_cfg.antiaim.hide_shots_key.key)
			hide_shots_key = false;

		if (!c_config::get()->auto_check(c_config::get()->i["rage_key_enabled"], c_config::get()->i["rage_key_enabled_st"]) || g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN || g_ctx.globals.fakeducking)
		{
			double_tap_enabled = false;

			if (!firing_dt && was_in_dt)
			{
				g_ctx.globals.trigger_teleport = true;
				g_ctx.globals.teleport_amount = max_tickbase_shift;

				was_in_dt = false;
			}

			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.tickbase_shift = 0;

			return false;
		}

		if (m_gamerules()->m_bIsValveDS())
		{
			double_tap_enabled = false;
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.tickbase_shift = 0;
			return false;
		}

		if (antiaim::get().freeze_check)
			return true;

		was_in_dt = true;

		if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER
			&& g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER
			&& (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife())) //-V648
		{
			auto next_command_number = m_pcmd->m_command_number + 1;
			auto user_cmd = m_input()->GetUserCmd(next_command_number);

			lastdoubletaptime = g_ctx.globals.fixed_tickbase;

			memcpy(user_cmd, m_pcmd, sizeof(CUserCmd));
			user_cmd->m_command_number = next_command_number;

			util::copy_command(user_cmd, max_tickbase_shift);

			if (g_ctx.globals.aimbot_working)
			{
				g_ctx.globals.double_tap_aim = true;
				g_ctx.globals.double_tap_aim_check = true;
			}

			recharge_rapid_fire = true;
			shift_on_peek(m_pcmd);
			double_tap_enabled = false;
			double_tap_key = false;

			g_ctx.send_packet = true;
			firing_dt = true;
		}
		else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			g_ctx.globals.tickbase_shift = max_tickbase_shift;
	}
	return true;
}


void misc::hide_shots(CUserCmd* m_pcmd, bool should_work)
{
	hide_shots_enabled = true;

	if ((c_config::get()->b["rage_enabled"] && !c_config::get()->auto_check(c_config::get()->i["rage_key_enabled"], c_config::get()->i["rage_key_enabled_st"])))
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!should_work && double_tap_key)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;
		return;
	}

	if (!hide_shots_key)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	double_tap_key = false;

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (g_ctx.globals.fakeducking)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (antiaim::get().freeze_check)
		return;

	g_ctx.globals.next_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 9;

	auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);
	auto weapon_shoot = m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife() || revolver_shoot;

	if (g_ctx.send_packet && !g_ctx.globals.weapon->is_grenade() && weapon_shoot)
		g_ctx.globals.tickbase_shift = g_ctx.globals.next_tickbase_shift;
}






	