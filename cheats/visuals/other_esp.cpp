#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "../Configuration/Config.h"
#include "..\lagcompensation\local_animations.h"
#include "../Menu/MenuFramework/Framework.h"
#include "../Menu/MenuFramework/Renderer.h"
#include "../menu_alpha.h"

using namespace IdaLovesMe;

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f) //-V550
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat());
}

void otheresp::penetration_reticle()
{
	if (!c_config::get()->auto_check(c_config::get()->i["esp_en"], c_config::get()->i["esp_en_type"]))
		return;

	if (!c_config::get()->b["pencross"])
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	const auto weapon_info = weapon->get_csweapon_info();
	if (!weapon_info)
		return;

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f)
		return;

	auto color = Color(237, 42, 28, 179);
	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color(34, 139, 34, 179);

	float angle_z = math::dot_product(Vector(0, 0, 1), trace.plane.normal);
	float invangle_z = math::dot_product(Vector(0, 0, -1), trace.plane.normal);
	float angle_y = math::dot_product(Vector(0, 1, 0), trace.plane.normal);
	float invangle_y = math::dot_product(Vector(0, -1, 0), trace.plane.normal);
	float angle_x = math::dot_product(Vector(1, 0, 0), trace.plane.normal);
	float invangle_x = math::dot_product(Vector(-1, 0, 0), trace.plane.normal);


	if (angle_z > 0.5 || invangle_z > 0.5)
		otheresp::get().fill_deez_nuts(trace.endpos, Vector2D(5, 5), color, 0);
	else if (angle_y > 0.5 || invangle_y > 0.5)
		otheresp::get().fill_deez_nuts(trace.endpos, Vector2D(5, 5), color, 1);
	else if (angle_x > 0.5 || invangle_x > 0.5)
		otheresp::get().fill_deez_nuts(trace.endpos, Vector2D(5, 5), color, 2);
}

void otheresp::fill_deez_nuts(Vector center, Vector2D size, Color color, int angle) {
	Vector top_left, top_right, bot_left, bot_right;

	switch (angle) {
	case 0: // Z
		top_left = Vector(-size.x, -size.y, 0);
		top_right = Vector(size.x, -size.y, 0);

		bot_left = Vector(-size.x, size.y, 0);
		bot_right = Vector(size.x, size.y, 0);

		break;
	case 1: // Y
		top_left = Vector(-size.x, 0, -size.y);
		top_right = Vector(size.x, 0, -size.y);

		bot_left = Vector(-size.x, 0, size.y);
		bot_right = Vector(size.x, 0, size.y);

		break;
	case 2: // X
		top_left = Vector(0, -size.y, -size.x);
		top_right = Vector(0, -size.y, size.x);

		bot_left = Vector(0, size.y, -size.x);
		bot_right = Vector(0, size.y, size.x);

		break;
	}

	//top line
//    Vector c_top_left = center + add_top_left;
	Vector c_top_left = center + top_left;
	Vector c_top_right = center + top_right;

	//bottom line
	Vector c_bot_left = center + bot_left;
	Vector c_bot_right = center + bot_right;

	Vector m_flTopleft, m_flTopRight, m_flBotLeft, m_flBotRight;
	//your standard world to screen if u need one just grab from a past
	if (math::world_to_screen(c_top_left, m_flTopleft) && math::world_to_screen(c_top_right, m_flTopRight) &&
		math::world_to_screen(c_bot_left, m_flBotLeft) && math::world_to_screen(c_bot_right, m_flBotRight)) {

		Vertex_t vertices[4];
		//static int m_flTexID = g_pSurface->CreateNewTextureID(true);
		static int m_flTexID = m_surface()->CreateNewTextureID(true);
		m_surface()->DrawSetTexture(m_flTexID);
		m_surface()->DrawSetColor(color);

		vertices[0].Init(Vector2D(m_flTopleft.x, m_flTopleft.y));
		vertices[1].Init(Vector2D(m_flTopRight.x, m_flTopRight.y));
		vertices[2].Init(Vector2D(m_flBotRight.x, m_flBotRight.y));
		vertices[3].Init(Vector2D(m_flBotLeft.x, m_flBotLeft.y));

		m_surface()->DrawTexturedPolygon(4, vertices, true);
	}
}




