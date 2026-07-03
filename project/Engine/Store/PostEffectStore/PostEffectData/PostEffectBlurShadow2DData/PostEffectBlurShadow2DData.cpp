#include "PostEffectBlurShadow2DData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectBlurShadow2DData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::BlurShadow2D>();
	param_->shadowColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	param_->shadowOffset = Vector2(0.01f, 0.01f);
	param_->blurSize = 1.0f;

	// パラメータを記録する
	group_ = "BlurShadow2D_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "ShadowColor", &param_->shadowColor);
		parameter_->SetValue(group_, "ShadowOffset", &param_->shadowOffset);
		parameter_->SetValue(group_, "BlurSize", &param_->blurSize);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::BlurShadow2DDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->shadowColor = param_->shadowColor;
	resource_->data_->shadowOffset = param_->shadowOffset;
	resource_->data_->blurSize = param_->blurSize;
}

/// @brief リセット
void Engine::PostEffectBlurShadow2DData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->shadowColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		param_->shadowOffset = Vector2(0.01f, 0.01f);
		param_->blurSize = 1.0f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectBlurShadow2DData::Register(const PostEffectRenderContext& context)
{
	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;

	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->shadowColor = param_->shadowColor;
	resource_->data_->shadowOffset = param_->shadowOffset;
	resource_->data_->blurSize = param_->blurSize;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectBlurShadow2DData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_BlurShadow2D").c_str()))
	{
		ImGui::ColorEdit4("ShadowColor", &param_->shadowColor.x);
		ImGui::DragFloat2("ShadowOffset", &param_->shadowOffset.x, 0.001f, -1.0f, 1.0f);
		ImGui::DragFloat("BlurSize", &param_->blurSize, 0.01f, 0.0f, 10.0f);

		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(group_);
			std::string message = std::format("{} : saved.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(group_);
			std::string message = std::format("{} : loaded.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// 終了
		ImGui::TreePop();
	}

#endif
}