#pragma once

#include <game/server/entity.h>

class CPortal : public CEntity
{
private:
    int m_Owner;
    vec2 m_From;
    vec2 m_To;
    vec2 m_Dir;
    int m_Type;
    bool m_Horizontal;

    CPortal *m_pLinkedPortal;

public:
    CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, float Length);
    ~CPortal();

    void Tick() override;
    void Reset() override;
    void Snap(int SnappingClient) override;
	void SwapClients(int Client1, int Client2) override;

    int GetType() const { return m_Type; }
    vec2 GetDirection() const { return m_Dir; }
    bool IsLinked() const { return m_pLinkedPortal != nullptr; }
    bool IntersectCharacter(vec2 CharPos1, vec2 CharPos2, float Radius, vec2 &TeleOutPos, float &VelRotattion);
    void IntersectCharacter(vec2 CharPos, float &Distance, float &t);
    vec2 GetOutPos(float Ratio, float Distance);
};