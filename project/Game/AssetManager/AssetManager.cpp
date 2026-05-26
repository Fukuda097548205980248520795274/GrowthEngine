#include "AssetManager.h"

// インスタンスの初期化
std::unique_ptr<AssetManager> AssetManager::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @return 
AssetManager* AssetManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new AssetManager());
	}

	return instance_.get();
}

///// @brief スキニングモデル（キャラなど）を取得する
///// @param filePath 
///// @return 
//Render3DSkinningModel* AssetManager::GetSkinningModel(const std::string& filePath)
//{
//    
//}
//
///// @brief プレハブ静的モデル（床など）を取得する
///// @param filePath 
///// @return 
//PrefabBaseStaticModel* AssetManager::GetPrefabStaticModel(const std::string& filePath)
//{
//
//}