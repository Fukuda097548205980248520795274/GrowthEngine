#pragma once
#include "GrowthEngine.h"

class Framework
{
public:

	/// @brief コンストラクタ
	virtual ~Framework() = default;

	/// @brief コンストラクタ
	/// @param screenWidth スクリーン横幅
	/// @param screenHeight スクリーン縦幅
	/// @param title タイトル
	Framework(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief 実行
	/// @return 
	int32_t Run();


protected:

	/// @brief 初期化
	virtual void Initialize() = 0;

	/// @brief 更新処理
	virtual void Update() = 0;

	/// @brief 描画処理
	virtual void Draw() = 0;

};

