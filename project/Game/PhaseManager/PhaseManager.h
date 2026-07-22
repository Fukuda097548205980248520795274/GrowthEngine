#pragma once
#include <functional>
#include <unordered_map>

template <typename PhaseType>
class PhaseManager
{
public:

	using Action = std::function<void()>;


public:

	/// @brief コンストラクタ
	PhaseManager() = default;

	/// @brief デストラクタ
	~PhaseManager() = default;

	/// @brief 初期化
	/// @param initialPhase 
	void Initialize(PhaseType initialPhase)
	{
		currentPhase_ = initialPhase;

		// 初期フェーズの開始時のアクションを実行
		if (onEnterActions_.count(currentPhase_))
			onEnterActions_[currentPhase_]();
	}

	/// @brief 更新処理
	void Update()
	{
		// 現在のフェーズの更新時のアクションを実行
		if (onUpdateActions_.count(currentPhase_))
			onUpdateActions_[currentPhase_]();
	}

	/// @brief 描画処理
	void Draw()
	{
		// 現在のフェーズの描画時のアクションを実行
		if (onDrawActions_.count(currentPhase_))
			onDrawActions_[currentPhase_]();
	}

	/// @brief フェーズを変更する
	/// @param newPhase 
	void ChangePhase(PhaseType newPhase)
	{
		// 現在のフェーズの終了時のアクションを実行
		if (onExitActions_.count(currentPhase_))
			onExitActions_[currentPhase_]();

		// 新しいフェーズに変更
		currentPhase_ = newPhase;

		// 新しいフェーズの開始時のアクションを実行
		if (onEnterActions_.count(currentPhase_))
			onEnterActions_[currentPhase_]();
	}

	/// @brief 現在のフェーズを取得する
	/// @return 
	PhaseType GetCurrentPhase() const { return currentPhase_; }

	/// @brief フェーズの最初に実行されるアクションを設定する
	/// @param phase 
	/// @param action 
	void SetOnEnter(PhaseType phase, Action action) { onEnterActions_[phase] = action; }

	/// @brief フェーズの更新時に実行されるアクションを設定する
	/// @param phase 
	/// @param action 
	void SetOnUpdate(PhaseType phase, Action action) { onUpdateActions_[phase] = action; }

	/// @brief フェーズの終了時に実行されるアクションを設定する
	/// @param phase 
	/// @param action 
	void SetOnExit(PhaseType phase, Action action) { onExitActions_[phase] = action; }

	/// @brief フェーズの描画時に実行されるアクションを設定する
	/// @param phase 
	/// @param action 
	void SetOnDraw(PhaseType phase, Action action) { onDrawActions_[phase] = action; }


private:

	// 現在のフェーズ
	PhaseType currentPhase_{};

	// フェーズの開始時に実行されるアクションを格納するマップ
	std::unordered_map<PhaseType, Action> onEnterActions_;

	// フェーズの更新時に実行されるアクションを格納するマップ
	std::unordered_map<PhaseType, Action> onUpdateActions_;

	// フェーズの終了時に実行されるアクションを格納するマップ
	std::unordered_map<PhaseType, Action> onExitActions_;

	// フェーズの描画時に実行されるアクションを格納するマップ
	std::unordered_map<PhaseType, Action> onDrawActions_;
};

