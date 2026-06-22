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

		// イベントトリガーのステージデータファイル名
		char eventStageDataFileName[256] = "";

		// イベントが発生したときのコールバック関数
		std::function<bool(int, const char*)> onTriggerCallback = nullptr;
	};

	/// @brief イベントの種類
	enum class EventType
	{
		None,
		ObjectSpawn
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

	/// @brief イベントの種類を設定する
	/// @param eventType 
	void SetEventType(int eventType) { eventType_ = eventType; }

	/// @brief イベントの文字列パラメータを設定する
	/// @param param 
	void SetEventStringParam(const char* param) { strncpy_s(eventStageDataFileName_, sizeof(eventStageDataFileName_), param, _TRUNCATE); }


private:

	/// @brief 床の衝突判定
	Collision3DInstanceAABB* collision_ = nullptr;


	// イベントの種類
	int eventType_ = 0;

	// イベントの整数パラメータ
	char eventStageDataFileName_[256] = {};

	// イベントが発生したときのコールバック関数
	std::function<bool(int, const char*)> onTriggerCallback_ = nullptr;


public:

	/// @brief デバッグUIを描画する
	/// @param placementData 
	/// @param placementList 
	/// @param history 
	/// @param isDirty 
	void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty) override;
};

