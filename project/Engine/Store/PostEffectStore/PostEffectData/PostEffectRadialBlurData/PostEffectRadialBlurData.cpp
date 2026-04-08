#include "PostEffectRadialBlurData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectRadialBlurData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;

	// パラメータの生成
	param_ = std::make_unique<PostEffect::RadialBlur>();
	param_->center = Vector2(0.5f, 0.5f);
	param_->samples = 24;
	param_->power = 0.2f;

	// パラメータを記録する
	group_ = "RadialBlur_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Center", &param_->center);
		parameter_->SetValue(group_, "Samples", &param_->samples);
		parameter_->SetValue(group_, "Power", &param_->power);

		parameter_->RegisterGroupDataReflection(group_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::RadialBlurDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->center = param_->center;
	resource_->data_->samples = param_->samples;
	resource_->data_->power = param_->power;
}

/// @brief リセット
void Engine::PostEffectRadialBlurData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
	}
	else
	{
		param_->center = Vector2(0.5f, 0.5f);
		param_->samples = 10;
		param_->power = 0.01f;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectRadialBlurData::Register(const PostEffectRenderContext& context)
{
    ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;

	/*-----------------
	    データを渡す
	-----------------*/

	resource_->data_->center = param_->center;
	resource_->data_->samples = param_->samples;
	resource_->data_->power = param_->power;


	/*------------------------
	    コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->Register(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectRadialBlurData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// UV
	if (ImGui::TreeNode((name_ + "_RadialBlur").c_str()))
	{
		ImGui::DragFloat2("Center", &param_->center.x, 0.01f, -1.0f, 2.0f);
		ImGui::DragInt("Samples", &param_->samples, 1.0f, 0, 100);
		ImGui::DragFloat("Power", &param_->power, 0.01f, 0.0f, 5.0f);

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