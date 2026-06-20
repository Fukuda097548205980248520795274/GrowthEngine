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

	/// @brief スパークを放出する
	/// @param position 
	void EmitSpark000(const Vector3& position);

	/// @brief インパクトを放出する
	/// @param position 
	/// @param rotate 
	void Impact000(const Vector3& position, const Vector3& rotate);

	/// @brief インパクトを放出する
	/// @param position 
	void Impact001(const Vector3& position);

	/// @brief インパクトを放出する
	/// @param position 
	void Impact002(const Vector3& position);

	/// @brief インパクトを放出する
	/// @param position 
	void Impact003(const Vector3& position);

	/// @brief インパクトドロップを放出する
	/// @param position 
	void ImpactDrop000(const Vector3& position);

	/// @brief インパクトスモークを放出する
	/// @param position 
	void ImpactSmoke000(const Vector3& position);

	/// @brief インパクトスモークを放出する
	/// @param position 
	void ImpactSmoke001(const Vector3& position);


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


private:

	// スパークパーティクル000
	std::unique_ptr<Particle3D> spark000_ = nullptr;


private:

	/// @brief インパクト000
	std::unique_ptr<Particle3D> impact000_ = nullptr;

	/// @brief インパクト001
	std::unique_ptr<Particle3D> impact001_ = nullptr;

	/// @brief インパクト002
	std::unique_ptr<Particle3D> impact002_ = nullptr;

	/// @brief インパクト003
	std::unique_ptr<Particle3D> impact003_ = nullptr;


private:

	// インパクトドロップ000
	std::unique_ptr<Particle3D> impactDrop000_ = nullptr;


private:

	/// @brief インパクトスモーク000
	std::unique_ptr<Particle3D> impactSmoke000_ = nullptr;

	/// @brief インパクトスモーク001
	std::unique_ptr<Particle3D> impactSmoke001_ = nullptr;
};

