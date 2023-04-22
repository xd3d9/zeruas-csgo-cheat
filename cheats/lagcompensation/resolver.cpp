
#include "animation_system.h"
#include "..\ragebot\aim.h"
void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset(bool fake)
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

bool resolver::low_delta()
{
	auto record = player_record;
	if (!g_ctx.local()->is_alive())
		return false;
	float angle_diff = math::AngleDiff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 35 && lby > -35)
		return true;
	return false;
}

bool resolver::low_delta2()
{
	auto record = player_record;
	if (!g_ctx.local()->is_alive())
		return false;
	float angle_diff = math::AngleDiff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 20 && lby > -20)
		return true;
	return false;
	
}

bool freestand_target(player_t* target, float* yaw)
{
    float dmg_left = 0.f;
    float dmg_right = 0.f;

    static auto get_rotated_pos = [](Vector start, float rotation, float distance)
    {
        float rad = DEG2RAD(rotation);
        start.x += cos(rad) * distance;
        start.y += sin(rad) * distance;

        return start;
    };

    const auto local = g_ctx.local();

    if (!local || !target || !local->is_alive())
        return false;

    Vector local_eye_pos = target->get_shoot_position();
    Vector eye_pos = local->get_shoot_position();
    Vector angle = (local_eye_pos, eye_pos);

    auto backwards = target->m_angEyeAngles().y; // angle.y;

    Vector pos_left = get_rotated_pos(eye_pos, angle.y + 90.f, 60.f);
    Vector pos_right = get_rotated_pos(eye_pos, angle.y - 90.f, -60.f);

    const auto wall_left = (local_eye_pos, pos_left,
        nullptr, nullptr, local);

    const auto wall_right = (local_eye_pos, pos_right,
        nullptr, nullptr, local);



    if (dmg_left == 0.f && dmg_right == 0.f)
    {
        *yaw = backwards;
        return false;
    }

    // we can hit both sides, lets force backwards
    if (fabsf(dmg_left - dmg_right) < 5.f)
    {
        *yaw = backwards;
        return false;
    }

    bool direction = dmg_left > dmg_right;
    *yaw = direction ? angle.y - 90.f : angle.y + 90.f;

    return true;
}

float resolver::GetBackwardYaw(player_t* ent) {
	return math::calculate_angle(g_ctx.local()->GetAbsOrigin(), ent->GetAbsOrigin()).y;
}

float resolver::GetForwardYaw(player_t* ent) {
	return math::normalize_yaw(GetBackwardYaw(ent) - 180.f);
}

float flAngleMod(float flAngle)
{
    return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65530));
}

void resolver::resolve_yaw(bool fake)
{
    player_info_t player_info;

    if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
        return;

    if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) //-V807
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    if (g_ctx.globals.missed_shots[player->EntIndex()] >= 2 || g_ctx.globals.missed_shots[player->EntIndex()] && aim::get().last_target[player->EntIndex()].record.type != LBY)
    {
        switch (last_side)
        {
        case RESOLVER_ORIGINAL:
            g_ctx.globals.missed_shots[player->EntIndex()] = 0;
            fake = true;
            break;
        case RESOLVER_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;

            was_first_bruteforce = true;
            return;
        case RESOLVER_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;

            was_second_bruteforce = true;
            return;
        case RESOLVER_LOW_FIRST:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_SECOND;
            return;
        case RESOLVER_LOW_SECOND:
            player_record->type = BRUTEFORCE;
            player_record->side = RESOLVER_LOW_FIRST;
            return;
        }
    }

    auto animstate = player->get_animation_state();

    if (!animstate)
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
    auto valid_lby = true;

    if (animstate->m_velocity > 1.1f || fabs(animstate->flUpVelocity) > 100.f)
        valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;

    if (fabs(delta) > 30.0f && valid_lby)
    {
        if (g_ctx.globals.missed_shots[player->EntIndex()])
            delta = -delta;

        if (delta > 30.0f)
        {
            player_record->type = LBY;
            player_record->side = RESOLVER_LOW_FIRST;
        }
        else if (delta < -30.0f)
        {
            player_record->type = LBY;
            player_record->side = RESOLVER_LOW_SECOND;
        }
    }
    else
    {
        auto trace = false;

        if (m_globals()->m_curtime - lock_side > 2.0f)
        {
            auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
            auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

            if (first_visible != second_visible)
            {
                trace = true;
                side = second_visible;
                lock_side = m_globals()->m_curtime;
            }
            else
            {
                auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
                auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

                if (fabs(first_position - second_position) > 1.0f)
                    side = first_position > second_position;
            }
        }
        else
            trace = true;

        if (side)
        {
            player_record->type = trace ? TRACE : DIRECTIONAL;
            player_record->side = RESOLVER_FIRST;
        }
        else
        {
            player_record->type = trace ? TRACE : DIRECTIONAL;
            player_record->side = RESOLVER_SECOND;
        }
    }
}

float resolver::resolve_pitch()
{
	/*
	if (player->m_angEyeAngles().x > 50.f) {
		player->m_angEyeAngles().x = 90.f;
	}
	if (player->m_angEyeAngles().x < -50.f) {
		player->m_angEyeAngles().x = -90.f;
	} // ar ari sachiro :D
	*/
	if (fabs(original_pitch) > 85.0f)
		fake = true;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return RESOLVER_ORIGINAL;
	}
	return original_pitch;
}