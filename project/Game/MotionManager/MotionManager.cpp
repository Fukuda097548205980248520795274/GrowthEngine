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

	// モーション読み込み
	LoadStand(dir);
	LoadStance(dir);

	// 初期化完了
	isInit_ = true;
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
	table_[MotionType::Stance][1] = engine_->LoadAnimation(dir + "/stance/stance_001", "stance_001.gltf");
	table_[MotionType::Stance][2] = engine_->LoadAnimation(dir + "/stance/stance_002", "stance_002.gltf");
	table_[MotionType::Stance][3] = engine_->LoadAnimation(dir + "/stance/stance_003", "stance_003.gltf");
	table_[MotionType::Stance][4] = engine_->LoadAnimation(dir + "/stance/stance_004", "stance_004.gltf");
}