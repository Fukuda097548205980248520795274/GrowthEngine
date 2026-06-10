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
    json arrayJson = json::array(); // 配置データの配列

    for (const auto& data : dataList)
    {
        json itemJson;
        itemJson["category"] = static_cast<int>(data.category);
        itemJson["subType"] = data.subType;
		itemJson["name"] = data.name;

        // Vector3は配列として保存すると扱いやすいです
        itemJson["position"] = { data.position.x, data.position.y, data.position.z };
		itemJson["rotate"] = { data.rotate_.x, data.rotate_.y, data.rotate_.z };
        itemJson["scale"] = { data.scale.x, data.scale.y, data.scale.z };

        // 各種パラメータ
        itemJson["hp"] = data.hp;
        itemJson["durability"] = data.durability;
        itemJson["attackPower"] = data.attackPower;
		itemJson["isUnbreakable"] = data.isUnbreakable;

		// 行動パターンのスクリプト名
		itemJson["behaviorScriptName"] = data.behaviorScriptName;

		// モーション名
		itemJson["standMotionName"] = data.standMotion.name;
		itemJson["stanceMotionName"] = data.stanceMotion.name;
		itemJson["walkMotionName"] = data.walkMotion.name;
		itemJson["dashMotionName"] = data.dashMotion.name;
		itemJson["avoidFrontMotionName"] = data.avoidFrontMotion.name;
		itemJson["avoidBackMotionName"] = data.avoidBackMotion.name;
		itemJson["avoidLeftMotionName"] = data.avoidLeftMotion.name;
		itemJson["avoidRightMotionName"] = data.avoidRightMotion.name;

        arrayJson.push_back(itemJson);
    }
    rootJson["objects"] = arrayJson;


	// NavMeshのデータも保存する
    json navMeshJson = json::array();
    if (navMesh)
    {
        for (const auto& poly : navMesh->GetPolygons())
        {
            json polyJson;
            polyJson["id"] = poly.id;

            // 頂点配列の保存
            json vertsJson = json::array();
            for (const auto& v : poly.vertices) {
                vertsJson.push_back({ v.x, v.y, v.z });
            }
            polyJson["vertices"] = vertsJson;

            // 隣接IDの保存
            json neighborsJson = json::array();
            for (int neighborId : poly.neighborIds) {
                neighborsJson.push_back(neighborId);
            }
            polyJson["neighborIds"] = neighborsJson;

            navMeshJson.push_back(polyJson);
        }
    }
    rootJson["navMesh"] = navMeshJson;



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

            // データの復元
            data.category = static_cast<EditCategory>(itemJson.value("category", 0));
            data.subType = itemJson.value("subType", 0);
			
			data.name[0] = '\0'; // デフォルトは空文字列
			if (itemJson.contains("name"))
			{
				std::string nameStr = itemJson["name"].get<std::string>();
				strncpy_s(data.name, nameStr.c_str(), sizeof(data.name) - 1);
				data.name[sizeof(data.name) - 1] = '\0'; // 念のためヌル終端
			}

            if (itemJson.contains("position"))
            {
                data.position.x = itemJson["position"][0];
                data.position.y = itemJson["position"][1];
                data.position.z = itemJson["position"][2];
            }
            
			if (itemJson.contains("rotate"))
			{
				data.rotate_.x = itemJson["rotate"][0];
				data.rotate_.y = itemJson["rotate"][1];
				data.rotate_.z = itemJson["rotate"][2];
			}

            if (itemJson.contains("scale"))
            {
                data.scale.x = itemJson["scale"][0];
                data.scale.y = itemJson["scale"][1];
                data.scale.z = itemJson["scale"][2];
            }

            data.hp = itemJson.value("hp", 100);
            data.durability = itemJson.value("durability", 100);
            data.attackPower = itemJson.value("attackPower", 1.0f);
			data.isUnbreakable = itemJson.value("isUnbreakable", false);

			data.standMotion.name = itemJson.value("standMotionName", "Standing");
			data.stanceMotion.name = itemJson.value("stanceMotionName", "Fighter");
			data.walkMotion.name = itemJson.value("walkMotionName", "Walk");
			data.dashMotion.name = itemJson.value("dashMotionName", "Dash");
			data.avoidFrontMotion.name = itemJson.value("avoidFrontMotionName", "Front");
			data.avoidBackMotion.name = itemJson.value("avoidBackMotionName", "Back");
			data.avoidLeftMotion.name = itemJson.value("avoidLeftMotionName", "Front");
			data.avoidRightMotion.name = itemJson.value("avoidRightMotionName", "Back");

			MotionManager* motionManager = MotionManager::GetInstance();
			data.standMotion.handle = motionManager->GetMotion(MotionType::Stand, data.standMotion.name);
			data.stanceMotion.handle = motionManager->GetMotion(MotionType::Stance, data.stanceMotion.name);
			data.walkMotion.handle = motionManager->GetMotion(MotionType::Walk, data.walkMotion.name);
			data.dashMotion.handle = motionManager->GetMotion(MotionType::Dash, data.dashMotion.name);
			data.avoidFrontMotion.handle = motionManager->GetMotion(MotionType::Avoid, data.avoidFrontMotion.name);
			data.avoidBackMotion.handle = motionManager->GetMotion(MotionType::Avoid, data.avoidBackMotion.name);
			data.avoidLeftMotion.handle = motionManager->GetMotion(MotionType::Avoid, data.avoidLeftMotion.name);
			data.avoidRightMotion.handle = motionManager->GetMotion(MotionType::Avoid, data.avoidRightMotion.name);

			data.behaviorScriptName[0] = '\0'; // デフォルトは空文字列
            if (itemJson.contains("behaviorScriptName"))
            {
				std::string scriptName = itemJson["behaviorScriptName"].get<std::string>();
				strncpy_s(data.behaviorScriptName, scriptName.c_str(), sizeof(data.behaviorScriptName) - 1);
				data.behaviorScriptName[sizeof(data.behaviorScriptName) - 1] = '\0'; // 念のためヌル終端
            }

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