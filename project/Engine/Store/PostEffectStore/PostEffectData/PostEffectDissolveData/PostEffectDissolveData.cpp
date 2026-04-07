#include "PostEffectDissolveData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "Store/TextureStore/TextureStore.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectDissolveData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso, TextureStore* textureStore)
{
	// nullptrチェック
	assert(device);
	assert(pso);
	assert(textureStore);

	// 引数を受け取る
	textureStore_ = textureStore;
	pso_ = pso;

	// エンジンのインスタンスを取得する
	const GrowthEngine* engine = GrowthEngine::GetInstance();

	// パラメータの生成
	param_ = std::make_unique<PostEffect::Dissolve>();
	param_->threshold = 0.5f;
	param_->edgeDiff = 0.03f;
	param_->edgeColor = Vector3(1.0f, 1.0f, 1.0f);
	param_->hMaskTexture = engine->LoadTexture("./Assets/Textures/noise0.png");

	// ファイルパスを取得する
	maskTextureFilePath_ = textureStore_->GetFilePath(param_->hMaskTexture);

	// パラメータを記録する
	group_ = "Dissolve_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "Threshold", &param_->threshold);
		parameter_->SetValue(group_, "Edge_Diff", &param_->edgeDiff);
		parameter_->SetValue(group_, "Edge_Color", &param_->edgeColor);
		parameter_->SetValue(group_, "MaskTexture_FilePath", &maskTextureFilePath_);

		parameter_->RegisterGroupDataReflection(group_);
		param_->hMaskTexture = engine->LoadTexture(maskTextureFilePath_);
	}

	// リソース生成
	resource_ = std::make_unique<ConstantBufferResource<PostEffect::DissolveDataForGPU>>();
	resource_->Initialize(device, log);
	resource_->data_->threshold = param_->threshold;
	resource_->data_->edgeDiff = param_->edgeDiff;
	resource_->data_->edgeColor = param_->edgeColor;
}

/// @brief リセット
void Engine::PostEffectDissolveData::Reset()
{
	// エンジンのインスタンスを取得する
	const GrowthEngine* engine = GrowthEngine::GetInstance();

	if (parameter_->IsFileFound(group_))
	{
		parameter_->RegisterGroupDataReflection(group_);
		param_->hMaskTexture = engine->LoadTexture(maskTextureFilePath_);
	}
	else
	{
		param_->threshold = 0.5f;
		param_->edgeDiff = 0.3f;
		param_->edgeColor = Vector3(1.0f, 1.0f, 1.0f);
		param_->hMaskTexture = engine->LoadTexture("./Assets/Textures/white2x2.png");
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectDissolveData::Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource,
	DepthResource* depthResource, const Matrix4x4& projectionInverse)
{
	(void)depthResource;
	(void)projectionInverse;

	/*-----------------
		データを渡す
	-----------------*/

	resource_->data_->threshold = param_->threshold;
	resource_->data_->edgeDiff = param_->edgeDiff;
	resource_->data_->edgeColor = param_->edgeColor;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenResource->Register(commandList, 0);

	// パラメータの設定
	resource_->RegisterGraphics(commandList, 1);

	// マスクテクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->hMaskTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectDissolveData::DebugParameter()
{
#ifdef _DEVELOPMENT

	if (ImGui::TreeNode((name_ + "_Dissolve").c_str()))
	{
		ImGui::DragFloat("Threshold", &param_->threshold, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Edge_Diff", &param_->edgeDiff, 0.01f, 0.0f, 1.0f);
		ImGui::ColorEdit3("Edge_Color", &param_->edgeColor.x);

		// テクスチャ
		ImGui::Text("\n");

		ImGui::ImageButton(
			textureStore_->GetFilePath(param_->hMaskTexture).c_str(),
			textureStore_->GetSrvGpuHandle(param_->hMaskTexture).ptr,
			ImVec2(32.0f, 32.0f),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
			ImVec4(1, 1, 1, 1)
		);

		// --- ドロップ処理 ---
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
			{
				int droppedIndex = *(const int*)payload->Data;

				// droppedIndex が dataTable_ の index
				// ここでマテリアルなどに設定する
				param_->hMaskTexture = static_cast<uint32_t>(droppedIndex);
				maskTextureFilePath_ = textureStore_->GetFilePath(param_->hMaskTexture);
			}
			ImGui::EndDragDropTarget();
		}

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