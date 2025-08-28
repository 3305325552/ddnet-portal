#include "portal.h"
#include "character.h"

#include <game/generated/protocol.h>
#include <game/collision.h>
#include <game/server/gamecontext.h>

CPortal::CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, float Length)
    : CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
    m_Owner = Owner;
    m_Pos = Pos;
    m_Dir = Dir;

    vec2 Perpendicular = vec2(-m_Dir.y, m_Dir.x);
    m_From = m_Pos + Perpendicular * (Length / 2.0f);
    m_To = m_Pos - Perpendicular * (Length / 2.0f);

    CCharacter *pOwnerChar = GameServer()->GetPlayerChar(Owner);
    if(pOwnerChar->m_LastPortalType != 0)
    {
        m_Type = 0;
        if(pGameWorld->m_aaPortals[Owner][m_Type]) GameServer()->m_World.RemoveEntity(pGameWorld->m_aaPortals[Owner][m_Type]);
        pGameWorld->m_aaPortals[Owner][m_Type] = this;
        pOwnerChar->m_LastPortalType = 0;
        if(pGameWorld->m_aaPortals[Owner][!m_Type]) 
        {
            m_pLinkedPortal = pGameWorld->m_aaPortals[Owner][!m_Type];
            m_pLinkedPortal->m_pLinkedPortal = this;
        }
    }
    else
    {
        m_Type = 1;
        if(pGameWorld->m_aaPortals[Owner][m_Type]) GameServer()->m_World.RemoveEntity(pGameWorld->m_aaPortals[Owner][m_Type]);
        pGameWorld->m_aaPortals[Owner][m_Type] = this;
        pOwnerChar->m_LastPortalType = 1;
        if(pGameWorld->m_aaPortals[Owner][!m_Type])
        {
            m_pLinkedPortal = pGameWorld->m_aaPortals[Owner][!m_Type];
            m_pLinkedPortal->m_pLinkedPortal = this;
        }
    }
		// for(int Index : vIndices)

    pGameWorld->InsertEntity(this);
}

CPortal::~CPortal()
{
    for(int i = 0; i < 2; i++)
        GameServer()->m_World.m_aaPortals[m_Owner][i] = nullptr;
}

void CPortal::Tick()
{
    // dbg_msg("portal", "ticking");
}

void CPortal::Reset()
{
	m_MarkedForDestroy = true;
}

void CPortal::Snap(int SnappingClient)
{
	// Only players who can see the targeted player can see the plasma bullet
	CCharacter *pTarget = GameServer()->GetPlayerChar(m_Owner);
	if(!pTarget || !pTarget->CanSnapCharacter(SnappingClient))
	{
		return;
	}

	// Only players with the plasma bullet in their field of view or who want to see everything will receive the snap
	if(NetworkClipped(SnappingClient))
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);

	GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion), GetId(),
		m_To, m_From, 0, m_Owner, LASERTYPE_PORTAL, m_Type, m_Number);
}

void CPortal::SwapClients(int Client1, int Client2)
{
	m_Owner = m_Owner == Client1 ? Client2 : m_Owner == Client2 ? Client1 : m_Owner;
}

bool CPortal::IntersectCharacter(vec2 CharPos1, vec2 CharPos2, float Radius, vec2 &TeleOutPos, float &VelRotation)
{
    if(!m_pLinkedPortal) return false;

    bool Intersected = false;
    
    // if(distance(CharPos, m_From) <= Radius || distance(CharPos, m_To) <= Radius) Intersected = true;
    float t, Dist;

    float d = distance(CharPos1, CharPos2);
	int End(d + 1);

    for(int i = 0; i < End; i++)
    {
        vec2 Point = mix(CharPos1, CharPos2, i / (float)End);
        IntersectCharacter(Point, Dist, t);
        if(Dist <= Radius)
        {
            Intersected = true;
            break;
        }
    }

    if(Intersected)
    {
        TeleOutPos = m_pLinkedPortal->GetOutPos(t, Dist);
        VelRotation = m_pLinkedPortal->GetVelocityRotation(m_Dir);
        return true;
    }
    else
        return false;
}

void CPortal::IntersectCharacter(vec2 CharPos, float &Distance, float &t)
{
    vec2 AB = m_To - m_From;
    vec2 AC = CharPos - (m_From + m_Dir * 28.f);

    t = dot(AC, AB) / dot(AB, AB);
    t = std::clamp(t, 0.0f, 1.0f);

    Distance = distance(CharPos, m_From + AB * t);
}

vec2 CPortal::GetOutPos(float Ratio, float Distance)
{
    vec2 OutPos = m_From + (m_To - m_From) * Ratio + m_Dir * 18.f;
    if(Collision()->TestBox(OutPos, vec2(28.f, 28.f)))
        return m_Pos + m_Dir * 18.f;
    else
        return OutPos;
}

float CPortal::GetVelocityRotation(vec2 Dir)
{
    if(m_Dir.y < -0.01f && Dir.y < -0.01f) return 180.f;
    if(fabs(dot(m_Dir, Dir)) < 0.01f) return (angle(m_Dir) - angle(-Dir)) / pi * 180.f;
    else return 0.f;
}