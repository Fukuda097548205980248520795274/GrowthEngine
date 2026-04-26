#include "MotionManager.h"

/// インスタンスの初期化
std::unique_ptr<MotionManager> MotionManager::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @return 
MotionManager* MotionManager::GetInstance()
{
	// インスタンスがなければ生成する
	if(instance_ == nullptr)
	{
		instance_.reset(new MotionManager());
	}

	return instance_.get();
}

/// @brief 初期化処理
void MotionManager::Initialize()
{
	// 初期化済みなら何もしない
	if (isInit_)return;


	// エンジンを取得する
	engine_ = GrowthEngine::GetInstance();

	// ディレクトリ
	std::string dir = "Assets/Models/Character";

	// スケルトンを読み込む
	hSkeleton_ = engine_->LoadSkeleton(dir , "bone.gltf");

	// ジョイントマップを作成する
	jointMap_["LeftHand"] = "ボーン.007";
	jointMap_["RightHand"] = "ボーン.015";
	jointMap_["LeftFoot"] = "ボーン.011";
	jointMap_["RightFoot"] = "ボーン.019";

	// モーション読み込み
	LoadStand(dir);
	LoadStance(dir);
	LoadWalk(dir);
	LoadDash(dir);
	LoadAttack(dir);
	LoadAvoidFront(dir);
	LoadAvoidBack(dir);
	LoadAvoidLeft(dir);
	LoadAvoidRight(dir);
	LoadAttack(dir);
	LoadStagger(dir);
	LoadGrab(dir);
	LoadGrabbed(dir);

	// 初期化完了
	isInit_ = true;
}

/// @brief ジョイント名を取得する
/// @param partName 
/// @return 
std::string MotionManager::GetJointName(const std::string& partName) const
{
	auto it = jointMap_.find(partName);
	if (it != jointMap_.end())
	{
		return it->second;
	}

	// ここに来るのは想定外
	assert(false);
	return partName;
}

/// @brief 立ちモーション読み込み
void MotionManager::LoadStand(const std::string& dir)
{
	table_[MotionType::Stand][0] = engine_->LoadAnimation(dir + "/standing", "standing.gltf");
}

/// @brief 構えモーション読み込み
/// @param dir 
void MotionManager::LoadStance(const std::string& dir)
{
	table_[MotionType::Stance][0] = engine_->LoadAnimation(dir + "/stance/stance_000", "stance_000.gltf");
	table_[MotionType::Stance][1] = engine_->LoadAnimation(dir + "/stance/stance_001", "stance_001.gltf");
	table_[MotionType::Stance][2] = engine_->LoadAnimation(dir + "/stance/stance_002", "stance_002.gltf");
	table_[MotionType::Stance][3] = engine_->LoadAnimation(dir + "/stance/stance_003", "stance_003.gltf");
	table_[MotionType::Stance][4] = engine_->LoadAnimation(dir + "/stance/stance_004", "stance_004.gltf");
}

/// @brief 歩きモーション読み込み
/// @param dir 
void MotionManager::LoadWalk(const std::string& dir)
{
	table_[MotionType::Walk][0] = engine_->LoadAnimation(dir + "/walk", "walk.gltf");
}

/// @brief ダッシュモーション読み込み
/// @param dir 
void MotionManager::LoadDash(const std::string& dir)
{
	table_[MotionType::Dash][0] = engine_->LoadAnimation(dir + "/dash", "dash.gltf");
}

/// @brief 前回避モーション
/// @param dir 
void MotionManager::LoadAvoidFront(const std::string& dir)
{
	table_[MotionType::AvoidFont][0] = engine_->LoadAnimation(dir + "/avoidFront/avoidFront_000", "avoidFront_000.gltf");
}

/// @brief 後ろ回避モーション
/// @param dir 
void MotionManager::LoadAvoidBack(const std::string& dir)
{
	table_[MotionType::AvoidBack][0] = engine_->LoadAnimation(dir + "/avoidBack/avoidBack_000", "avoidBack_000.gltf");
}

/// @brief 左回避モーション
/// @param dir 
void MotionManager::LoadAvoidLeft(const std::string& dir)
{

}

/// @brief 右回避モーション
/// @param dir 
void MotionManager::LoadAvoidRight(const std::string& dir)
{

}

/// @brief 攻撃モーション読み込み
/// @param dir 
void MotionManager::LoadAttack(const std::string& dir)
{
	table_[MotionType::Attack][0] = engine_->LoadAnimation(dir + "/attack/attack_000", "attack_000.gltf");
	table_[MotionType::Attack][1] = engine_->LoadAnimation(dir + "/attack/attack_001", "attack_001.gltf");
	table_[MotionType::Attack][2] = engine_->LoadAnimation(dir + "/attack/attack_002", "attack_002.gltf");
	table_[MotionType::Attack][3] = engine_->LoadAnimation(dir + "/attack/attack_003", "attack_003.gltf");
}

/// @brief 怯みモーション読み込み
/// @param dir 
void MotionManager::LoadStagger(const std::string& dir)
{
	table_[MotionType::Stagger][0] = engine_->LoadAnimation(dir + "/stagger/stagger_000", "stagger_000.gltf");
	table_[MotionType::Stagger][1] = engine_->LoadAnimation(dir + "/stagger/stagger_001", "stagger_001.gltf");
}

/// @brief 掴みモーション読み込み
/// @param dir 
void MotionManager::LoadGrab(const std::string& dir)
{
	table_[MotionType::Grab][0] = engine_->LoadAnimation(dir + "/grab/grab_000", "grab_000.gltf");
}

/// @brief 掴まれモーション読み込み
/// @param dir 
void MotionManager::LoadGrabbed(const std::string& dir)
{
	table_[MotionType::Grabbed][0] = engine_->LoadAnimation(dir + "/grabbed/grabbed_000", "grabbed_000.gltf");
}