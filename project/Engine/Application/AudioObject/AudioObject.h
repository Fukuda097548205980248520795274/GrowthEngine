#pragma once
#include "Handle/Handle.h"
#include <string>


class GrowthEngine;

class AudioObject
{
public:
	
	/// @brief コンストラクタ
	/// @param filePath 
	/// @param volume 
	/// @param isLoop 
	AudioObject(const std::string& filePath, float volume, bool isLoop);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// オーディオを流す
	/// </summary>
	void PlayAudio();

	/// <summary>
	/// オーディオを止める
	/// </summary>
	void StopAudio();

	/// <summary>
	/// ピッチを設定する
	/// </summary>
	void SetPitch(float pitch);

	/// <summary>
	/// ボリュームを設定する
	/// </summary>
	/// <param name="volume"></param>
	void SetVolume(float volume);

	/// <summary>
	/// ループの設定
	/// </summary>
	/// <param name="isLoop"></param>
	void SetLoop(bool isLoop);


private:

	// エンジン
	const GrowthEngine* engine_ = nullptr;



	// ボリューム
	float volume_ = 0.0f;

	// ピッチ
	float pitch_ = 1.0f;

	// ループフラグ
	bool isLoop_ = false;


	// オーディオハンドル
	AudioHandle ah_;

	// プレイハンドル
	PlayHandle ph_;


	// 再生するかどうか
	bool isPlay_ = false;
};

