#include "Manager.h"
// シングルトンインスタンス生成
template <class T>
T& Manager<T>::GetInstance()
{
	static T* instance = nullptr; // 静的ポインタ変数としてシングルトンインスタンスを保持
	if (!instance)
	{
		instance = new T(); // インスタンスがなければ新しく生成

	}
	return *instance; // インスタンスへの参照を返す
}