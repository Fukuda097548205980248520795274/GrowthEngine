#pragma once
#include "GrowthEngine.h"

struct ModelElementData
{
	// モデル名
    std::string modelName;

	// モデルのディレクトリとファイル名
	std::string modelDirectory;
	std::string modelFileName;

	// アニメーションのディレクトリとファイル名
	std::string animationDirectory;
	std::string animationFileName;

	// スケルトンのディレクトリとファイル名
	std::string skeletonDirectory;
	std::string skeletonFileName;

	// 3D描画の種類
	Engine::Render3D::Type type = Engine::Render3D::Type::None;

	// 3D描画オブジェクト
    std::unique_ptr<Engine::BaseRender3D> render3D;
};