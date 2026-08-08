#pragma once
#include "GrowthEngine.h"

/// @brief モーションの種類
enum class MotionType
{
	Stand,
	Stance,
	Walk,
	Dash,
	Attack,
	Avoid,
	Stagger,
	Grab,
	Grabbed,
	DownFall,
	DownLying,
	DowoGetUp,
	Guard,
	StyleChange,
	GrabStrikeAttacker,
	GrabStrikeTarget,
	Telegraph
};

enum class JointType
{
	None,
	Root,
	Spine,
	Chest,
	Neck,
	Head,
	ArmL,
	ArmR,
	HandL,
	HandR,
	LegL,
	LegR,
	FootL,
	FootR,
	ToeEndL,
	ToeEndR,
	ToeBaseL,
	ToeBaseR,
	UpLegL,
	UpLegR,
	Weapon,
};

inline const char* jointTypeNames[] =
{
	"None",
	"Root",
	"Spine",
	"Chest",
	"Neck",
	"Head",
	"ArmL",
	"ArmR",
	"HandL",
	"HandR",
	"LegL",
	"LegR",
	"FootL",
	"FootR",
	"ToeEndL",
	"ToeEndR",
	"ToeBaseL",
	"ToeBaseR",
	"UpLegL",
	"UpLegR",
	"Weapon" 
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
	AnimationHandle GetMotion(MotionType type, const std::string& name) { return table_[type][name]; }

	/// @brief ジョイントの親を取得する
	/// @param jointType 
	/// @return 
	JointType GetParentJoint(const JointType& jointType) const { return parentMap_.at(jointType); }

	/// @brief スケルトンを取得する
	/// @return 
	SkeletonHandle GetSkeleton() { return hSkeleton_; }

	/// @brief ジョイント名を取得する
	/// @param partName 
	/// @return 
	std::string GetJointName(const JointType& jointType) const;

	/// @brief モーション名のリストを取得する
	/// @param type 
	/// @return 
	std::vector<std::string> GetMotionNames(MotionType type) const;

	/// @brief モーションテーブルを取得する
	/// @return 
	const auto& GetMotionTable() const { return table_; }


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

	/// @brief 歩きモーション読み込み
	/// @param dir 
	void LoadWalk(const std::string& dir);

	/// @brief ダッシュモーション読み込み
	/// @param dir 
	void LoadDash(const std::string& dir);

	/// @brief 回避モーション
	/// @param dir 
	void LoadAvoid(const std::string& dir);

	/// @brief 攻撃モーション読み込み
	/// @param dir 
	void LoadAttack(const std::string& dir);

	/// @brief 怯みモーション読み込み
	/// @param dir 
	void LoadStagger(const std::string& dir);

	/// @brief 掴みモーション読み込み
	/// @param dir 
	void LoadGrab(const std::string& dir);

	/// @brief 掴まれモーション読み込み
	/// @param dir 
	void LoadGrabbed(const std::string& dir);

	/// @brief ダウンモーション読み込み
	/// @param dir 
	void LoadDownFall(const std::string& dir);

	/// @brief ダウン中モーション読み込み
	/// @param dir 
	void LoadDownLying(const std::string& dir);

	/// @brief ダウンから起き上がるモーション読み込み
	/// @param dir 
	void LoadDownGetUp(const std::string& dir);

	/// @brief 防御モーション読み込み
	/// @param dir
	void LoadGuard(const std::string& dir);

	/// @brief スタイルチェンジモーション読み込み
	/// @param dir 
	void LoadStyleChange(const std::string& dir);

	/// @brief 掴み攻撃（攻撃側）モーション読み込み
	/// @param dir 
	void LoadGrabStrikeAttacker(const std::string& dir);

	/// @brief 掴み攻撃（被攻撃側）モーション読み込み
	/// @param dir 
	void LoadGrabStrikeTarget(const std::string& dir);

	/// @brief 予備動作モーション読み込み
	/// @param dir 
	void LoadTelegraph(const std::string& dir);


private:

	/// @brief エンジン
	const GrowthEngine* engine_;

	/// @brief インスタンス
	static std::unique_ptr<MotionManager> instance_;

	/// @brief スケルトンのハンドル
	SkeletonHandle hSkeleton_;

	/// @brief テーブル
	using MotionTable = std::unordered_map<MotionType, std::unordered_map<std::string, AnimationHandle>>;
	MotionTable table_;

	/// @brief ジョイントマップ
	std::unordered_map<JointType, std::string> jointMap_;

	/// @brief ジョイントの親マップ
	std::unordered_map<JointType, JointType> parentMap_;

	/// @brief 初期化したかどうか
	bool isInit_ = false;
};

