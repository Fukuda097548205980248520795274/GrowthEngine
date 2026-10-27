#include "StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"
#include "../StageEditor.h"
#include "NavMesh/NavMesh.h"
#include "Scene/GameScene/GameScene.h"

/// @brief ファイルにステージデータを保存する
/// @param filename 
/// @param dataList 
/// @param navMesh 
/// @param stageSettings 
/// @return 
bool StageFileManager::SaveToFile(const std::string& filename, const std::vector<PlacementData>& dataList, const NavMesh* navMesh, const StageSettings& stageSettings)
{
	json rootJson;
	toJson(rootJson, dataList);
	toJson(rootJson, stageSettings);
	toJson(rootJson, *navMesh);

	// ファイルに書き出し
	std::filesystem::path dir(stageDataDir_);
	std::filesystem::path fullPath = dir / filename;

	// std::ios::binary を指定してバイナリモードで開く
	std::ofstream ofs(fullPath.string(), std::ios::binary);

	if (ofs.is_open())
	{
		// JSONをMessagePackバイナリに変換
		std::vector<uint8_t> v_msgpack = json::to_msgpack(rootJson);

		// バイナリデータを書き込み
		ofs.write(reinterpret_cast<const char*>(v_msgpack.data()), v_msgpack.size());
		ofs.close();

		return true;
	}

	return false;
}

/// @brief ファイルからステージデータを読み込む
/// @param filename 
/// @param outDataList 
/// @param spawner 
/// @param navMesh 
/// @param stageSettings 
/// @return 
bool StageFileManager::LoadFromFile(const std::string& filename, std::vector<PlacementData>& outDataList, StageSpawner* spawner, NavMesh* navMesh, StageSettings& stageSettings)
{
	outDataList.clear();
	if (spawner == nullptr) return false;

	// std::ios::binary を指定してバイナリモードで開く
	std::ifstream ifs(stageDataDir_ + filename, std::ios::binary);
	if (!ifs.is_open()) return false;

	// ファイルサイズを取得
	ifs.seekg(0, std::ios::end);
	size_t size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	// ファイルが空の場合は読み込み失敗とする
	if (size == 0) {
		ifs.close();
		return false;
	}

	std::vector<uint8_t> v_msgpack(size);
	ifs.read(reinterpret_cast<char*>(v_msgpack.data()), size);
	ifs.close();

	json rootJson;
	try
	{
		// MessagePackバイナリからJSONオブジェクトへ復元
		rootJson = json::from_msgpack(v_msgpack);
	}
	catch (const json::parse_error& e)
	{
		// パースに失敗した場合はエラーメッセージを出力して終了する（クラッシュを防ぐ）
		// 例: std::string errorMsg = e.what(); 
		// ※必要であれば OutputDebugStringA(e.what()); などでデバッガに出力してください
		return false;
	}

	if (rootJson.contains("objects") && rootJson["objects"].is_array())
	{
		for (const auto& itemJson : rootJson["objects"])
		{
			PlacementData data;

			// JSONから配置データを復元
			fromJson(itemJson, data);

			// 実際のゲーム内エンティティを生成
			if (spawner->SpawnActualEntity(data))
			{
				// リストに追加
				outDataList.push_back(data);
			}
		}
	}

	// ステージ設定を復元
	fromJson(rootJson, stageSettings);

	// NavMeshのデータも復元する
	if (navMesh)
	{
		// 既存のナビメッシュをクリア
		navMesh->Clear();

		if (rootJson.contains("navMesh"))
		{
			for (const auto& polyJson : rootJson["navMesh"])
			{
				NavPolygon poly;
				poly.id = polyJson["id"].get<int>();

				// 頂点の復元
				for (int i = 0; i < 4; ++i) {
					poly.vertices[i].x = polyJson["vertices"][i][0].get<float>();
					poly.vertices[i].y = polyJson["vertices"][i][1].get<float>();
					poly.vertices[i].z = polyJson["vertices"][i][2].get<float>();
				}

				// 隣接情報の復元
				for (int i = 0; i < 4; ++i) {
					poly.neighborIds[i] = polyJson["neighborIds"][i].get<int>();
				}

				// isActiveとgroupIdの復元
				poly.isActive = polyJson.value("isActive", true);
				poly.groupId = polyJson.value("groupId", 0);

				navMesh->AddPolygon(poly);
			}
		}
	}

	if (auto modelEditor = scene_->GetModelEditor())
	{
		modelEditor->Reset(); // モデルエディタの状態をリセットする
		modelEditor->Load(stageSettings.modelFileName); // モデルエディタにモデルを読み込む
	}
	if (auto lightEditor = scene_->GetLightEditor())
	{
		lightEditor->Reset(); // ライトエディタの状態をリセットする
		lightEditor->Load(stageSettings.lightFileName); // ライトエディタにライトを読み込む
	}


	// 読み込み成功
	return true;
}

/// @brief jsonをバイナリに変換する
/// @param jsonFileName 
/// @param binFileName 
bool StageFileManager::ConvertJsonToBinary(const std::string& jsonFileName, const std::string& binFileName)
{
	// 1. 既存のテキストJSONを読み込む
	std::ifstream ifs(stageDataDir_ + jsonFileName);
	if (!ifs.is_open()) return false;

	json rootJson;
	try {
		ifs >> rootJson; // テキストJSONからオブジェクトにパース
	}
	catch (...) {
		return false; // パース失敗
	}
	ifs.close();

	// 2. バイナリ（MessagePack）として保存する
	std::ofstream ofs(stageDataDir_ + binFileName, std::ios::binary);
	if (!ofs.is_open()) return false;

	std::vector<uint8_t> v_msgpack = json::to_msgpack(rootJson);
	ofs.write(reinterpret_cast<const char*>(v_msgpack.data()), v_msgpack.size());
	ofs.close();

	return true;
}

/// @brief ステージファイルをコピーする
/// @param srcFileName 
/// @param destFileName 
/// @return 
bool StageFileManager::CopyStageFile(const std::string& srcFileName, const std::string& destFileName)
{
	try
	{
		// コピー元のパスを作成
		std::filesystem::path srcPath = stageDataDir_ + srcFileName;

		// コピー先のパスを作成
		std::filesystem::path destPath = stageDataDir_ + destFileName;

		// コピー元のファイルが存在するか確認
		if (!std::filesystem::exists(srcPath))
			return false;

		std::filesystem::copy_file(srcPath, destPath, std::filesystem::copy_options::overwrite_existing);
		return true;
	}
	catch (const std::exception& e)
	{
		// エラー時の処理
		(void)e;
		return false;
	}
}

/// @brief 保存されているステージファイルのリストを取得する
/// @return 
std::vector<std::string> StageFileManager::GetSavedStageFiles() const
{
	std::vector<std::string> files;
	std::error_code ec; // エラー時の強制終了（クラッシュ）を防ぐための安全装置

	// そもそも保存先ディレクトリが存在するかチェック
	if (std::filesystem::exists(stageDataDir_, ec))
	{
		// フォルダの中身を1つずつ確認
		for (const auto& entry : std::filesystem::directory_iterator(stageDataDir_, ec))
		{
			// 何らかのエラーで読み取れないファイルがあったらスキップ
			if (ec) continue;

			// 「通常のファイル」かつ「拡張子が .json」のものだけをピックアップ
			if (entry.is_regular_file(ec) && entry.path().extension() == ".json")
			{
				// ファイル名だけを抽出してリストに追加
				files.push_back(entry.path().filename().string());
			}
		}
	}

	return files;
}