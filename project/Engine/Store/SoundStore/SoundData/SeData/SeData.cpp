#include "SeData.h"
#include "Store/AudioStore/AudioStore.h"
#include "Parameter/SoundParameter/SoundParameter.h"
#include "RenderContext/ImGuiRender/ImGuiRender.h"
#include <algorithm>
#include <cassert>

/// @brief 初期化
/// @param hAudio 
/// @param audioStore 
void Engine::SeData::Initialize(AudioStore* audioStore, SoundParameter* parameter, AudioHandle hAudio)
{
	// nullptrチェック
	assert(audioStore);
	assert(parameter);

	// 引数を受け取る
	audioStore_ = audioStore;
	parameter_ = parameter;

	// サウンドデータ
	param_ = std::make_unique<SeParam>();
	param_->volume = 0.5f;
	param_->pitch = 1.0f;
	param_->hAudio = hAudio;

	// ファイルパスを取得する
	filePath_ = audioStore_->GetFilePath(param_->hAudio);

	if (parameter_)
	{
		// パラメータに値を登録する
		parameter_->SetValue(name_, "Volume", &param_->volume);
		parameter_->SetValue(name_, "Pitch", &param_->pitch);
		parameter_->SetValue(name_, "FilePath", &filePath_);

		// 登録した調整項目の値に、ファイルの値を反映させる
		parameter_->RegisterGroupDataReflection(name_);
		param_->hAudio = audioStore_->Load(filePath_, nullptr);
	}
}

/// @brief リセット
void Engine::SeData::Reset()
{
	if (parameter_->IsFileFound(name_))
	{
		// 登録した調整項目の値に、ファイルの値を反映させる
		parameter_->RegisterGroupDataReflection(name_);
		param_->hAudio = audioStore_->Load(filePath_, nullptr);
	}
	else
	{
		param_->volume = 0.5f;
		param_->pitch = 1.0f;
	}
}

/// @brief 更新処理
void Engine::SeData::Update()
{
	// 音量の制御
	param_->volume = std::clamp(param_->volume, 0.0f, 1.0f);
}

/// @brief 再生する
void Engine::SeData::Play()
{
	// 音声を流す
	audioStore_->PlayAudio(param_->hAudio, param_->volume);
}

/// @brief デバッグ用パラメータ
void Engine::SeData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode(("SE_" + name_).c_str()))
	{
		// 音量の変更
		ImGui::DragFloat("Volume", &param_->volume, 0.001f, 0.0f, 1.0f);

		// ピッチの変更
		ImGui::DragFloat("Pitch", &param_->pitch, 0.001f, -10.0f, 10.0f);



		ImGui::Text("\n");

		// 再生ボタン
		if (ImGui::Button("Play"))
			Play();

		ImGui::Text("\n");

		// 停止ボタン
		if (ImGui::Button("Stop"))
			Stop();

		ImGui::Text("\n");



		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(name_);
			std::string message = std::format("{} : saved.", name_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(name_);
			std::string message = std::format("{} : loaded.", name_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		ImGui::TreePop();
	}

#endif
}