void otheresp::velocity()
{
	if (!c_config::get()->b["velocitygraph"])
		return;

	if (!g_ctx.local())
		return;

	if (!m_engine()->IsInGame() || !g_ctx.local()->is_alive())
		return;

	static std::vector<float> velData(120, 0);

	Vector vecVelocity = g_ctx.local()->m_vecVelocity();
	float currentVelocity = sqrt(vecVelocity.x * vecVelocity.x + vecVelocity.y * vecVelocity.y);

	velData.erase(velData.begin());
	velData.push_back(currentVelocity);

	int vel = g_ctx.local()->m_vecVelocity().Length2D();

	static int width, height;
	m_engine()->GetScreenSize(width, height);
	render::get().text(fonts[VELOCITY], width / 2, height / 1.1, Color(0, 255, 100, 255), HFONT_CENTERED_X | HFONT_CENTERED_Y, "[%i]", vel);


	for (auto i = 0; i < velData.size() - 1; i++)
	{
		int cur = velData.at(i);
		int next = velData.at(i + 1);

		render::get().line(
			width / 2 + (velData.size() * 5 / 2) - (i - 1) * 5.f,
			height / 1.15 - (std::clamp(cur, 0, 450) * .2f),
			width / 2 + (velData.size() * 5 / 2) - i * 5.f,
			height / 1.15 - (std::clamp(next, 0, 450) * .2f), Color(255, 255, 255, 255)
		);
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


void otheresp::holopanel(player_t* WhoUseThisBone, int hitbox_id, bool autodir)
{

	std::string ping = std::to_string(g_ctx.globals.ping);
	char const* pincc = ping.c_str();

	std::string fps = std::to_string(g_ctx.globals.framerate);
	char const* fpsc = fps.c_str();	

	if (c_config::get()->b["holopanel"])
	{
		auto bone_pos = WhoUseThisBone->hitbox_position(hitbox_id);
		Vector angle;
		if (key_binds::get().get_key_bind_state(_THIRDPERSON))
		{
			if (math::world_to_screen(bone_pos, angle))
			{
				render::get().line(angle.x, angle.y, angle.x + 100, angle.y - 150, Color(255, 255, 255));

				render::get().rect_filled(angle.x + 100, angle.y - 155, 150, 5, Color(g_cfg.menu.menu_theme.r(), g_cfg.menu.menu_theme.g(), g_cfg.menu.menu_theme.b(), 255));
				if (g_ctx.globals.framerate < 60)
				{
					render::get().rect_filled(angle.x + 100, angle.y - 150, 150, 105 + render::get().text_heigth(fonts[NAME], "WARNING! LOW FPS") + 5, Color(0, 0, 0, 150));
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 45, Color(219, 179, 15, 255), 0, "WARNING! LOW FPS");
				}
				else
				{
					render::get().rect_filled(angle.x + 100, angle.y - 150, 150, 105, Color(0, 0, 0, 150));
				}

				if (c_config::get()->b["rage_dt"] && c_config::get()->i["rage_dt_key_style"] > KEY_NONE && c_config::get()->i["rage_dt_key_style"] < KEY_MAX && misc::get().double_tap_key)
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(117, 219, 15, 255), 0, "Doubletap");
				}
				else if (c_config::get()->b["hideshots"] && c_config::get()->i["hs_key"] > KEY_NONE && c_config::get()->i["hs_key"] < KEY_MAX && misc::get().hide_shots_key)
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(117, 219, 15, 255), 0, "HideShots");
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110, angle.y - 145, Color(255, 255, 255, 255), 0, "Exploit : ");
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Exploit : "), angle.y - 145, Color(219, 15, 15, 255), 0, "Not active");
				}

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 125, Color(255, 255, 255, 255), 0, "DoubleTap Type : ");
				if (c_config::get()->i["rage_dtmode"] == 1 or c_config::get()->i["rage_dtmode"] == 2)
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Doubletap Type : "), angle.y - 125, Color(117, 219, 15, 255), 0, "Instant");
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Doubletap Type : "), angle.y - 125, Color(219, 15, 15, 255), 0, "Default");
				}

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 105, Color(255, 255, 255, 255), 0, "Fps : ");
				if (g_ctx.globals.framerate >= 60)
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Fps : "), angle.y - 105, Color(117, 219, 15, 255), 0, fpsc);
				}
				else
				{
					render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Fps : "), angle.y - 105, Color(219, 15, 15, 255), 0, fpsc);
				}


				render::get().text(fonts[NAME], angle.x + 110, angle.y - 85, Color(255, 255, 255, 255), 0, "Ping : ");
				render::get().text(fonts[NAME], angle.x + 110 + render::get().text_width(fonts[NAME], "Ping : "), angle.y - 85, Color(255, 255, 255, 255), 0, pincc);

				render::get().text(fonts[NAME], angle.x + 110, angle.y - 65, Color(255, 255, 255, 255), 0, "ZeruaS");

			}
		}
	}
}

