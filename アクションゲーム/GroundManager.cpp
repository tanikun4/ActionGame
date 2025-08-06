#include "GroundManager.h"
#include "Game.h"

void GroundManager::Initialize() {
    m_Polygons.clear();

    auto grounds = Game::GetInstance()->GetObjects<Ground>();
    for (auto& g : grounds) {
        const auto& vertices = g->GetVertices();
        for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
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