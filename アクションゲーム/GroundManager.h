#pragma once
#include "Manager.h"
#include <vector>
#include "Collision.h"

class GroundManager :
    public Manager<GroundManager>
{
    friend class Manager<GroundManager>; // Manager ‚©‚ç new ‚Å‚«‚é‚æ‚¤‚É‚·‚é

public:
   void Init();
   const std::vector<Collision::Polygon>& GetGroundPolygons() const;
private:
   std::vector<Collision::Polygon> m_Polygons;
};

