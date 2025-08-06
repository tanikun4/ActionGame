#pragma once
#include "Manager.h"
#include <vector>
#include "Collision.h"

class GroundManager :
    public Manager<GroundManager>
{
public:
    void Initialize();
    const std::vector<Collision::Polygon>& GetGroundPolygons() const;
private:
    std::vector<Collision::Polygon> m_Polygons;
};

