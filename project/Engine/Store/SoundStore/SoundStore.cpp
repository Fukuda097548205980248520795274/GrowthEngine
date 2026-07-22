#include "SoundStore.h"
#include "Store/AudioStore/AudioStore.h"
#include "RenderContext/ImGuiRender/ImGuiRender.h"
#include <cassert>

#include "SoundData/BgmData/BgmData.h"
#include "SoundData/SeData/SeData.h"

/// @brief コンストラクタ
Engine::SoundStore::SoundStore()
{
	// サウンドパラメータを生成する
	parameter_ = std::make_unique<SoundParameter>("Sound");
}

/// @brief 初期化
/// @param audioStore 
void Engine::SoundStore::Initialize(AudioStore* audioStore)
{
	// nullptrチェック
	assert(audioStore);

	// 引数を受け取る
	audioStore_ = audioStore;
}

/// @brief 更新処理
void Engine::SoundStore::Update()
{
	for (auto& data : dataTable_)data->Update();
}

/// @brief 読み込み
/// @param name 
/// @param hAudio 
/// @param type 
SoundHandle Engine::SoundStore::Load(const std::string& name, AudioHandle hAudio, SoundType type)
{
	// 同じ名前を見つけたら、そのハンドルを返す
	for(auto& data : dataTable_)
		if (data->GetName() == name)
		{
			data->Reset();
			return data->GetHandle();
		}

	// サウンドハンドルを取得する
	SoundHandle hSound = static_cast<SoundHandle>(dataTable_.size());

	// 名前を記録する
	nameTable_[name] = hSound;

	// サウンドデータを作成してテーブルに追加する
	std::unique_ptr<SoundData> data;
	switch (type)
	{
	case SoundType::Bgm:
		data = std::make_unique<BgmData>(name, hSound);
		break;
	case SoundType::Se:
		data = std::make_unique<SeData>(name, hSound);
		break;
	default:
		assert(false);
	}

	data->Initialize(audioStore_, parameter_.get(), hAudio);
	dataTable_.push_back(std::move(data));

	return hSound;
}

/// @brief デバッグ用パラメータを表示する
void Engine::SoundStore::DebugParameter()
{
#ifdef DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin("Sound"))
	{
		ImGui::End();
		return;
	}

	for (auto& data : dataTable_)data->DebugParameter();

	// 終了
	ImGui::End();

#endif
}