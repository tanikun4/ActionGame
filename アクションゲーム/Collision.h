#pragma once

#include <simplemath.h>
#include <variant>

class TestCube;
namespace Collision
{
	struct Base {
		Base() = default;
	};

	// ライン（無限の長さの線）
	struct Line {
		DirectX::SimpleMath::Vector3 point; // 通過地点
		DirectX::SimpleMath::Vector3 vec; // 線の方向ベクトル
	};

	// プレーン(無限の広さの平面)
	struct Plane {
		DirectX::SimpleMath::Vector3 point; // 平面上の1点
		DirectX::SimpleMath::Vector3 normal; // 平面の法線ベクトル
		//float d;         // 平面の方程式: ax + by + cz + d = 0 の d
	};

	// セグメント（有限の長さの線分）
	struct Segment {
		DirectX::SimpleMath::Vector3 start; // 始点
		DirectX::SimpleMath::Vector3 end; // 終点
	};

	// 三角形ポリゴン（有限の広さの平面）
	struct Polygon {
		const DirectX::SimpleMath::Vector3 p0{}; //頂点0
		const DirectX::SimpleMath::Vector3 p1{}; //頂点1
		const DirectX::SimpleMath::Vector3 p2{}; //頂点2
	};


	// 球体
	struct Sphere : public Base {
		DirectX::SimpleMath::Vector3 center{}; // 中心
		float radius{}; // 半径
		Sphere(const DirectX::SimpleMath::Vector3& c,const float r)
			: Base()       // ← 親を明示初期化（必要に応じて引数付きで）
			, center(c)
			, radius(r) {
		}
	};

	//// 円柱
	//struct Cylinder {
	//	DirectX::SimpleMath::Vector3 top; // 上
	//	DirectX::SimpleMath::Vector3 bottom; // 底
	//	float radius; // 半径
	//};

	//// カプセル
	//struct Capsule {
	//	DirectX::SimpleMath::Vector3 top; // 上の中心 
	//	DirectX::SimpleMath::Vector3 bottom; // 底の中心
	//	float radius; // 半径
	//};

	// BOX定義
	struct AABB : public Base {
		DirectX::SimpleMath::Vector3 min{};
		DirectX::SimpleMath::Vector3 max{};
		AABB(const DirectX::SimpleMath::Vector3& mi, const DirectX::SimpleMath::Vector3& ma)
			: Base()       // ← 親を明示初期化（必要に応じて引数付きで）
			, min(mi)
			, max(ma) {
		}
	};

	struct OBB : public Base {
		DirectX::SimpleMath::Vector3 m_pos{};              // 位置
		DirectX::SimpleMath::Vector3 m_axis[3]{ {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };			 // 方向ベクトル(軸)
		DirectX::SimpleMath::Vector3 m_length{ 20.0f,20.0f,20.0f };          // 各軸方向の長さ(サイズ)
		DirectX::SimpleMath::Vector3 m_scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
		OBB(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& rotation, const DirectX::SimpleMath::Vector3& scale)
			: Base()       // ← 親を明示初期化（必要に応じて引数付きで）
			, m_pos(pos)
			, m_scale(scale)
		{
			DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
			m_axis[0] = DirectX::SimpleMath::Vector3(r._11, r._12, r._13);
			m_axis[1] = DirectX::SimpleMath::Vector3(r._21, r._22, r._23);
			m_axis[2] = DirectX::SimpleMath::Vector3(r._31, r._32, r._33);
		}
		float GetLen(int elem) const {
			switch (elem) {
			case 0:
				return m_scale.x * m_length.x;
			case 1:
				return m_scale.y * m_length.y;
			case 2:
				return m_scale.z * m_length.z;
			default:
				return 0;
			}
			return 0;
		}

		DirectX::SimpleMath::Vector3 GetDirect(int elem) const {
			return m_axis[elem];
		}
	};

	using ColliderVariant = std::variant<Sphere, AABB, OBB>;

