#pragma once
#include "GrowthEngine.h"

class TitleScene : public Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	TitleScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	// カプセルコリジョン
	std::unique_ptr<Collision3DBaseCapsule> capsule_ = nullptr;
	Collision3DInstanceCapsule* capsuleInstance_ = nullptr;

	std::unique_ptr<Collision3DBaseAABB> aabb_ = nullptr;
	Collision3DInstanceAABB* aabbInstance_ = nullptr;
};

