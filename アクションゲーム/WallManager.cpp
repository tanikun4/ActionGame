#include "WallManager.h"
#include "Game.h"
#include "TestCube.h"//壁用オブジェクト
#include "DebugUI.h"

using namespace std;
using namespace DirectX::SimpleMath;

// 壁の設置、指定したGroundサイズに合わせて四方を囲むように壁を配置する
void WallManager::SetWall(const Vector2& groundsize, vector<Object*>& _scene_object)
{
	// Ground のサイズ
	float halfX = groundsize.x * 0.5f;
	float halfZ = groundsize.y * 0.5f;

	// Cube の厚み（Z方向とX方向で壁として使う分）
	float thickness = 1.0f; // 例：10 units
	//Cubeの縦の厚み
	float scaleY = 1.5f;
	//初期色
	DirectX::SimpleMath::Vector4 color{ 1,1,1,0.5f };//透明度を下げて見やすいようにしておく

	// 上辺（+Z側）
	TestCube* top = Game::GetInstance()->AddObject<TestCube>();
	top->SetScale(Vector3(groundsize.x * 0.05f, scaleY, thickness)); // X幅は地面いっぱい、Zは薄く
	top->SetPosition(Vector3(-halfX * 0.1f, scaleY * 10.0f, halfZ + halfZ * 0.1f)); // Yは高さ調整
	top->SetColor(color);
	_scene_object.emplace_back(top);

	// 下辺（-Z側）
	TestCube* bottom = Game::GetInstance()->AddObject<TestCube>();
	bottom->SetScale(Vector3(groundsize.x * 0.05f, scaleY, thickness));
	bottom->SetPosition(Vector3(-halfX * 0.1f, scaleY * 10.0f, -halfZ));
	bottom->SetColor(color);
	_scene_object.emplace_back(bottom);

	// 右辺（+X側）
	TestCube* right = Game::GetInstance()->AddObject<TestCube>();
	right->SetScale(Vector3(thickness, scaleY, groundsize.y * 0.05f));
	right->SetPosition(Vector3(halfX, scaleY * 10.0f, 0.0f));
	right->SetColor(color);
	_scene_object.emplace_back(right);

	// 左辺（-X側）
	TestCube* left = Game::GetInstance()->AddObject<TestCube>();
	left->SetScale(Vector3(thickness, scaleY, groundsize.y * 0.05f));
	left->SetPosition(Vector3(-halfX - halfX * 0.15f, scaleY * 10.0f, 0.0f));
	left->SetColor(color);
	_scene_object.emplace_back(left);
}

void WallManager::DebugWallStatus() {//壁の大きさや位置を操作する
	ImGui::Begin("WallStatus");

	// 元の値を保持する static 配列
	static bool initialized = false;
	static std::vector<Vector3> originalScale;
	static std::vector<Vector3> originalPos;

	// TestCube を取得
	auto cubes = Game::GetInstance()->GetObjects<TestCube>();

	// 初期値を保存
	if (!initialized) {
		originalScale.clear();
		originalPos.clear();
		for (auto& c : cubes) {
			originalScale.push_back(c->GetScale());
			originalPos.push_back(c->GetPosition());
		}
		initialized = true;
	}

	static Vector3 wall_size{};
	ImGui::SliderFloat3("WallSize", &wall_size.x, -10.0f, 10.0f);

	static Vector3 wall_pos{};
	ImGui::SliderFloat3("WallPosition", &wall_pos.x, -100.0f, 100.0f);

	static Vector4 wall_color{ 1,1,1,0.5f };
	ImGui::SliderFloat4("WallColor", &wall_color.x, 0.0f, 1.0f);

	if (ImGui::Button("Reset Status")) {
		wall_size = Vector3(0, 0, 0);
		wall_pos = Vector3(0, 0, 0);
		wall_color = Vector4(1, 1, 1, 0.5f);
	}

	vector<TestCube*> cube = Game::GetInstance()->GetObjects<TestCube>();

	// 元の値にスライダー分だけ加える
	for (size_t i = 0; i < cubes.size(); ++i) {
		cubes[i]->SetScale(originalScale[i] + wall_size);
		cubes[i]->SetPosition(originalPos[i] + wall_pos);
		cubes[i]->SetColor(wall_color);
	}

	ImGui::End();
}