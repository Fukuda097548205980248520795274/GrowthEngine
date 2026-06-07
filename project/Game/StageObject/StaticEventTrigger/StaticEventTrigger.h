#pragma once
#include "../StageObject.h"

class StaticEventTrigger : public StageObject
{
public:

	struct InitData
	{
		/// @brief 床の位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// 床の大きさ
		Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

		// 地面の衝突判定
		Collision3DInstanceAABB* collision = nullptr;

		
		// イベントの種類
		int eventType = 0;

		// イベントの整数パラメータ
		std::string eventStringParam = "";

		// イベントが発生したときのコールバック関数
		std::function<void(int, const std::string&)> onTriggerCallback = nullptr;
	};

public:

	/// @brief コンストラクタ
	/// @param initData 
	StaticEventTrigger();

	/// @brief デストラクタ
	~StaticEventTrigger();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	virtual void Update() override;


private:

	/// @brief 床の衝突判定
	Collision3DInstanceAABB* collision_ = nullptr;


	// イベントの種類
	int eventType_ = 0;

	// イベントの整数パラメータ
	std::string eventStringParam_;

	// イベントが発生したときのコールバック関数
	std::function<void(int, const std::string&)> onTriggerCallback_ = nullptr;
};

