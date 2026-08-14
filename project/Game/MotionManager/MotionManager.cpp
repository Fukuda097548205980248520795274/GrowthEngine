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
		instance_->Initialize();
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
	jointMap_[JointType::HandL] = "mixamorig:LeftHand";
	jointMap_[JointType::HandR] = "mixamorig:RightHand";
	jointMap_[JointType::ArmL] = "mixamorig:LeftArm";
	jointMap_[JointType::ArmR] = "mixamorig:RightArm";
	jointMap_[JointType::FootL] = "mixamorig:LeftFoot";
	jointMap_[JointType::FootR] = "mixamorig:RightFoot";
	jointMap_[JointType::LegL] = "mixamorig:LeftLeg";
	jointMap_[JointType::LegR] = "mixamorig:RightLeg";
	jointMap_[JointType::ToeEndL] = "mixamorig:LeftToe_End";
	jointMap_[JointType::ToeEndR] = "mixamorig:RightToe_End";
	jointMap_[JointType::ToeBaseL] = "mixamorig:LeftToeBase";
	jointMap_[JointType::ToeBaseR] = "mixamorig:RightToeBase";
	jointMap_[JointType::Chest] = "mixamorig:Spine2";
	jointMap_[JointType::Head] = "mixamorig:Head";
	jointMap_[JointType::Root] = "mixamorig:Spine";
	jointMap_[JointType::UpLegL] = "mixamorig:LeftUpLeg";
	jointMap_[JointType::UpLegR] = "mixamorig:RightUpLeg";

	parentMap_[JointType::HandL] = JointType::ArmL;
	parentMap_[JointType::HandR] = JointType::ArmR;
	parentMap_[JointType::FootL] = JointType::LegL;
	parentMap_[JointType::FootR] = JointType::LegR;
	parentMap_[JointType::ToeEndL] = JointType::ToeBaseL;
	parentMap_[JointType::ToeEndR] = JointType::ToeBaseR;
	parentMap_[JointType::ToeBaseL] = JointType::FootL;
	parentMap_[JointType::ToeBaseR] = JointType::FootR;
	parentMap_[JointType::LegL] = JointType::UpLegL;
	parentMap_[JointType::LegR] = JointType::UpLegR;

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
	LoadStyleChange(dir);
	LoadGrabStrikeAttacker(dir);
	LoadGrabStrikeTarget(dir);
	LoadTelegraph(dir);

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
	table_[MotionType::Stance]["Senran"] = engine_->LoadAnimation(dir + "/stance/stance_Senran", "stance_Senran.gltf");
	table_[MotionType::Stance]["Gekitetu"] = engine_->LoadAnimation(dir + "/stance/stance_Gekitetu", "stance_Gekitetu.gltf");
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
	table_[MotionType::Attack]["Kick_000"] = engine_->LoadAnimation(dir + "/attack/Kick_000", "Kick_000.gltf");
	table_[MotionType::Attack]["Kick_001"] = engine_->LoadAnimation(dir + "/attack/Kick_001", "Kick_001.gltf");
	table_[MotionType::Attack]["Kick_002"] = engine_->LoadAnimation(dir + "/attack/Kick_002", "Kick_002.gltf");
	table_[MotionType::Attack]["Kick_003"] = engine_->LoadAnimation(dir + "/attack/Kick_003", "Kick_003.gltf");
	table_[MotionType::Attack]["Kick_004"] = engine_->LoadAnimation(dir + "/attack/Kick_004", "Kick_004.gltf");
	table_[MotionType::Attack]["Kick_005"] = engine_->LoadAnimation(dir + "/attack/Kick_005", "Kick_005.gltf");
	table_[MotionType::Attack]["Kick_006"] = engine_->LoadAnimation(dir + "/attack/Kick_006", "Kick_006.gltf");
	table_[MotionType::Attack]["Back_Flip_000"] = engine_->LoadAnimation(dir + "/attack/Back_Flip_000", "Back_Flip_000.gltf");
	table_[MotionType::Attack]["MMA_Kick_000"] = engine_->LoadAnimation(dir + "/attack/MMA_Kick_000", "MMA_Kick_000.gltf");
	table_[MotionType::Attack]["MMA_Kick_001"] = engine_->LoadAnimation(dir + "/attack/MMA_Kick_001", "MMA_Kick_001.gltf");
	table_[MotionType::Attack]["MMA_Kick_002"] = engine_->LoadAnimation(dir + "/attack/MMA_Kick_002", "MMA_Kick_002.gltf");
	table_[MotionType::Attack]["Hook_000"] = engine_->LoadAnimation(dir + "/attack/Hook_000", "Hook_000.gltf");
	table_[MotionType::Attack]["Hook_001"] = engine_->LoadAnimation(dir + "/attack/Hook_001", "Hook_001.gltf");
	table_[MotionType::Attack]["Jab_000"] = engine_->LoadAnimation(dir + "/attack/Jab_000", "Jab_000.gltf");
	table_[MotionType::Attack]["Headbutt_000"] = engine_->LoadAnimation(dir + "/attack/Headbutt_000", "Headbutt_000.gltf");
	table_[MotionType::Attack]["Drop_Kick_000"] = engine_->LoadAnimation(dir + "/attack/Drop_Kick_000", "Drop_Kick_000.gltf");
	table_[MotionType::Attack]["Zombie_Punch_000"] = engine_->LoadAnimation(dir + "/attack/Zombie_Punch_000", "Zombie_Punch_000.gltf");
	table_[MotionType::Attack]["Zombie_Punch_001"] = engine_->LoadAnimation(dir + "/attack/Zombie_Punch_001", "Zombie_Punch_001.gltf");
	table_[MotionType::Attack]["Zombie_Punch_002"] = engine_->LoadAnimation(dir + "/attack/Zombie_Punch_002", "Zombie_Punch_002.gltf");
	table_[MotionType::Attack]["Zombie_Kick_000"] = engine_->LoadAnimation(dir + "/attack/Zombie_Kick_000", "Zombie_Kick_000.gltf");
	table_[MotionType::Attack]["Zombie_Kick_001"] = engine_->LoadAnimation(dir + "/attack/Zombie_Kick_001", "Zombie_Kick_001.gltf");
	table_[MotionType::Attack]["Flip_Kick_000"] = engine_->LoadAnimation(dir + "/attack/Flip_Kick_000", "Flip_Kick_000.gltf");
	table_[MotionType::Attack]["Flip_Kick_001"] = engine_->LoadAnimation(dir + "/attack/Flip_Kick_001", "Flip_Kick_001.gltf");
	table_[MotionType::Attack]["Blow_Weapon_Attack_000"] = engine_->LoadAnimation(dir + "/attack/Blow_Weapon_Attack_000", "Blow_Weapon_Attack_000.gltf");
}

