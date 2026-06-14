#include "StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"
#include "NavMesh/NavMesh.h"

/// @brief ファイルにステージデータを保存する
/// @param filename 
/// @param dataList 
/// @return 
bool StageFileManager::SaveToFile(const std::string& filename, const std::vector<PlacementData>& dataList, const NavMesh* navMesh)
{
	json rootJson;

	// 配置データをJSONに変換して保存する
	toJson(rootJson, dataList);

	// NavMeshのデータも保存する
	toJson(rootJson, *navMesh);


	// ファイルに書き出し
	std::filesystem::path dir(stageDataDir_);
	std::filesystem::path fullPath = dir / filename;
	std::ofstream ofs(fullPath.string());

	if (ofs.is_open())
	{
		ofs << rootJson.dump(4); // 4文字インデントで見やすく整形
		ofs.close();

		// 保存成功
		return true;
	}

	// 保存失敗
	return false;
}

/// @brief ファイルからステージデータを読み込む
/// @param filename 
/// @param outDataList 
/// @return 
bool StageFileManager::LoadFromFile(const std::string& filename, std::vector<PlacementData>& outDataList, StageSpawner* spawner, NavMesh* navMesh)
{
	if (spawner == nullptr) return false; // スポナーがなければ何もしない

	std::ifstream ifs(stageDataDir_ + filename);
	if (!ifs.is_open()) return false; // ファイルがなければ何もしない

	json rootJson;
	ifs >> rootJson;
	ifs.close();

	// 既存の配置データをクリア
	outDataList.clear();

	if (rootJson.contains("objects") && rootJson["objects"].is_array())
	{
		for (const auto& itemJson : rootJson["objects"])
		{
			PlacementData data;

			// JSONから配置データを復元
			fromJson(itemJson, data);

			// 実際のゲーム内エンティティを生成
			spawner->SpawnActualEntity(data);

			// リストに追加
			outDataList.push_back(data);
		}
	}


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

				navMesh->AddPolygon(poly);
			}
		}
	}


	// 読み込み成功
	return true;
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
				// ファイル名（例: "stage1.json"）だけを抽出してリストに追加
				files.push_back(entry.path().filename().string());
			}
		}
	}

	return files;
}