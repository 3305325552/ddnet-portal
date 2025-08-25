#include "portal.h"
#include "character.h"

#include <game/client/laser_data.h>

CPortal::CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, float Length)
    : CEntity(pGameWorld, CGameWorld::ENTTYPE_PORTAL)
{
    m_Owner = Owner;
    m_Pos = Pos;
    m_Dir = Dir;

    vec2 Perpendicular = vec2(-m_Dir.y, m_Dir.x);
    m_From = m_Pos + Perpendicular * (Length / 2.0f);
    m_To = m_Pos - Perpendicular * (Length / 2.0f);

    CCharacter *pOwnerChar = pGameWorld->GetCharacterById(Owner);
    if(pOwnerChar->m_LastPortalType != 0)
    {
        m_Type = 0;
        pOwnerChar->m_LastPortalType = 0;
    }
    else
    {
        m_Type = 1;
        pOwnerChar->m_LastPortalType = 1;
    }

    pGameWorld->InsertEntity(this);
}

void CPortal::Tick()
{
    // dbg_msg("portal", "ticking");
}

CLaserData CPortal::GetData()
{
    CLaserData Result;
    Result.m_From = m_From;
    Result.m_To = m_To;
    Result.m_Type = m_Type;

    return Result;
}