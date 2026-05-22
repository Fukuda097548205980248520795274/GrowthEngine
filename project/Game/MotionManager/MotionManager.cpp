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
	jointMap_[JointType::HandL] = "ボーン.007";
	jointMap_[JointType::HandR] = "ボーン.015";
	jointMap_[JointType::FootL] = "ボーン.011";
	jointMap_[JointType::FootR] = "ボーン.019";
	jointMap_[JointType::Chest] = "ボーン.001";
	jointMap_[JointType::Head] = "ボーン.003";
	jointMap_[JointType::Root] = "ボーン";

	// モーション読み込み
	LoadStand(dir);
	LoadStance(dir);
	LoadWalk(dir);
	LoadDash(dir);
	LoadAttack(dir);
	LoadAvoid(dir);
	LoadAttack(dir);
	LoadStagger(dir);
	LoadGrab(dir);
	LoadGrabbed(dir);
	LoadDownFall(dir);
	LoadDownLying(dir);
	LoadDownGetUp(dir);
	LoadGuard(dir);

	// 初期化完了
	isInit_ = true;
}

/// @brief ジョイント名を取得する
/// @param partName 
/// @return 
std::string MotionManager::GetJointName(const JointType& jointType) const
{
	auto it = jointMap_.find(jointType);
	if (it != jointMap_.end())
	{
		return it->second;
	}

	// ここに来るのは想定外
	assert(false);
	return "Error";
}

/// @brief モーション名のリストを取得する
/// @param type 
/// @return 
std::vector<std::string> MotionManager::GetMotionNames(MotionType type) const
{
	std::vector<std::string> names;
	auto it = table_.find(type);
	if (it != table_.end()) {
		for (const auto& pair : it->second) {
			names.push_back(pair.first);
		}
	}
	return names;
}


/// @brief 立ちモーション読み込み
void MotionManager::LoadStand(const std::string& dir)
{
	table_[MotionType::Stand]["Standing"] = engine_->LoadAnimation(dir + "/standing", "standing.gltf");
}

/// @brief 構えモーション読み込み
/// @param dir 
void MotionManager::LoadStance(const std::string& dir)
{
	table_[MotionType::Stance]["Standard"] = engine_->LoadAnimation(dir + "/stance/stance_000", "stance_000.gltf");
	table_[MotionType::Stance]["Fighter"] = engine_->LoadAnimation(dir + "/stance/stance_001", "stance_001.gltf");
	table_[MotionType::Stance]["Gang"] = engine_->LoadAnimation(dir + "/stance/stance_002", "stance_002.gltf");
	table_[MotionType::Stance]["WeekMan"] = engine_->LoadAnimation(dir + "/stance/stance_003", "stance_003.gltf");
	table_[MotionType::Stance]["Nimble"] = engine_->LoadAnimation(dir + "/stance/stance_004", "stance_004.gltf");
}

/// @brief 歩きモーション読み込み
/// @param dir 
void MotionManager::LoadWalk(const std::string& dir)
{
	table_[MotionType::Walk]["Walk"] = engine_->LoadAnimation(dir + "/walk", "walk.gltf");
}

/// @brief ダッシュモーション読み込み
/// @param dir 
void MotionManager::LoadDash(const std::string& dir)
{
	table_[MotionType::Dash]["Dash"] = engine_->LoadAnimation(dir + "/dash", "dash.gltf");
}

/// @brief 回避モーション
/// @param dir 
void MotionManager::LoadAvoid(const std::string& dir)
{
	table_[MotionType::Avoid]["Front"] = engine_->LoadAnimation(dir + "/avoidFront/avoidFront_000", "avoidFront_000.gltf");
	table_[MotionType::Avoid]["Back"] = engine_->LoadAnimation(dir + "/avoidBack/avoidBack_000", "avoidBack_000.gltf");
}

/// @brief 攻撃モーション読み込み
/// @param dir 
void MotionManager::LoadAttack(const std::string& dir)
{
	table_[MotionType::Attack]["Player_Combo_1"] = engine_->LoadAnimation(dir + "/attack/attack_000", "attack_000.gltf");
	table_[MotionType::Attack]["Player_Combo_2"] = engine_->LoadAnimation(dir + "/attack/attack_001", "attack_001.gltf");
	table_[MotionType::Attack]["Player_Combo_3"] = engine_->LoadAnimation(dir + "/attack/attack_002", "attack_002.gltf");
	table_[MotionType::Attack]["Player_Combo_4"] = engine_->LoadAnimation(dir + "/attack/attack_003", "attack_003.gltf");
}

/// @brief 怯みモーション読み込み
/// @param dir 
void MotionManager::LoadStagger(const std::string& dir)
{
	table_[MotionType::Stagger]["Front"] = engine_->LoadAnimation(dir + "/stagger/stagger_000", "stagger_000.gltf");
	table_[MotionType::Stagger]["Back"] = engine_->LoadAnimation(dir + "/stagger/stagger_001", "stagger_001.gltf");
	table_[MotionType::Stagger]["Front_Down"] = engine_->LoadAnimation(dir + "/stagger/stagger_002", "stagger_002.gltf");
}

/// @brief 掴みモーション読み込み
/// @param dir 
void MotionManager::LoadGrab(const std::string& dir)
{
	table_[MotionType::Grab]["Front"] = engine_->LoadAnimation(dir + "/grab/grab_000", "grab_000.gltf");
}

/// @brief 掴まれモーション読み込み
/// @param dir 
void MotionManager::LoadGrabbed(const std::string& dir)
{
	table_[MotionType::Grabbed]["Front"] = engine_->LoadAnimation(dir + "/grabbed/grabbed_000", "grabbed_000.gltf");
}

/// @brief ダウンモーション読み込み
/// @param dir 
void MotionManager::LoadDownFall(const std::string& dir)
{
	table_[MotionType::DownFall]["Front"] = engine_->LoadAnimation(dir + "/downFall/downFall_000", "downFall_000.gltf");
}

/// @brief ダウン中モーション読み込み
/// @param dir 
void MotionManager::LoadDownLying(const std::string& dir)
{
	table_[MotionType::DownLying]["Front"] = engine_->LoadAnimation(dir + "/downLying/downLying_000", "downLying_000.gltf");
}

/// @brief ダウンから起き上がるモーション読み込み
/// @param dir 
void MotionManager::LoadDownGetUp(const std::string& dir)
{
	table_[MotionType::DowoGetUp]["Front"] = engine_->LoadAnimation(dir + "/downGetUp/downGetUp_000", "downGetUp_000.gltf");
}

/// @brief 防御モーション読み込み
/// @param dir
void MotionManager::LoadGuard(const std::string& dir)
{
	table_[MotionType::Guard]["BothHands"] = engine_->LoadAnimation(dir + "/guard/guard_000", "guard_000.gltf");
	table_[MotionType::Guard]["OneLeg"] = engine_->LoadAnimation(dir + "/guard/guard_001", "guard_001.gltf");
}