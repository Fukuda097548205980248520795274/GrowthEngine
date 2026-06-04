#include "StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"

/// @brief ファイルにステージデータを保存する
/// @param filename 
/// @param dataList 
/// @return 
bool StageFileManager::SaveToFile(const std::string& filename, const std::vector<PlacementData>& dataList)
{
    std::string fullPath = stageDataDir_ + filename;

    json rootJson;
    json arrayJson = json::array(); // 配置データの配列

    for (const auto& data : dataList)
    {
        json itemJson;
        itemJson["category"] = static_cast<int>(data.category);
        itemJson["subType"] = data.subType;

        // Vector3は配列として保存すると扱いやすいです
        itemJson["position"] = { data.position.x, data.position.y, data.position.z };
        itemJson["rotateY"] = data.rotateY;
        itemJson["scale"] = { data.scale.x, data.scale.y, data.scale.z };

        // 各種パラメータ
        itemJson["hp"] = data.hp;
        itemJson["durability"] = data.durability;
        itemJson["attackPower"] = data.attackPower;

		// 行動パターンのスクリプト名
		itemJson["behaviorScriptName"] = data.behaviorScriptName;

        // モーションハンドルの保存（モーション名で保存するのが理想ですが、今回はハンドル値として保存する例）
        itemJson["hStandMotion"] = data.standMotion.handle;
        itemJson["hStanceMotion"] = data.stanceMotion.handle;
        itemJson["hWalkMotion"] = data.walkMotion.handle;
        itemJson["hDashMotion"] = data.dashMotion.handle;
        itemJson["hAvoidFrontMotion"] = data.avoidFrontMotion.handle;
        itemJson["hAvoidBackMotion"] = data.avoidBackMotion.handle;
        itemJson["hAvoidLeftMotion"] = data.avoidLeftMotion.handle;
        itemJson["hAvoidRightMotion"] = data.avoidRightMotion.handle;

        arrayJson.push_back(itemJson);
    }

    rootJson["StageObjects"] = arrayJson;

    // ファイルに書き出し
    std::ofstream ofs(fullPath);
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
bool StageFileManager::LoadFromFile(const std::string& filename, std::vector<PlacementData>& outDataList, StageSpawner* spawner)
{
	if (spawner == nullptr) return false; // スポナーがなければ何もしない

    std::ifstream ifs(stageDataDir_ + filename);
    if (!ifs.is_open()) return false; // ファイルがなければ何もしない

    json rootJson;
    ifs >> rootJson;
    ifs.close();

    // 既存の配置データをクリア
    outDataList.clear();

    if (rootJson.contains("StageObjects") && rootJson["StageObjects"].is_array())
    {
        for (const auto& itemJson : rootJson["StageObjects"])
        {
            PlacementData data;

            // データの復元
            data.category = static_cast<EditCategory>(itemJson.value("category", 0));
            data.subType = itemJson.value("subType", 0);

            if (itemJson.contains("position"))
            {
                data.position.x = itemJson["position"][0];
                data.position.y = itemJson["position"][1];
                data.position.z = itemJson["position"][2];
            }
            data.rotateY = itemJson.value("rotateY", 0.0f);

            if (itemJson.contains("scale"))
            {
                data.scale.x = itemJson["scale"][0];
                data.scale.y = itemJson["scale"][1];
                data.scale.z = itemJson["scale"][2];
            }

            data.hp = itemJson.value("hp", 100);
            data.durability = itemJson.value("durability", 100);
            data.attackPower = itemJson.value("attackPower", 1.0f);

            data.standMotion.handle = itemJson.value("hStandMotion", 0);
            data.stanceMotion.handle = itemJson.value("hStanceMotion", 0);
            data.walkMotion.handle = itemJson.value("hWalkMotion", 0);
            data.dashMotion.handle = itemJson.value("hDashMotion", 0);
            data.avoidFrontMotion.handle = itemJson.value("hAvoidFrontMotion", 0);
            data.avoidBackMotion.handle = itemJson.value("hAvoidBackMotion", 0);
            data.avoidLeftMotion.handle = itemJson.value("hAvoidLeftMotion", 0);
            data.avoidRightMotion.handle = itemJson.value("hAvoidRightMotion", 0);

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