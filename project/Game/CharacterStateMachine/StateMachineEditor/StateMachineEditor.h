#pragma once
#include <string>

class Character;

class StateMachineEditor
{
public:

	/// @brief コンストラクタ
    StateMachineEditor() = default;

	/// @brief デストラクタ
    ~StateMachineEditor() = default;

	/// @brief UIを描画する
    /// @param character 
    void DrawUI(Character* character);

	/// @brief 選択中の状態の名前を取得する
    /// @return 
    const std::string& GetSelectedStateName() const { return selectedStateName_; }


private:

	/// @brief 選択中の状態の名前
	std::string selectedStateName_{};
};

