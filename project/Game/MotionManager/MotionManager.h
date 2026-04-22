#pragma once
#include "GrowthEngine.h"

/// @brief モーションの種類
enum class MotionType
{
	Stand,
	Stance
};

class MotionManager
{
public:

	/// @brief デストラクタ
	~MotionManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static MotionManager* GetInstance();

	/// @brief 初期化処理
	void Initialize();

	/// @brief モーションを取得する
	/// @param type 
	/// @param index
	/// @return 
	AnimationHandle GetMotion(MotionType type, int32_t index) { return table_[type][index]; }

	/// @brief スケルトンを取得する
	/// @return 
	SkeletonHandle GetSkeleton() { return hSkeleton_; }


private:

	/// @brief 生成させない
	MotionManager() = default;
	MotionManager(const MotionManager&) = delete;
	MotionManager& operator=(const MotionManager&) = delete;

private:

	/// @brief 立ちモーション読み込み
	/// @param dir
	void LoadStand(const std::string& dir);

	/// @brief 構えモーション読み込み
	/// @param dir 
	void LoadStance(const std::string& dir);


private:

	/// @brief エンジン
	const GrowthEngine* engine_;

	/// @brief インスタンス
	static std::unique_ptr<MotionManager> instance_;

	/// @brief スケルトンのハンドル
	SkeletonHandle hSkeleton_;

	/// @brief テーブル
	using MotionTable = std::unordered_map<MotionType, std::unordered_map<int32_t, AnimationHandle>>;
	MotionTable table_;

	/// @brief 初期化したかどうか
	bool isInit_ = false;
};

