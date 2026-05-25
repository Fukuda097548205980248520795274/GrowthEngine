#pragma once
#include "GrowthEngine.h"

#include "Effect/GuardEffect/GuardEffect.h"

class EffectManager
{
public:

	/// @brief デストラクタ
	~EffectManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static EffectManager* GetInstance();

	/// @brief 更新処理
	void Update();

	/// @brief 描画処理
	void Draw();

	/// @brief ガードエフェクトを生成する
	/// @param position 
	/// @param rotate 
	void CreateGuardEffect(const Vector3& position, const Vector3& rotate);


private:

	/// @brief 初期化処理
	void Initialize();

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();


private:

	/// @brief インスタンス
	static std::unique_ptr<EffectManager> instance_;

	// コンストラクタと代入演算子を削除
	EffectManager() = default;
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;



private:

	/// @brief ガードエフェクトのモデル
	std::unique_ptr<PrefabBaseTube> guardEffectModel_ = nullptr;

	/// @brief ガードエフェクト
	std::list<std::unique_ptr<GuardEffect>> guardEffects_;
};

