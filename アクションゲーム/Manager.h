#pragma once

template <class T>
class Manager
{
public:
	static T& GetInstance();	// シングルトンインスタンスへの参照を取得
protected:
	Manager(){}		// コンストラクタ(外部からのインスタンス化を禁止)
	~Manager(){}	// デストラクタ(外部からの削除を防止)
private:
	Manager(const Manager&) = delete;	//	コピーコンストラクタを禁止(= deleteが使われると、ビルドエラーになる)
	Manager& operator = (const Manager&) = delete;	//	代入演算子を禁止
};

