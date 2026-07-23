#include "PostEffectLuminanceBasedOutlineData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectLuminanceBasedOutlineData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::LuminanceBasedOutline>();
	param_->colorThreshold = 0.1f;
	param_->outlineWidth = 1.0f;

	// パラメータを記録する
	group_ = "LuminanceBasedOutline_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "ColorThreshold", &param_->colorThreshold);
		parameter_->SetValue(group_, "OutlineWidth", &param_->outlineWidth);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::LuminanceBasedOutlineDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->colorThreshold = param_->colorThreshold;
	resource_->data_->outlineWidth = param_->outlineWidth;
	resource_->data_->screenResolution =
		Vector2(static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));
}

/// @brief リセット
void Engine::PostEffectLuminanceBasedOutlineData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->colorThreshold = 0.1f;
		param_->outlineWidth = 1.0f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectLuminanceBasedOutlineData::Register(const PostEffectRenderContext& context)
{
	ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;
	OffscreenResource* outlineTextureResource = context.outlineTextureResource;

	// データを渡す
	resource_->data_->colorThreshold = param_->colorThreshold;
	resource_->data_->outlineWidth = param_->outlineWidth;
	resource_->data_->screenResolution =
		Vector2(static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()), static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight()));

	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// アウトラインテクスチャの設定
	outlineTextureResource->RegisterGraphics(commandList, 1);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 2);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectLuminanceBasedOutlineData::DebugParameter()
{
#ifdef DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_LuminanceBasedOutline").c_str()))
	{
		ImGui::DragFloat("ColorThreshold", &param_->colorThreshold, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("OutlineWidth", &param_->outlineWidth, 0.01f, 0.0f, 10.0f);

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