#pragma once
#include "GrowthEngine.h"
#include "Application/Camera/Camera3D/Camera3D.h"

class Scene
{
public:

	/// @brief コンストラクタ
	Scene();

	/// @brief 仮想デストラクタ
	virtual ~Scene() = default;


	/// @brief 初期化
	virtual void Initialize();

	/// @brief 更新処理
	virtual void Update();

	/// @brief 描画処理
	virtual void Draw();


protected:

	// 3Dカメラ
	std::unique_ptr<Camera3D> camera3d_ = nullptr;
};

