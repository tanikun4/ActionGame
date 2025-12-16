#include    "main.h"
#include    "Application.h"

//=======================================
//エントリーポイント
//=======================================
//int main(void)
//{
//#if defined(DEBUG) || defined(_DEBUG)
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif//defined(DEBUG) || defined(_DEBUG)
//
//    //std::cout << "あああ" << endl;
//
//    // アプリケーション実行
//    Application app(SCREEN_WIDTH,SCREEN_HEIGHT);
//    app.Run();
//
//    return 0;
//}


#include <windows.h>
#include <crtdbg.h>

//=======================================
// エントリーポイント（WinMain）
//=======================================
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPSTR,
    int nCmdShow
)
{
#if defined(DEBUG) || defined(_DEBUG)
    // メモリリーク検出
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // コンソール生成
    AllocConsole();

    // printf / cout をコンソールに接続
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    // アプリケーション実行
    Application app(SCREEN_WIDTH, SCREEN_HEIGHT);
    app.Run();

#if defined(DEBUG) || defined(_DEBUG)
    FreeConsole();
#endif

    return 0;
}
