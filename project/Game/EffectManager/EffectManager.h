#pragma once
#include "GrowthEngine.h"

#include "Effect/GuardEffect/GuardEffect.h"
#include "Effect/RepelEffect/RepelEffect.h"

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

	/// @brief 弾きエフェクトを生成する
	/// @param position 
	void CreateRepelEffect(const Vector3& position);

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

	/// @brief インパクトを放出する
	/// @param position 
	void Impact004(const Vector3& position);

	/// @brief インパクトを放出する
	/// @param position 
	void Impact005(const Vector3& position);

	/// @brief インパクトドロップを放出する
	/// @param position 
	void ImpactDrop000(const Vector3& position);

	/// @brief インパクトスモークを放出する
	/// @param position 
	void ImpactSmoke000(const Vector3& position);

	/// @brief インパクトスモークを放出する
	/// @param position 
	void ImpactSmoke001(const Vector3& position);

	/// @brief 吹っ飛びスモークを放出する
	/// @param position 
	void BlownSmoke000(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround000(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround001(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround002(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround003(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround004(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround005(const Vector3& position);

	/// @brief インパクト地面を放出する
	/// @param position 
	void ImpactGround006(const Vector3& position);

	/// @brief 予備動作エフェクトを放出する
	/// @param position 
	void TelegraphEffect000(const Vector3& position);

	/// @brief 走りスモークを放出する
	/// @param position 
	void DashSmoke000(const Vector3& position);

	/// @brief 掴みインパクトを放出する
	/// @param position 
	void GrabImpact000(const Vector3& position);

	/// @brief 攻撃インパクトを放出する
	/// @param position 
	void AttackImpact000(const Vector3& position);

	/// @brief レイジインパクトを放出する
	/// @param position 
	void RageImpact000(const Vector3& position);

	/// @brief レイジインパクトを放出する
	/// @param position 
	void RageImpact001(const Vector3& position);

	/// @brief レイジインパクトを放出する
	/// @param position 
	void RageImpact002(const Vector3& position);


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

	/// @brief 弾きエフェクトのモデル
	std::unique_ptr<PrefabBaseTube> repelEffectModel_ = nullptr;

	/// @brief 弾きエフェクト
	std::list<std::unique_ptr<RepelEffect>> repelEffects_;


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

	/// @brief インパクト004
	std::unique_ptr<Particle3D> impact004_ = nullptr;

	/// @brief インパクト005
	std::unique_ptr<Particle3D> impact005_ = nullptr;


private:

	// インパクトドロップ000
	std::unique_ptr<Particle3D> impactDrop000_ = nullptr;


private:

	/// @brief インパクトスモーク000
	std::unique_ptr<Particle3D> impactSmoke000_ = nullptr;

	/// @brief インパクトスモーク001
	std::unique_ptr<Particle3D> impactSmoke001_ = nullptr;


private:

	/// @brief 吹っ飛びスモーク000
	std::unique_ptr<Particle3D> blownSmoke000_ = nullptr;


private:

	/// @brief インパクト地面000
	std::unique_ptr<Particle3D> impactGround000_ = nullptr;

	/// @brief インパクト地面001
	std::unique_ptr<Particle3D> impactGround001_ = nullptr;

	/// @brief インパクト地面002
	std::unique_ptr<Particle3D> impactGround002_ = nullptr;
	
	/// @brief インパクト地面003
	std::unique_ptr<Particle3D> impactGround003_ = nullptr;

	/// @brief インパクト地面004
	std::unique_ptr<Particle3D> impactGround004_ = nullptr;

	/// @brief インパクト地面005
	std::unique_ptr<Particle3D> impactGround005_ = nullptr;
	
	/// @brief インパクト地面006
	std::unique_ptr<Particle3D> impactGround006_ = nullptr;


private:

	/// @brief 予備動作エフェクト000
	std::unique_ptr<Particle3D> telegraphEffect000_ = nullptr;


private:

	/// @brief 走りスモーク000
	std::unique_ptr<Particle3D> dashSmoke000_ = nullptr;


private:

	/// @brief 掴みインパクト000
	std::unique_ptr<Particle3D> grabImpact000_ = nullptr;


private:

	/// @brief 攻撃インパクト000
	std::unique_ptr<Particle3D> attackImpact000_ = nullptr;


private:

	/// @brief レイジインパクト000パーティクル
	std::unique_ptr<Particle3D> rageImpact000_ = nullptr;

	/// @brief レイジインパクト001パーティクル
	std::unique_ptr<Particle3D> rageImpact001_ = nullptr;

	/// @brief レイジインパクト002パーティクル
	std::unique_ptr<Particle3D> rageImpact002_ = nullptr;

};

