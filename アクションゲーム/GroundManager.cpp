#include "GroundManager.h"
#include "Game.h"
#include "Ground.h"

void GroundManager::Init() {
    m_Polygons.clear();

    auto grounds = Game::GetInstance()->GetObjects<Ground>();
    for (auto& g : grounds) {
        const auto& vertices = g->GetVertices();
        for (int i = 0; i < vertices.size(); i += 3) {
            Collision::Polygon poly = {
                vertices[i + 0].position,
                vertices[i + 1].position,
                vertices[i + 2].position
            };
            m_Polygons.emplace_back(poly);
        }
    }
}

const std::vector<Collision::Polygon>& GroundManager::GetGroundPolygons() const {
    return m_Polygons;
}