void otheresp::indicators()
{
	static int height, width;
	m_engine()->GetScreenSize(width, height);

	g_ctx.globals.indicator_pos = height / 2;

	static auto percent_col = [](int per) -> Color {
		int red = per < 50 ? 255 : floorf(255 - (per * 2 - 100) * 255.f / 100.f);
		int green = per > 50 ? 255 : floorf((per * 2) * 255.f / 100.f);

		return Color(red, green, 0);
	};
	auto cfg = c_config::get();

	if (cfg->m["ftind"][0] && cfg->auto_check(cfg->i["rage_sp_enabled"], cfg->i["rage_sp_enabled_style"]))
	{
		otheresp::get().m_indicators.push_back({ "SP", D3DCOLOR_RGBA(255, 255, 0,255) });
	}

	if (cfg->m["ftind"][1] && cfg->auto_check(cfg->i["rage_baim_enabled"], cfg->i["rage_baim_enabled_style"]))
	{
		otheresp::get().m_indicators.push_back({ "BAIM", D3DCOLOR_RGBA(255, 255, 255,255) });
	}

	Color PINGCOL = percent_col((float(std::clamp(g_ctx.globals.ping, 30, 200) - 64)) / 200.f * 100.f);
	if (cfg->m["ftind"][2] && (cfg->b["misc_ping_spike"] && cfg->auto_check(cfg->i["ping_spike_key"], cfg->i["ping_spike_key_style"])))
	{
		otheresp::get().m_indicators.push_back({ "PING", D3DCOLOR_RGBA(PINGCOL.r(), PINGCOL.g(), PINGCOL.b(),255) });
	}

	if (cfg->m["ftind"][3] && (cfg->b["rage_dt"] && cfg->auto_check(cfg->i["rage_dt_key"], cfg->i["rage_dt_key_style"])))
	{
		otheresp::get().m_indicators.push_back({ "DT", D3DCOLOR_RGBA(255, 255, 255, 255) });
	}

	if (cfg->m["ftind"][4] && cfg->auto_check(cfg->i["rage_fd_enabled"], cfg->i["rage_fd_enabled_style"]))
	{
		otheresp::get().m_indicators.push_back({ "DUCK", D3DCOLOR_RGBA(255, 255, 255,255) });
	}

	if (cfg->m["ftind"][5] && cfg->i["aa_fs"] == 0 && cfg->auto_check(cfg->i["freestand_key"], cfg->i["freestand_key_style"]))
	{
		otheresp::get().m_indicators.push_back({ "FS", D3DCOLOR_RGBA(255, 255, 255,255) });
	}

	Color FPSCOL = percent_col((float(std::clamp(g_ctx.globals.framerate, 30, 80) - 64)) / 200.f * 100.f);

	if (cfg->b["fps_warning"])
	{
		otheresp::get().m_indicators.push_back({ "FPS", D3DCOLOR_RGBA(FPSCOL.r(), FPSCOL.g(), FPSCOL.b(),255) });
	}

}

