#include "AppParam.h"
#include "GrowthEngine.h"

/// @brief 更新処理
void AppParam::Update()
{
#ifdef DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin(folderName_.c_str()))
	{
		ImGui::End();
		return;
	}


	// 各グループについて
	for (Datas::iterator itGroup = data_.begin(); itGroup != data_.end(); ++itGroup)
	{
		// グループ名を取得
		const std::string& groupName = itGroup->first;

		// グループ参照
		Group& group = itGroup->second;


		// 各項目について
		for (Group::iterator itItem = group.begin(); itItem != group.end(); ++itItem)
		{
			// 項目名を取得
			const std::string& itemName = itItem->first;

			// 項目参照
			Item& item = itItem->second;


			// bool型
			if (std::holds_alternative<bool*>(item))
			{
				bool** ptr = std::get_if<bool*>(&item);
				ImGui::Checkbox(itemName.c_str(), *ptr);
			}
			else if (std::holds_alternative<int32_t*>(item))
			{
				// int32_t型
				int32_t** ptr = std::get_if<int32_t*>(&item);
				ImGui::DragInt(itemName.c_str(), *ptr, 0.5f, -100000, 100000);
			} 
			else if (std::holds_alternative<uint32_t*>(item))
			{
				// uint32_t型
				uint32_t** ptr = std::get_if<uint32_t*>(&item);
				uint32_t min = 0;
				uint32_t max = 100000;
				ImGui::DragScalar(itemName.c_str(), ImGuiDataType_U32, *ptr, 0.5f, &min, &max);
			} 
			else if (std::holds_alternative<float*>(item))
			{
				// float型
				float** ptr = std::get_if<float*>(&item);
				ImGui::DragFloat(itemName.c_str(), *ptr, 0.01f, -100000.0f, 100000.0f);
			}
			else if (std::holds_alternative<Vector2*>(item))
			{
				// Vector2型
				Vector2** ptr = std::get_if<Vector2*>(&item);
				ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(*ptr), 0.01f, -100000.0f, 100000.0f);
			}
			else if (std::holds_alternative<Vector3*>(item))
			{
				// Vector3型
				Vector3** ptr = std::get_if<Vector3*>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(*ptr), 0.01f, -100000.0f, 100000.0f);
			}
			else if (std::holds_alternative<Vector4*>(item))
			{
				// Vector4型
				Vector4** ptr = std::get_if<Vector4*>(&item);
				ImGui::DragFloat4(itemName.c_str(), reinterpret_cast<float*>(*ptr), 0.01f, -100000.0f, 100000.0f);
			}
		}


		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			SaveFile(groupName);
			std::string message = std::format("{} : saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			RegisterGroupDataReflection(groupName);
			std::string message = std::format("{} : loaded.", groupName);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}
	}



	// 終了
	ImGui::End();

#endif
}

/// @brief 登録した調整項目の値に、ファイルの値を反映させる
/// @param fileName 
void AppParam::RegisterGroupDataReflection(const std::string& fileName)
{
	// ファイルパス
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";

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
	json::iterator itGroup = root.find(fileName);

	// 未登録は何もしない
	if (itGroup == root.end())
		return;


	// グループ参照
	Group& group = data_[fileName];

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
			} else if (itItem->is_number_integer() && std::holds_alternative<int32_t*>(item))
			{
				// int32_t型
				int32_t value = itItem->get<int32_t>();
				int32_t** ptr = std::get_if<int32_t*>(&item);
				**ptr = value;
			} else if (itItem->is_number_integer() && std::holds_alternative<uint32_t*>(item))
			{
				// uint32_t型
				uint32_t value = itItem->get<uint32_t>();
				uint32_t** ptr = std::get_if<uint32_t*>(&item);
				**ptr = value;
			} else if (itItem->is_number_float() && std::holds_alternative<float*>(item))
			{
				// float型
				float value = itItem->get<float>();
				float** ptr = std::get_if<float*>(&item);
				**ptr = value;
			} else if (itItem->is_array() && itItem->size() == 2 && std::holds_alternative<Vector2*>(item))
			{
				// Vector2型
				Vector2 value = { itItem->at(0), itItem->at(1) };
				Vector2** ptr = std::get_if<Vector2*>(&item);
				**ptr = value;
			} else if (itItem->is_array() && itItem->size() == 3 && std::holds_alternative<Vector3*>(item))
			{
				// Vector3型
				Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
				Vector3** ptr = std::get_if<Vector3*>(&item);
				**ptr = value;
			} else if (itItem->is_array() && itItem->size() == 4 && std::holds_alternative<Vector4*>(item))
			{
				// Vector4型
				Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
				Vector4** ptr = std::get_if<Vector4*>(&item);
				**ptr = value;
			}


			break;
		}
	}
}

/// @brief ファイルを記録する
/// @param fileName 
void AppParam::SaveFile(const std::string& fileName)
{
	// グループを検索
	Datas::iterator itGroup = data_.find(fileName);

	// 未登録のときは処理しない
	if (itGroup == data_.end())
		return;


	// データコンテナ
	json root;
	root = json::object();

	// jsonオブジェクトに登録
	root[fileName] = json::object();

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
			root[fileName][itemName] = *value;
		} else if (std::holds_alternative<int32_t*>(item))
		{
			// int32_t型
			int32_t* value = std::get<int32_t*>(item);
			root[fileName][itemName] = *value;
		} else if (std::holds_alternative<uint32_t*>(item))
		{
			// uint32_t型
			uint32_t* value = std::get<uint32_t*>(item);
			root[fileName][itemName] = *value;
		} else if (std::holds_alternative<float*>(item))
		{
			//float型
			float* value = std::get<float*>(item);
			root[fileName][itemName] = *value;
		} else if (std::holds_alternative<Vector2*>(item))
		{
			// Vector2型
			Vector2* value = std::get<Vector2*>(item);
			root[fileName][itemName] = json::array({ value->x , value->y });
		} else if (std::holds_alternative<Vector3*>(item))
		{
			// Vector3型
			Vector3* value = std::get<Vector3*>(item);
			root[fileName][itemName] = json::array({ value->x , value->y , value->z });
		} else if (std::holds_alternative<Vector4*>(item))
		{
			// Vector4型
			Vector4* value = std::get<Vector4*>(item);
			root[fileName][itemName] = json::array({ value->x, value->y, value->z , value->w });
		}
	}

	// ディレクトリがなければ作成する
	std::filesystem::path dir(directory_);
	if (!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}

	// 新規ファイルを作成する
	CreateRecordFile(fileName);

	// ファイルパス
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";

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