#pragma once
#include "D3D12Core/D3D12Core.h"
#include <memory>

namespace Engine
{
	class Log;

	class RenderContext
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(Log* log);


	private:

		// D3D12Core
		std::unique_ptr<D3D12Core> core_ = nullptr;
	};
}