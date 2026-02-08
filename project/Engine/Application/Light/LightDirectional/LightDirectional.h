#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Handle/Handle.h"
#include <string>

class LightDirectional
{
private:

	// パラメータ
	struct Param
	{
		// 方向
		Vector3* directional = nullptr;

		// 輝度
		float* intensity = nullptr;

		// 色
		Vector3* color = nullptr;
	};

public:

	/// @brief コンストラクタ
	/// @param name 
	LightDirectional(const std::string& name);

	/// @brief 設置
	void Set();


	/// @brief パラメータ
	Param param_{};


private:

	/// @brief 名前
	std::string name_{};

	/// @brief ハンドル
	LightHandle handle_ = 0;
};

