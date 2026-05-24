#pragma once
#include "GrowthEngine.h"

class SoundManager
{
public:

	/// @brief デストラクタ
	~SoundManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static SoundManager* GetInstance();


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = nullptr;


public:

	/// @brief 旋嵐スタイルチェンジのSE
	void SeStyleChangeSenran()const;

	/// @brief 撃鉄スタイルチェンジのSE
	void SeStyleChangeGekitetu()const;

	/// @brief 受け流しのSE
	void SeParried() { if (seParried_)seParried_->Play(); }

	/// @brief 弾きのSE
	void SeDeflect() { if (seDeflect_)seDeflect_->Play(); }


	/// @brief 旋嵐スタイルのBGM
	std::unique_ptr<Sound> bgmStyleSenran_ = nullptr;

	/// @brief 撃鉄スタイルのBGM
	std::unique_ptr<Sound> bgmStyleGekitetu_ = nullptr;


private:


	/// @brief 旋嵐スタイルチェンジのSE
	std::unique_ptr<Sound> seStyleSenran0_ = nullptr;
	std::unique_ptr<Sound> seStyleSenran1_ = nullptr;
	std::unique_ptr<Sound> seStyleSenran2_ = nullptr;
	std::unique_ptr<Sound> seStyleSenran3_ = nullptr;
	std::unique_ptr<Sound> seStyleSenran4_ = nullptr;

	/// @brief 撃鉄スタイルチェンジのSE
	std::unique_ptr<Sound> seStyleGekitetu0_ = nullptr;
	std::unique_ptr<Sound> seStyleGekitetu1_ = nullptr;
	std::unique_ptr<Sound> seStyleGekitetu2_ = nullptr;
	std::unique_ptr<Sound> seStyleGekitetu3_ = nullptr;

	/// @brief 受け流しのSE
	std::unique_ptr<Sound> seParried_ = nullptr;

	/// @brief 弾きのSE
	std::unique_ptr<Sound> seDeflect_ = nullptr;


private:

	/// @brief インスタンス
	static std::unique_ptr<SoundManager> instance_;

	/// @brief 初期化
	void Initialize();

	// コピー禁止
	SoundManager() = default;
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
};

