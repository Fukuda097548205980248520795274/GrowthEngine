#pragma once
#include "SoundData/SoundData.h"
#include <vector>
#include <unordered_map>
#include "Parameter/SoundParameter/SoundParameter.h"

namespace Engine
{
	class AudioStore;

	class SoundStore
	{
	public:

		/// @brief コンストラクタ
		SoundStore();

		/// @brief 初期化
		/// @param audioStore 
		void Initialize(AudioStore* audioStore);

		/// @brief リセット
		void Reset();

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param name 
		/// @param hAudio 
		/// @param type 
		SoundHandle Load(const std::string& name, AudioHandle hAudio, SoundType type);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hSound 
		/// @return 
		template<typename T>
		T* GetParam(SoundHandle hSound) {return static_cast<T*>(dataTable_[hSound]->GetParam());}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name) { return GetParam<T>(nameTable_[name]); }

		/// @brief 再生する
		/// @param hSound 
		void Play(SoundHandle hSound) { dataTable_[hSound]->Play(); }

		/// @brief 再生する
		/// @param name 
		void Play(const std::string& name) { Play(nameTable_[name]); }

		/// @brief 停止する
		/// @param hSound 
		void Stop(SoundHandle hSound) { dataTable_[hSound]->Stop(); }

		/// @brief 停止する
		/// @param name 
		void Stop(const std::string& name) { Stop(nameTable_[name]); }

		/// @brief 再生しているかどうか
		/// @param hSound 
		/// @return 
		bool IsPlay(SoundHandle hSound) { return dataTable_[hSound]->IsPlay(); }

		/// @brief 再生しているかどうか
		/// @param name 
		/// @return 
		bool IsPlay(const std::string& name) { return IsPlay(nameTable_[name]); }


	public:

		/// @brief デバッグ用パラメータを表示する
		void DebugParameter();


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<SoundData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, SoundHandle> nameTable_;

		/// @brief パラメータ
		std::unique_ptr<SoundParameter> parameter_ = nullptr;

		/// @brief オーディオストア
		AudioStore* audioStore_ = nullptr;
	};
}