void otheresp::create_fonts() {

	static auto create_font2 = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};



	IndFont = create_font2("Calibri Bold", 29, FW_DONTCARE, FONTFLAG_DROPSHADOW);
	IndShadow = create_font2("Calibri Bold", 29, FW_DONTCARE, FONTFLAG_DROPSHADOW);
}

void otheresp::draw_indicators()
{

	//if (!g_ctx.local()->is_alive()) //-V807
		//return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	int h = 0;


	for (auto& indicator : m_indicators)
	{

		render::get().gradient(14, height - 300 - h - 3, render::get().text_width(IndShadow, indicator.str.c_str()) / 2, 33, Color(0, 0, 0, 0), Color(0, 0, 0, 165), GRADIENT_HORIZONTAL);
		render::get().gradient(14 + render::get().text_width(IndShadow, indicator.str.c_str()) / 2, height - 300 - h - 3, render::get().text_width(IndShadow, indicator.str.c_str()) / 2, 33, Color(0, 0, 0, 165), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		render::get().text(IndShadow, 23, height - 300 - h, Color(0, 0, 0, 200), HFONT_CENTERED_NONE, indicator.str.c_str());
		render::get().text(IndFont, 23, height - 300 - h, Color((int)get_r(indicator.color), (int)get_g(indicator.color), (int)get_b(indicator.color), (int)get_a(indicator.color)), HFONT_CENTERED_NONE, indicator.str.c_str());


		//DrawList::AddGradient(Vec2(14, height - 340 - h - 3), Vec2(Render::Draw->GetTextSize(Render::Fonts::Indicator, indicator.str.c_str()).x, 33), D3DCOLOR_RGBA(0, 0, 0, 0), D3DCOLOR_RGBA(0, 0, 0, 125));
		//DrawList::AddGradient(Vec2(14 + Render::Draw->GetTextSize(Render::Fonts::Indicator, indicator.str.c_str()).x, height - 340 - h - 3), Vec2(Render::Draw->GetTextSize(Render::Fonts::Indicator, indicator.str.c_str()).x, 33), D3DCOLOR_RGBA(0, 0, 0, 125), D3DCOLOR_RGBA(0, 0, 0, 0));
	    //Render::Draw->Text(indicator.str.c_str(), 23 + 1, height - 340 - h + 1, LEFT, false, Render::Fonts::Indicator, D3DCOLOR_RGBA(0, 0, 0, 150));
		//Render::Draw->Text(indicator.str.c_str(), 23, height - 340 - h, LEFT, false, Render::Fonts::Indicator, indicator.color);

		h += 36;
	}


	m_indicators.clear();

}

void otheresp::hitmarker_paint()
{
	auto cfg = c_config::get();
	if (!cfg->b["hitmarker"])
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (cfg->b["hitmarker"])
		{
			static int width, height;
			m_engine()->GetScreenSize(width, height);

			auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
			hitmarker.hurt_color.SetAlpha(alpha);

			auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

			render::get().line(width / 2 + 5 + offset, height / 2 - 5 - offset, width / 2 + 12 + offset, height / 2 - 12 - offset, hitmarker.hurt_color);
			render::get().line(width / 2 + 5 + offset, height / 2 + 5 + offset, width / 2 + 12 + offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 + 5 + offset, width / 2 - 12 - offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 - 5 - offset, width / 2 - 12 - offset, height / 2 - 12 - offset, hitmarker.hurt_color);
		}


	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y, damage_marker[i].hurt_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);
		}
	}
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	//
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr); //-V107

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0); //-V107
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();
	auto color_main = Color(c_config::get()->c["rage_quick_peek_assist_col"][0], c_config::get()->c["rage_quick_peek_assist_col"][1], c_config::get()->c["rage_quick_peek_assist_col"][2]);

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 2.f;

	if (!weapon->is_non_aim() && c_config::get()->b["rage_quick_peek_assist"] && c_config::get()->auto_check(c_config::get()->i["rage_quickpeek_enabled"], c_config::get()->i["rage_quickpeek_enabled_style"])) {

		render::get().Draw3DCircleGradient(position, 32, color_main, alpha);
	}
}
