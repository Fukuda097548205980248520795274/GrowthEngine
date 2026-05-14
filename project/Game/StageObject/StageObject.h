#pragma once


class StageObject
{
public:

	// ステージオブジェクトのタグ
	enum class StageObjectTag
	{
		None,
		Floor,
		Wall
	};


public:

	/// @brief ステージオブジェクトのタグを取得する
	/// @return 
	StageObjectTag GetTag() const { return tag_; }



protected:

	// ステージオブジェクトのタグ
	StageObjectTag tag_ = StageObjectTag::None;
};