/// @brief 怯みモーション読み込み
/// @param dir 
void MotionManager::LoadStagger(const std::string& dir)
{
	table_[MotionType::Stagger]["Front"] = engine_->LoadAnimation(dir + "/stagger/stagger_000", "stagger_000.gltf");
	table_[MotionType::Stagger]["Back"] = engine_->LoadAnimation(dir + "/stagger/stagger_001", "stagger_001.gltf");
	table_[MotionType::Stagger]["Front_Down"] = engine_->LoadAnimation(dir + "/stagger/stagger_002", "stagger_002.gltf");
	table_[MotionType::Stagger]["Front_Middle"] = engine_->LoadAnimation(dir + "/stagger/stagger_003", "stagger_003.gltf");
	table_[MotionType::Stagger]["Front_Heavy"] = engine_->LoadAnimation(dir + "/stagger/stagger_004", "stagger_004.gltf");
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

/// @brief スタイルチェンジモーション読み込み
/// @param dir 
void MotionManager::LoadStyleChange(const std::string& dir)
{
	table_[MotionType::StyleChange]["Senran"] = engine_->LoadAnimation(dir + "/styleChange/styleChange_Senran", "styleChange_Senran.gltf");
	table_[MotionType::StyleChange]["Gekitetu"] = engine_->LoadAnimation(dir + "/styleChange/styleChange_Gekitetu", "styleChange_Gekitetu.gltf");
}

/// @brief 掴み攻撃（攻撃側）モーション読み込み
/// @param dir 
void MotionManager::LoadGrabStrikeAttacker(const std::string& dir)
{
	table_[MotionType::GrabStrikeAttacker]["000"] = engine_->LoadAnimation(dir + "/grabStrike/grabStrike_00/attacker", "attacker.gltf");
}

/// @brief 掴み攻撃（被攻撃側）モーション読み込み
/// @param dir 
void MotionManager::LoadGrabStrikeTarget(const std::string& dir)
{
	table_[MotionType::GrabStrikeTarget]["000"] = engine_->LoadAnimation(dir + "/grabStrike/grabStrike_00/target", "target.gltf");
}

/// @brief 予備動作モーション読み込み
/// @param dir 
void MotionManager::LoadTelegraph(const std::string& dir)
{
	table_[MotionType::Telegraph]["000"] = engine_->LoadAnimation(dir + "/telegraph/telegraph_000", "telegraph_000.gltf");
}