#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>

class Blackboard
{
public:

	/// @brief 値を格納する
	/// @tparam T 
	/// @param key 
	/// @param value 
	template<typename T>
	void SetValue(const std::string& key, T value)
	{
		dataTable_[key] = value;
	}

	/// @brief 格納された値を取得する
	/// @tparam T 
	/// @param key 
	/// @return 
	template<typename T>
	T GetValue(const std::string& key)
	{
		// キーを使って値を探す
		auto it = dataTable_.find(key);

		// データがあるかどうか
		if (it != dataTable_.end())
		{
			return std::any_cast<T>(it->second);
		}

		// 停止
		throw std::runtime_error("BlackBoard : key not found");
	}

	/// @brief 値が格納されているかどうか
	/// @param key 
	/// @return 
	bool HasValue(const std::string& key)
	{
		// キーを使って探す
		auto it = dataTable_.find(key);

		// データがあるかどうか
		return it != dataTable_.end() ? true : false;
	}


private:

	/// @brief データテーブル
	std::unordered_map<std::string, std::any> dataTable_;
};