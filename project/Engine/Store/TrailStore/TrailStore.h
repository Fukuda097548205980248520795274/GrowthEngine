#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "TrailData/TrailData.h"
#include "PSO/PSOTrail/PSOTrail.h"
#include "Handle/Handle.h"

namespace Engine
{
	class ShaderCompiler;
	class TextureStore;
	class Log;

	class TrailStore
	{
	public:

		/// @brief 初期化
		/// @param device
		/// @param compiler 
		/// @param textureStore 
		/// @param camera3DStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, TextureStore* textureStore, Camera3DStore* camera3DStore, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief トレイル読み込み
		/// @param name 
		/// @param maxLifeTime 
		/// @param device 
		/// @param log 
		/// @return 
		TrailHandle Load(const std::string& name, float maxLifeTime, TextureHandle hTexture, ID3D12Device* device, Log* log);

		/// @brief 描画処理
		/// @param hTrail 
		/// @param commandList 
		void Draw(TrailHandle hTrail, ID3D12GraphicsCommandList* commandList) { dataTable_[hTrail]->Register(commandList); }

		/// @brief 描画処理
		/// @param name 
		/// @param commandList 
		void Draw(const std::string& name, ID3D12GraphicsCommandList* commandList) { dataTable_[nameTable_[name]]->Register(commandList); }

		/// @brief パラメータを取得する
		/// @param hTrail 
		/// @return 
		TrailData::Param* GetParam(TrailHandle hTrail) { return dataTable_[hTrail]->GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		TrailData::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetParam(); }


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<TrailData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, TrailHandle> nameTable_;

		/// @brief トレイル用PSO
		std::unique_ptr<PSOTrail> psoTrail_;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* camera3DStore_ = nullptr;
	};
}