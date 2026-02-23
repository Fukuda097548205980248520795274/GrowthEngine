#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/CameraData/CameraData.h"

namespace Engine
{
	class BaseGameCamera
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		BaseGameCamera(const std::string& name) : name_(name){}

		/// @brief カメラを切り替える
		virtual void Switch() = 0;


	protected:

		// 名前
		std::string name_;
	};
}