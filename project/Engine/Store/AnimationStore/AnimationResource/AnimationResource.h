#pragma once
#include "Data/AnimationData/AnimationData.h"
#include "Handle/Handle.h"

namespace Engine
{
	class AnimationResource
	{
	public:

		/// @brief コンストラクタ
		/// @param directory 
		/// @param fileName 
		AnimationResource(const std::string& directory, const std::string& fileName, AnimationHandle handle);

		/// @brief ハンドルを取得する
		/// @return 
		AnimationHandle GetHandle()const { return handle_; }

		/// @brief ファイルパスを取得する
		/// @return 
		std::string GetFilePath()const { return filePath_; }

		/// @brief アニメーションを取得する
		/// @return 
		Animation GetAnimation()const { return animation_; }


	private:

		// アニメーション
		Animation animation_{};


		// ハンドル
		AnimationHandle handle_ = 0;

		// ファイルパス
		std::string filePath_{};
	};
}