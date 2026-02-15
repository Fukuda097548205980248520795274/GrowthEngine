#pragma once
#include <memory>
#include "AnimationResource/AnimationResource.h"

namespace Engine
{
	class AnimationStore
	{
	public:

		/// @brief 読み込む
		/// @param directory 
		/// @param fileName 
		AnimationHandle Load(const std::string& directory, const std::string& fileName);

		/// @brief アニメーションを取得する
		/// @param handle 
		/// @return 
		Animation GetAnimation(AnimationHandle handle)const { return dataTable_[handle]->GetAnimation(); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<AnimationResource>> dataTable_;
	};
}