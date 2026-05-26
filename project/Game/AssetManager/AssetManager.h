#pragma once
#include "GrowthEngine.h"

class AssetManager
{
public:

	/// @brief デストラクタ
	~AssetManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static AssetManager* GetInstance();


	/// @brief スキニングモデル（キャラなど）を取得する
	/// @param filePath 
	/// @return 
	Render3DSkinningModel* GetSkinningModel(const std::string& filePath);

	/// @brief プレハブ静的モデル（床など）を取得する
	/// @param filePath 
	/// @return 
	PrefabBaseStaticModel* GetPrefabStaticModel(const std::string& filePath);


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();


	// ロード済みのモデルをキャッシュしておくマップ
	std::unordered_map<std::string, std::unique_ptr<Render3DSkinningModel>> skinningModels_;
	std::unordered_map<std::string, std::unique_ptr<PrefabBaseStaticModel>> prefabStaticModels_;


private:

	/// @brief インスタンス
	static std::unique_ptr<AssetManager> instance_;

	// コピー禁止
	AssetManager() = default;
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
};

