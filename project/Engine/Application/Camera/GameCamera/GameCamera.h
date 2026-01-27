#pragma once
#include <memory>
#include <string>
#include "Handle/Handle.h"
#include "Application/Camera/Camera3D/Camera3D.h"

class GameCamera
{
private:

	/// @brief トランスフォーム
	struct Transform
	{
		/// @brief 回転
		Vector3* rotation = nullptr;

		/// @brief 位置
		Vector3* translate = nullptr;
	};


public:

	/// @brief コンストラクタ
	GameCamera(const std::string& name);

	/// @brief カメラを切り替える
	void Switch();

	/// @brief トランスフォーム
	Transform transform_{};


private:

	// 名前
	std::string name_;

	// カメラハンドル
	CameraHandle hCamera_ = 0;
};

