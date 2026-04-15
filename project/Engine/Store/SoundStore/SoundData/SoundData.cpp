#include "SoundData.h"
#include "Store/AudioStore/AudioStore.h"
#include "Parameter/SoundParameter/SoundParameter.h"
#include "RenderContext/ImGuiRender/ImGuiRender.h"
#include <algorithm>
#include <cassert>

/// @brief 初期化
/// @param hAudio 
/// @param audioStore 
void Engine::SoundData::Initialize(AudioStore* audioStore, SoundParameter* parameter)
{
	// nullptrチェック
	assert(audioStore);
	assert(parameter);

	// 引数を受け取る
	audioStore_ = audioStore;
	parameter_ = parameter;

	// サウンドデータ
	param_ = std::make_unique<SoundParam>();
	param_->volume = 0.5f;
	param_->pitch = 1.0f;
	param_->enableLoop = false;

	// ファイルパスを取得する
	filePath_ = audioStore_->GetFilePath(hAudio_);

	if (parameter_)
	{
		// パラメータに値を登録する
		parameter_->SetValue(name_, "Volume", &param_->volume);
		parameter_->SetValue(name_, "Pitch", &param_->pitch);
		parameter_->SetValue(name_, "EnableLoop", &param_->enableLoop);
		parameter_->SetValue(name_, "FilePath", &filePath_);

		// 登録した調整項目の値に、ファイルの値を反映させる
		parameter_->RegisterGroupDataReflection(name_);
		hAudio_ = audioStore_->Load(filePath_, nullptr);
	}
}

/// @brief リセット
void Engine::SoundData::Reset()
{
	if (parameter_->IsFileFound(name_))
	{
		// 登録した調整項目の値に、ファイルの値を反映させる
		parameter_->RegisterGroupDataReflection(name_);
		hAudio_ = audioStore_->Load(filePath_, nullptr);
	}
	else
	{
		param_->volume = 0.5f;
		param_->pitch = 1.0f;
		param_->enableLoop = false;
	}
}

/// @brief 更新処理
void Engine::SoundData::Update()
{
	// 音量の制御
	param_->volume = std::clamp(param_->volume, 0.0f, 1.0f);

	// ボリューム変更
	if(param_->volume != preVolume_)
	{
		audioStore_->SetVolume(hPlay_, param_->volume);
	}

	// ピッチの変更
	if(param_->pitch != prePitch_)
	{
		audioStore_->SetPitch(hPlay_, param_->pitch);
	}

	// 前回の音量とピッチを記録する
	preVolume_ = param_->volume;
	prePitch_ = param_->pitch;


	// 音声が流れていない場合、何もしない
	if (isPlay_ == false)
		return;

	// ループ再生
	if (param_->enableLoop)
	{
		// 音声が流れていない場合、音声を流す
		if (!audioStore_->IsAudioPlay(hPlay_) || hPlay_ == 0)
		{
			hPlay_ = audioStore_->PlayAudio(hAudio_, param_->volume);
		}
	}
	else
	{
		// 音声が流れていない場合、再生フラグを下ろす
		if (!audioStore_->IsAudioPlay(hPlay_) || hPlay_ == 0)
		{
			isPlay_ = false;
		}
	}
}

/// @brief 再生する
void Engine::SoundData::Play()
{
	// 音声が流れている場合、停止する
	if (!audioStore_->IsAudioPlay(hPlay_))
	{
		audioStore_->StopAudio(hPlay_);
	}

	// 音声を流す
	hPlay_ = audioStore_->PlayAudio(hAudio_, param_->volume);
	isPlay_ = true;
}

/// @brief 停止する
void Engine::SoundData::Stop()
{
	// 音声を停止する
	audioStore_->StopAudio(hPlay_);
	isPlay_ = false;
}

/// @brief 再生されているかどうか
/// @return 
bool Engine::SoundData::IsPlay() const { return audioStore_->IsAudioPlay(hPlay_); }

/// @brief デバッグ用パラメータ
void Engine::SoundData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode(name_.c_str()))
	{
		// 音量の変更
		ImGui::DragFloat("Volume", &param_->volume, 0.001f, 0.0f, 1.0f);

		// ピッチの変更
		ImGui::DragFloat("Pitch", &param_->pitch, 0.001f, -10.0f, 10.0f);

		// ループの有効化
		ImGui::Checkbox("Enable Loop", &param_->enableLoop);



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