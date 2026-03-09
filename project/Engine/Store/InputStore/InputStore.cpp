#include "InputStore.h"
#include <cassert>

#include "InputResource/InputKeyResource/InputKeyResource.h"
#include "InputResource/InputGamepadButtonResource/InputGamepadButtonResource.h"
#include "InputResource/InputGamepadLeftStickResource/InputGamepadLeftStickResource.h"
#include "InputResource/InputGamepadRightStickResource/InputGamepadRightStickResource.h"
#include "InputResource/InputGamepadLeftTriggerResource/InputGamepadLeftTriggerResource.h"
#include "InputResource/InputGamepadRightTriggerResource/InputGamepadRightTriggerResource.h"

/// @brief 初期化
/// @param input 
void Engine::InputStore::Initialize(Input* input)
{
	// nullptrチェック
	assert(input);

	// 引数を受け取る
	input_ = input;
}

/// @brief 更新処理
void Engine::InputStore::Update()
{
	// データの更新
	for (auto& data : dataTable_)data->Update();
}

/// @brief キー読み込み
/// @param name 
/// @param inputState 
/// @param key 
/// @return 
InputHandle Engine::InputStore::LoadKey(const std::string& name, InputState inputState, BYTE key)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドルを取得する
	InputHandle hInput = static_cast<InputHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hInput;

	// データの生成と初期化
	std::unique_ptr<InputKeyResource> data = std::make_unique<InputKeyResource>(inputState, hInput, name, key);
	data->Initialize(input_);
	dataTable_.push_back(std::move(data));

	return hInput;
}

/// @brief ゲームパッドボタン読み込み
/// @param name 
/// @param inputState 
/// @param controller 
/// @param button 
/// @return 
InputHandle Engine::InputStore::LoadGamepadButton(const std::string& name, InputState inputState, DWORD controller, DWORD button)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドルを取得する
	InputHandle hInput = static_cast<InputHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hInput;

	// データの生成と初期化
	std::unique_ptr<InputGamepadButtonResource> data = std::make_unique<InputGamepadButtonResource>(inputState, hInput, name, controller, button);
	data->Initialize(input_);
	dataTable_.push_back(std::move(data));

	return hInput;
}

/// @brief ゲームパッドスティック読み込み
/// @param name 
/// @param inputState 
/// @param stickType 
/// @param controller 
/// @param direction 
/// @param dot 
/// @return 
InputHandle Engine::InputStore::LoadGamepadStick(const std::string& name, InputState inputState, StickType stickType, DWORD controller, const Vector2& direction, float dot)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドルを取得する
	InputHandle hInput = static_cast<InputHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hInput;

	if (stickType == StickType::Left)
	{
		std::unique_ptr<InputGamepadLeftStickResource> data = std::make_unique<InputGamepadLeftStickResource>(inputState, hInput, name, controller, direction, dot);
		data->Initialize(input_);
		dataTable_.push_back(std::move(data));

		return hInput;
	}
	else if (stickType == StickType::Right)
	{
		std::unique_ptr<InputGamepadRightStickResource> data = std::make_unique<InputGamepadRightStickResource>(inputState, hInput, name, controller, direction, dot);
		data->Initialize(input_);
		dataTable_.push_back(std::move(data));

		return hInput;
	}

	assert(false);
	return 0;
}

/// @brief ゲームパッドトリガー読み込み
/// @param name 
/// @param inputState 
/// @param stickType 
/// @param controller 
/// @param threshold 
/// @return 
InputHandle Engine::InputStore::LoadGamepadTrigger(const std::string& name, InputState inputState, TriggerType triggerType, DWORD controller, float threshold)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドルを取得する
	InputHandle hInput = static_cast<InputHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = hInput;

	if (triggerType == TriggerType::Left)
	{
		std::unique_ptr<InputGamepadLeftTriggerResource> data = std::make_unique<InputGamepadLeftTriggerResource>(inputState, hInput, name, controller, threshold);
		data->Initialize(input_);
		dataTable_.push_back(std::move(data));

		return hInput;
	}
	else if (triggerType == TriggerType::Right)
	{
		std::unique_ptr<InputGamepadRightTriggerResource> data = std::make_unique<InputGamepadRightTriggerResource>(inputState, hInput, name, controller, threshold);
		data->Initialize(input_);
		dataTable_.push_back(std::move(data));

		return hInput;
	}

	assert(false);
	return 0;
}