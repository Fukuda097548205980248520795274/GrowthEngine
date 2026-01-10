#pragma once
#include <string>

namespace Engine
{
	/// <summary>
	/// string -> wstring
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstring -> string
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string ConvertString(const std::wstring& str);
}