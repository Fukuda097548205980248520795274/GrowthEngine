#include "GrowthEngine.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	// エンジンの生成と取得
	GrowthEngine* engine = GrowthEngine::GetInstance(1280,720,"LE2A_12_フクダ_ソウワ");

	while (engine->GameLoop())
	{

	}

	// エンジンの終了処理
	engine->Finalize();

	return 0;
}