	//当たり判定
	bool CheckHit(const Base& a, const Base& b);
	bool CheckHit(const Line& line, const Plane& plane); //線(無限の長さ)と平面(無限の大きさ)
	bool CheckHit(const Segment& segment, const Plane& plane); //線分と平面(無限の大きさ)
	bool CheckHit(const Line& line, const Polygon& polygon); //線(無限の長さ)とポリゴン
	bool CheckHit(const Line& line, const Polygon& polygon, DirectX::SimpleMath::Vector3& contact); //同上
	bool CheckHit(const Segment& segment, const Polygon& polygon); //線分とポリゴン
	bool CheckHit(const Segment& segment, const Polygon& polygon, DirectX::SimpleMath::Vector3& contact); //同上
	bool CheckHit(const Sphere& sphere, const Plane& plane); //球体と平面(無限の大きさ)
	bool CheckHit(const Sphere& sphere, const Polygon& polygon); //球体とポリゴン
	bool CheckHit(const Sphere& sphere, const Polygon& polygon, DirectX::SimpleMath::Vector3& contact); //同上
	//bool CheckHit(const Cylinder& cylinder, const Plane& plane); //円柱と平面
	//bool CheckHit(const Cylinder& cylinder, const Polygon& polygon); //円柱とポリゴン
	//bool CheckHit(const Cylinder& cylinder, const Polygon& polygon, DirectX::SimpleMath::Vector3& contact); //同上
	//bool CheckHit(const Capsule& capsule, const Plane& plane); //カプセルと平面(無限の大きさ)
	//bool CheckHit(const Capsule& capsule, const Polygon& polygon); //カプセルとポリゴン
	//bool CheckHit(const Capsule& capsule, const Polygon& polygon, DirectX::SimpleMath::Vector3& contact); //同上
	bool CheckHit(const Sphere& sphere1, const Sphere& sphere2); //球体と球体
	bool CheckHit(const Sphere& sphere1, const Sphere& sphere2, DirectX::SimpleMath::Vector3& contact); //同上
	bool CheckHit(const AABB& p1, const AABB& p2); // AABBとAABB
	bool CheckHit(const OBB& obb, const Sphere& sphere);//obbと球の当たり判定
	bool CheckHit(const Sphere& sphere, const OBB& obb);
	bool CheckHit(const TestCube& obb, const Sphere& sphere);
	bool CheckHit(const OBB& obb1, const OBB& obb2);//OBB同士の当たり判定

	template<typename T1, typename T2>
	bool CheckHit(const T1&, const T2&) {
		// 未対応の組み合わせはfalse
		return false;
	}

	// variant 版
    inline bool CheckHit(const ColliderVariant& a, const ColliderVariant& b) {
        return std::visit([](auto&& lhs, auto&& rhs) -> bool {
            return Collision::CheckHit(lhs, rhs); // 適切なオーバーロードが呼ばれる
        }, a, b);
    }

	bool CompareLengthOBB(		//OBBの重なりを判定
		const OBB& obb1,		// OBB1
		const OBB& obb2,		// OBB2
		const DirectX::SimpleMath::Vector3& vecseparate,		// 分離軸
		const DirectX::SimpleMath::Vector3& vecdistance);	// 中心座標を結んだベクトル

	//内積・外積
	float Dot(const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2);
	DirectX::SimpleMath::Vector3 Cross(const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2);


	DirectX::SimpleMath::Vector3 ClosestPointOnSegment(const DirectX::SimpleMath::Vector3& point, const Segment& segment);
	float DistanceSquaredPointToSegment(const DirectX::SimpleMath::Vector3& point, const Segment& segment);
	float DistanceSquaredPointToSegment(const DirectX::SimpleMath::Vector3& point, const Segment& segment, DirectX::SimpleMath::Vector3& contact);
	float DistancePointToSegment(const DirectX::SimpleMath::Vector3& point, const Segment& segment);
	float DistancePointToSegment(const DirectX::SimpleMath::Vector3& point, const Segment& segment, DirectX::SimpleMath::Vector3& contact);
	float DistancePointToPlane(const DirectX::SimpleMath::Vector3& point, const Plane& plane);
	DirectX::SimpleMath::Vector3 ProjectPointToPlane(const DirectX::SimpleMath::Vector3& point, const Plane& plane);
	bool PointInTriangle(const DirectX::SimpleMath::Vector3& point, const Polygon& polygon);
	DirectX::SimpleMath::Vector3 ClosestPointOnTriangle(const DirectX::SimpleMath::Vector3& point, const Polygon& polygon);
	DirectX::SimpleMath::Vector3 GetNormal(const Polygon& polygon);


	DirectX::SimpleMath::Vector3 moveSphere(const Segment& capsule, const float& radius, const Polygon& polygon, const DirectX::SimpleMath::Vector3& contact, float& distance);
	DirectX::SimpleMath::Vector3 moveSphere(const Sphere& sphere, const Polygon& polygon, const DirectX::SimpleMath::Vector3& contact);
	float LenOBBtoPoint(const TestCube& obb,const DirectX::SimpleMath::Vector3& point);//OBBと点の長さ
	float LenOBBtoPoint(const OBB& obb,const DirectX::SimpleMath::Vector3& point);//OBBと点の長さ


	//struct Plane {
	//	DirectX::SimpleMath::Vector3 normal; // 平面の法線ベクトル
	//	float d;         // 平面の方程式: ax + by + cz + d = 0 の d
	//};





	// AABBを作る
	AABB SetAABB(DirectX::SimpleMath::Vector3 centerposition, float width, int height, int depth);

/*
	// 線分に平行な与えられた幅の４角形の座標を求める
	void CalcQuadOrientedLine(
		DirectX::SimpleMath::Vector3 startpos,
		DirectX::SimpleMath::Vector3 endpos,
		DirectX::SimpleMath::Vector3* v,
		float width);


	// 幅をもった線分（四角形）と球の当たり判定を行う
	bool CollisionSphereOrientedQuad(
		DirectX::SimpleMath::Vector3 StartPos,						// 開始位置
		DirectX::SimpleMath::Vector3 EndPos,							// 終了位置
		float width,							// 幅
		float radius,							// 半径
		DirectX::SimpleMath::Vector3 CirclePos);						// 中心座標
	*/





}