#include "BehaviorTreeSetting.h"

/// @brief ツリー構造をファイルに保存する
/// @param fileName 
/// @param nodes 
/// @param links 
void BehaviorTreeSetting::SaveTree(const std::string& fileName, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links)
{

    json root;

	// ノードの保存
    for (const auto& node : nodes) 
    {
        json n;
        n["id"] = node.id;
        n["type"] = node.type;
        n["pos"] = { node.pos.x, node.pos.y };
        n["input_pin"] = node.inputPinId;
        n["output_pin"] = node.outputPinId;

		// ノード固有のパラメータも記録 （必要に応じて拡張可能）
        n["params"] = -1;

        root["nodes"].push_back(n);
    }

	// リンクの保存
    for (const auto& link : links)
    {
        json l;
        l["id"] = link.id;
        l["start"] = link.startPinId;
        l["end"] = link.endPinId;
        root["links"].push_back(l);
    }

	// ファイルパスの構築
    std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
    std::ofstream ofs(filePath);
    if (ofs.is_open()) 
    {
		// JSONを整形して保存
        ofs << root.dump(4);
    }
}

/// @brief ファイルからツリー構造を読み込む
/// @param fileName 
/// @param out_nodes 
/// @param out_links 
void BehaviorTreeSetting::LoadTree(const std::string& fileName, std::vector<EditorNode>& out_nodes, std::vector<EditorLink>& out_links)
{
	// ファイルパスの構築
    std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) return;

	// JSONの読み込み
    json root;
    ifs >> root;

	// 読み込む前に出力用のコンテナをクリア
    out_nodes.clear();
    out_links.clear();

	// ノードの読み込み
    if (root.contains("nodes")) 
    {
        for (auto& n : root["nodes"]) 
        {
            EditorNode node;
            node.id = n["id"];
            node.type = n["type"];
            node.pos.x = n["pos"][0];
            node.pos.y = n["pos"][1];
            node.inputPinId = n["input_pin"];
            node.outputPinId = n["output_pin"];

            if (n.contains("params"))
            {
				// 追加のパラメータがある場合はここで処理
            }

            out_nodes.push_back(node);
        }
    }

	// リンクの読み込み
    if (root.contains("links")) 
    {
        for (auto& l : root["links"])
        {
            EditorLink link;
            link.id = l["id"];
            link.startPinId = l["start"];
            link.endPinId = l["end"];
            out_links.push_back(link);
        }
    }
}