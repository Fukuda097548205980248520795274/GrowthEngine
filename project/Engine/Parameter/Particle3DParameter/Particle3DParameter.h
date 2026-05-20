#pragma once
#include "../Parameter.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Data/ParticleData/ParticleData.h"
#include "PSO/EnumBlendMode/EnumBlendMode.h"

namespace Engine
{
	class Particle3DParameter : public Parameter
	{
	public:

		/// @brief コンストラクタ
		/// @param folderName 
		Particle3DParameter(const std::string& folderName) : Parameter(folderName) {}

		/// @brief 登録した調整項目の値に、ファイルの値を反映させる
		/// @param fileName 
		void RegisterGroupDataReflection(const std::string& fileName) override;

		/// @brief ファイルを記録する
		/// @param fileName 
		void SaveFile(const std::string& fileName) override;

		/// @brief 項目に値を入れる
		/// @tparam T 
		/// @param fileName 
		/// @param key 
		/// @param value 
		template<typename T>
		void SetValue(const std::string& fileName, const std::string& key, T* value)
		{
			// グループの参照を取得
			Group& group = data_[fileName];

			// 新しい項目のデータを設定
			Item newItem{};
			newItem = value;

			// 設定した項目を追加
			group[key] = newItem;
		}


	protected:

		// 項目
		using Item = std::variant<bool*, int32_t*, uint32_t*, float*, Vector2*, Vector3*, Vector4*, std::string*, BlendMode*,
			Particle3D::EmitterShape* , Particle3D::AttractPostitionType*>;

		// グループ
		using Group = std::map<std::string, Item>;

		// 1全データ
		using Datas = std::map<std::string, Group>;

		// 全データ
		Datas data_;
	};
}