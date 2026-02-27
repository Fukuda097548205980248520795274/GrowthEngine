#include "PrimitiveParameter.h"

/// @brief 更新処理
void Engine::PrimitiveParameter::Update()
{

}

/// @brief 登録した調整項目の値に、ファイルの値を反映させる
/// @param groupName 
void Engine::PrimitiveParameter::RegisterGroupDataReflection(const std::string& groupName)
{
	// ファイルパス
	std::string filePath = directory_ + groupName + ".json";

	// 入力ファイルストリーム
	std::ifstream ifs;
	ifs.open(filePath);

	// ファイルが開けなかったら何もしない
	if (!ifs.is_open())
		return;


	// データコンテナ
	json root;
	ifs >> root;

	// ファイルを閉じる
	ifs.close();

	// グループを検索
	json::iterator itGroup = root.find(groupName);

	// 未登録は何もしない
	if (itGroup == root.end())
		return;


	// グループ参照
	Group& group = data_[groupName];

	// json各項目について
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem)
	{
		// アイテム名を取得
		const std::string& itemName = itItem.key();

		// データの各項目について
		for (Group::iterator itDataItem = group.begin(); itDataItem != group.end(); ++itDataItem)
		{
			// アイテム名を取得
			const std::string& dataItemName = itDataItem->first;

			// 一致したら処理する
			if (strcmp(itemName.c_str(), dataItemName.c_str()))
				continue;

			// アイテムを参照
			Item& item = itDataItem->second;


			// bool型
			if (itItem->is_boolean() && std::holds_alternative<bool*>(item))
			{
				bool value = itItem->get<bool>();
				bool** ptr = std::get_if<bool*>(&item);
				**ptr = value;
			}
			else if (itItem->is_number_integer() && std::holds_alternative<int32_t*>(item))
			{
				// int32_t型
				int32_t value = itItem->get<int32_t>();
				int32_t** ptr = std::get_if<int32_t*>(&item);
				**ptr = value;
			}
			else if (itItem->is_number_integer() && std::holds_alternative<uint32_t*>(item))
			{
				// uint32_t型
				uint32_t value = itItem->get<uint32_t>();
				uint32_t** ptr = std::get_if<uint32_t*>(&item);
				**ptr = value;
			}
			else if (itItem->is_number_float() && std::holds_alternative<float*>(item))
			{
				// float型
				float value = itItem->get<float>();
				float** ptr = std::get_if<float*>(&item);
				**ptr = value;
			}
			else if (itItem->is_array() && itItem->size() == 2 && std::holds_alternative<Vector2*>(item))
			{
				// Vector2型
				Vector2 value = { itItem->at(0), itItem->at(1) };
				Vector2** ptr = std::get_if<Vector2*>(&item);
				**ptr = value;
			}
			else if (itItem->is_array() && itItem->size() == 3 && std::holds_alternative<Vector3*>(item))
			{
				// Vector3型
				Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
				Vector3** ptr = std::get_if<Vector3*>(&item);
				**ptr = value;
			}
			else if (itItem->is_array() && itItem->size() == 4 && std::holds_alternative<Vector4*>(item))
			{
				// Vector4型
				Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
				Vector4** ptr = std::get_if<Vector4*>(&item);
				**ptr = value;
			}
			else if (itItem->is_string() && std::holds_alternative<std::string*>(item))
			{
				std::string value = itItem->get<std::string>();
				std::string** ptr = std::get_if<std::string*>(&item);
				**ptr = value;
			}


			break;
		}
	}
}

/// @brief ファイルを記録する
/// @param groupName 
void Engine::PrimitiveParameter::SaveFile(const std::string& groupName)
{
	// グループを検索
	Datas::iterator itGroup = data_.find(groupName);

	// 未登録のときは処理しない
	if (itGroup == data_.end())
		return;


	// データコンテナ
	json root;
	root = json::object();

	// jsonオブジェクトに登録
	root[groupName] = json::object();

	// 各行もについて
	for (Group::iterator itItem = itGroup->second.begin(); itItem != itGroup->second.end(); ++itItem)
	{
		// 項目名を取得
		const std::string& itemName = itItem->first;

		// 項目を参照
		Item& item = itItem->second;


		// bool型
		if (std::holds_alternative<bool*>(item))
		{
			bool* value = std::get<bool*>(item);
			root[groupName][itemName] = *value;
		}
		else if (std::holds_alternative<int32_t*>(item))
		{
			// int32_t型
			int32_t* value = std::get<int32_t*>(item);
			root[groupName][itemName] = *value;
		}
		else if (std::holds_alternative<uint32_t*>(item))
		{
			// uint32_t型
			uint32_t* value = std::get<uint32_t*>(item);
			root[groupName][itemName] = *value;
		}
		else if (std::holds_alternative<float*>(item))
		{
			//float型
			float* value = std::get<float*>(item);
			root[groupName][itemName] = *value;
		}
		else if (std::holds_alternative<Vector2*>(item))
		{
			// Vector2型
			Vector2* value = std::get<Vector2*>(item);
			root[groupName][itemName] = json::array({ value->x , value->y });
		}
		else if (std::holds_alternative<Vector3*>(item))
		{
			// Vector3型
			Vector3* value = std::get<Vector3*>(item);
			root[groupName][itemName] = json::array({ value->x , value->y , value->z });
		}
		else if (std::holds_alternative<Vector4*>(item))
		{
			// Vector4型
			Vector4* value = std::get<Vector4*>(item);
			root[groupName][itemName] = json::array({ value->x, value->y, value->z , value->w });
		}
		else if (std::holds_alternative<std::string*>(item))
		{
			std::string* value = std::get<std::string*>(item);
			root[groupName][itemName] = *value;
		}

	}

	// ディレクトリがなければ作成する
	std::filesystem::path dir(directory_);
	if (!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}

	// 新規ファイルを作成する
	CreateRecordFile(groupName);

	// ファイルパス
	std::string filePath = directory_ + groupName + ".json";

	// 出力ファイルストリーム
	std::ofstream ofs;
	ofs.open(filePath);

	// ファイルが開けなかった時
	if (ofs.is_open() == false)
	{
		return;
	}

	// ファイルにJSON文字列を書き込む
	ofs << std::setw(4) << root << std::endl;

	// ファイルを閉じる
	ofs.close();

	return;
}