#pragma once

#include <game/client/prediction/entity.h>

class CLaserData;

class CPortal : public CEntity
{
private:
    int m_Owner;
    vec2 m_From;
    vec2 m_To;
    vec2 m_Dir;
    int m_Type;

public:
    CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, float Length);

    int GetOwner() const { return m_Owner; }
    void Tick() override;
    CLaserData GetData();
};