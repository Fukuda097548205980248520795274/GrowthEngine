#include "ModelEditor.h"
#include "ModelSerializer/ModelSerializer.h"

/// @brief コンストラクタ
ModelEditor::ModelEditor()
{
	// モデルフォルダを走査してロードする
	RefreshModelList();
	RefreshAnimationList();
}

/// @brief 描画処理
void ModelEditor::Draw()
{
	// 3D描画
	for (auto& elem : modelElements_)
	{
		if (elem.render3D)
		{
			elem.render3D->Draw();
		}
	}
}

/// @brief UIを描画する
void ModelEditor::DrawUI()
{
#ifdef DEVELOPMENT

	// ImGuiのIOを取得
	ImGuiIO& io = ImGui::GetIO();
	bool isCtrl = io.KeyCtrl;

	// テキスト入力中やアイテムがアクティブな場合には無効にする
	if (!ImGui::GetIO().WantTextInput && !ImGui::IsAnyItemActive())
	{
		// Ctrl + Z で Undo
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			Undo();
		}

		// Ctrl + Y で Redo
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
		{
			Redo();
		}

		// Ctrl + S で 保存
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			Save();
		}

		// Deleteキーで選択中のスプライトを削除
		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			DeleteSelectedElement();
		}
	}

	DrawHierarchyWindow();
	DrawInspectorWindow();
	DrawAssetsWindow();
#endif
}

/// @brief モデルフォルダを走査してロードする
void ModelEditor::RefreshModelList()
{
	// 既存のリストとマップをクリア
	loadedModels_.clear();
	modelNames_.clear();

	// フォルダが存在しない場合は作成して終了
	if (!std::filesystem::exists(kModelDir))
	{
		std::filesystem::create_directories(kModelDir);
		return;
	}

	// フォルダ内を走査
	for (const auto& entry : std::filesystem::directory_iterator(kModelDir))
	{
		if (entry.is_directory())
		{
			// フォルダ名（これがモデル名になる想定）
			std::string modelName = entry.path().filename().string();

			// パスの区切り文字を統一
			std::string dirPath = entry.path().generic_string();

			// 期待されるファイル名
			std::string gltfFile = modelName + ".gltf";
			std::string objFile = modelName + ".obj";

			std::string targetFile = "";

			// .gltf があれば優先し、なければ .obj を探す
			if (std::filesystem::exists(dirPath + "/" + gltfFile))
			{
				targetFile = gltfFile;
			}
			else if (std::filesystem::exists(dirPath + "/" + objFile))
			{
				targetFile = objFile;
			}

			// 対象のモデルファイルが見つかった場合
			if (!targetFile.empty())
			{
				// ハンドルを取得する
				ModelHandle handle = GrowthEngine::GetInstance()->LoadModel(dirPath, targetFile);

				// 読み込んだ情報をマップとリストに保存
				loadedModels_[{dirPath, targetFile}] = handle;
				modelNames_.push_back(modelName);
			}
		}
	}
}

/// @brief アニメーションフォルダを走査してロードする
void ModelEditor::RefreshAnimationList()
{
	// 既存のリストとマップをクリア
	loadedAnimations_.clear();
	animationNames_.clear();

	// フォルダが存在しない場合は作成して終了
	if (!std::filesystem::exists(kModelDir))
	{
		std::filesystem::create_directories(kModelDir);
		return;
	}

	// フォルダ内を走査
	for (const auto& entry : std::filesystem::directory_iterator(kModelDir))
	{
		if (entry.is_directory())
		{
			// フォルダ名（これがモデル名になる想定）
			std::string modelName = entry.path().filename().string();

			// パスの区切り文字を統一
			std::string dirPath = entry.path().generic_string();

			// 期待されるファイル名
			std::string gltfFile = modelName + ".gltf";

			std::string targetFile = "";

			// .gltf があれば優先し、なければ .obj を探す
			if (std::filesystem::exists(dirPath + "/" + gltfFile))
			{
				targetFile = gltfFile;
			}

			// 対象のモデルファイルが見つかった場合
			if (!targetFile.empty())
			{
				// ハンドルを取得する
				AnimationHandle hAnimation = GrowthEngine::GetInstance()->LoadAnimation(dirPath, targetFile);
				SkeletonHandle hSkeleton = GrowthEngine::GetInstance()->LoadSkeleton(dirPath, targetFile);

				// 読み込んだ情報をマップとリストに保存
				loadedAnimations_[{dirPath, targetFile}] = hAnimation;
				animationNames_.push_back(modelName);

				loadedSkeletons_[{dirPath, targetFile}] = hSkeleton;
				skeletonNames_.push_back(modelName);
			}
		}
	}
}

/// @brief ヒエラルキーウィンドウ描画
void ModelEditor::DrawHierarchyWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("モデル - ヒエラルキー"))
	{
		for (int i = 0; i < modelElements_.size(); ++i)
		{
			bool isSelected = (selectedElementIndex_ == i);
			if (ImGui::Selectable(modelElements_[i].modelName.c_str(), isSelected))
			{
				selectedElementIndex_ = i;
			}
		}
	}
	ImGui::End();

#endif
}

/// @brief インスペクターウィンドウ描画
void ModelEditor::DrawInspectorWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("モデル - インスペクター"))
	{
		if (selectedElementIndex_ >= 0 && selectedElementIndex_ < modelElements_.size())
		{
			// 選択中のモデルデータを取得
			auto& selectedData = modelElements_[selectedElementIndex_];
			if (!selectedData.render3D)
			{
				ImGui::End();
				return;
			}

			// 名前の変更
			char nameBuffer[64];
			strcpy_s(nameBuffer, selectedData.modelName.c_str());

			// Enterキーが押されたか、入力が終了したかを判定
			bool isNameEntered = ImGui::InputText("名前", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

			// 名前が変更された場合、または入力が終了した場合に処理を行う
			if (isNameEntered || ImGui::IsItemDeactivatedAfterEdit())
			{
				std::string newName(nameBuffer);
				if (newName != selectedData.modelName)
				{
					SaveHistoryState();

					// 自分自身のインデックスを除外して重複チェック
					selectedData.modelName = GetUniqueName(newName, selectedElementIndex_);
				}
			}


			if (selectedData.render3D->GetType() == Engine::Render3D::Type::StaticModel)
			{
				ImGui::Text("モデルタイプ: StaticModel");
				auto staticModel = static_cast<Render3DStaticModel*>(selectedData.render3D.get());
				auto param = staticModel->param_;

				for (int i = 0; i < static_cast<int>(param->meshTransforms.size()); ++i)
				{
					if (ImGui::TreeNode(std::format("メッシュ {}", i).c_str()))
					{
						BlenderInspectorUI(&param->blendMode);
						TransformInspectorUI(&param->meshTransforms[i]);
						MaterialInspectorUI(&param->meshMaterial[i]);
						BlurInspectorUI(&param->meshBlur[i]);
						OutlineInspectorUI(&param->meshOutline[i]);

						ImGui::TreePop();
					}
				}
			}
			else if (selectedData.render3D->GetType() == Engine::Render3D::Type::AnimationModel)
			{
				ImGui::Text("モデルタイプ: AnimationModel");
				auto animationModel = static_cast<Render3DAnimationModel*>(selectedData.render3D.get());
				auto param = animationModel->param_;

				for (int i = 0; i < static_cast<int>(param->meshTransforms.size()); ++i)
				{
					if (ImGui::TreeNode(std::format("メッシュ {}", i).c_str()))
					{
						BlenderInspectorUI(&param->blendMode);
						TransformInspectorUI(&param->meshTransforms[i]);
						MaterialInspectorUI(&param->meshMaterial[i]);
						BlurInspectorUI(&param->meshBlur[i]);
						OutlineInspectorUI(&param->meshOutline[i]);

						ImGui::TreePop();
					}
				}

				// アニメーションの編集
				ImGui::DragFloat("アニメーションタイマー", &param->animation.timer, 0.01f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}
			else if (selectedData.render3D->GetType() == Engine::Render3D::Type::SkinningModel)
			{
				ImGui::Text("モデルタイプ: SkinningModel");
				auto skinningModel = static_cast<Render3DSkinningModel*>(selectedData.render3D.get());
				auto param = skinningModel->param_;

				for (int i = 0; i < static_cast<int>(param->meshTransforms.size()); ++i)
				{
					if (ImGui::TreeNode(std::format("メッシュ {}", i).c_str()))
					{
						BlenderInspectorUI(&param->blendMode);
						TransformInspectorUI(&param->meshTransforms[i]);
						MaterialInspectorUI(&param->meshMaterial[i]);
						BlurInspectorUI(&param->meshBlur[i]);
						OutlineInspectorUI(&param->meshOutline[i]);

						ImGui::TreePop();
					}
				}

				// アニメーションの編集
				ImGui::DragFloat("アニメーションタイマー", &param->animation.timer, 0.01f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}
			else if (selectedData.render3D->GetType() == Engine::Render3D::Type::UVSphere)
			{
				ImGui::Text("モデルタイプ: UVSphere");
				auto uvSphereModel = static_cast<Render3DUVSphere*>(selectedData.render3D.get());
				auto param = uvSphereModel->param_;

				BlenderInspectorUI(&param->blendMode);
				TransformInspectorUI(&param->transform);
				MaterialInspectorUI(&param->material);

				if (ImGui::TreeNode("分割"))
				{
					ImGui::DragInt("スライス", &param->division.slices, 1, 3, 32);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragInt("リング", &param->division.rings, 1, 3, 16);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::TreePop();
				}

				BlurInspectorUI(&param->blur);
				OutlineInspectorUI(&param->outline);
			}
			else if (selectedData.render3D->GetType() == Engine::Render3D::Type::Ring)
			{
				ImGui::Text("モデルタイプ: Ring");
				auto ringModel = static_cast<Render3DRing*>(selectedData.render3D.get());
				auto param = ringModel->param_;

				BlenderInspectorUI(&param->blendMode);
				TransformInspectorUI(&param->transform);
				MaterialInspectorUI(&param->material);

				if (ImGui::TreeNode("分割"))
				{
					ImGui::DragInt("スライス", &param->division.slices, 1, 3, 32);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("サイズ"))
				{
					ImGui::DragFloat("最初の内半径", &param->size.startInRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("最初の外半径", &param->size.startOutRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("最初の角度", &param->size.startAngle, 0.01f, 0.0f, 360.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("最後の内半径", &param->size.endInRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("最後の外半径", &param->size.endOutRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("最後の角度", &param->size.endAngle, 0.01f, 0.0f, 360.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::TreePop();
				}


				BlurInspectorUI(&param->blur);
				OutlineInspectorUI(&param->outline);
			}
			else if (selectedData.render3D->GetType() == Engine::Render3D::Type::Cylinder)
			{
				ImGui::Text("モデルタイプ: Cylinder");
				auto cylinderModel = static_cast<Render3DCylinder*>(selectedData.render3D.get());
				auto param = cylinderModel->param_;

				BlenderInspectorUI(&param->blendMode);
				TransformInspectorUI(&param->transform);
				MaterialInspectorUI(&param->material);

				if (ImGui::TreeNode("分割"))
				{
					ImGui::DragInt("スライス", &param->division.slices, 1, 3, 32);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("サイズ"))
				{
					ImGui::DragFloat("上半径", &param->size.topRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("下半径", &param->size.bottomRadius, 0.01f, 0.0f, 1000.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::DragFloat("高さ", &param->size.height, 0.01f, 0.0f, 360.0f);
					if (ImGui::IsItemActivated()) SaveHistoryState();

					ImGui::TreePop();
				}

				BlurInspectorUI(&param->blur);
				OutlineInspectorUI(&param->outline);
			}


			ImGui::Separator();
			ImGui::Text("--- 描画順（レイヤー） ---");

			// ひとつ前（奥）に移動
			if (ImGui::Button("上へ移動") && selectedElementIndex_ > 0)
			{
				SaveHistoryState();
				std::swap(modelElements_[selectedElementIndex_], modelElements_[selectedElementIndex_ - 1]);
				selectedElementIndex_--;
			}
			ImGui::SameLine();

			// ひとつ後ろ（手前）に移動
			if (ImGui::Button("下へ移動") && selectedElementIndex_ < modelElements_.size() - 1)
			{
				SaveHistoryState();
				std::swap(modelElements_[selectedElementIndex_], modelElements_[selectedElementIndex_ + 1]);
				selectedElementIndex_++;
			}
			ImGui::SameLine();

			// スプライトの削除
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("削除"))
			{
				SaveHistoryState();
				DeleteSelectedElement();
			}
			ImGui::PopStyleColor();
		}
		else
		{
			// 何も選択されていない時の案内表示
			ImGui::Text("ヒエラルキーからUI要素を選択してください。");
		}
	}
	ImGui::End();

#endif
}

/// @brief アセットウィンドウ描画
void ModelEditor::DrawAssetsWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("モデル - アセットブラウザ"))
	{
		// 新規ファイル作成ボタン
		if (ImGui::Button("新規ファイル作成"))
		{
			// ポップアップを開く前に入力欄にデフォルト名を入れておく
			strcpy_s(inputFilename_, "model_new");
			ImGui::OpenPopup("新規ファイル作成ポップアップ");
		}

		if (ImGui::BeginPopupModal("新規ファイル作成ポップアップ", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("ファイル名", inputFilename_, IM_ARRAYSIZE(inputFilename_));

			ImGui::Separator();

			// 作成ボタン
			if (ImGui::Button("作成", ImVec2(120, 0)))
			{
				if (strlen(inputFilename_) > 0)
				{
					// 既存のデータをクリア
					modelElements_.clear();
					selectedElementIndex_ = -1;

					// 入力された名前を「今開いているファイル名」として保持
					currentFileName_ = inputFilename_;
					isFileOpen_ = true;

					// 保持した名前で空のファイルを作成
					Save();

					ImGui::CloseCurrentPopup(); // ポップアップを閉じる
				}
			}

			ImGui::SameLine();

			// キャンセルボタン
			if (ImGui::Button("キャンセル", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup(); // 何もせずに閉じる
			}

			ImGui::EndPopup();
		}


		// ファイルを開くボタン
		if (isFileOpen_)
		{
			ImGui::SameLine();
			ImGui::Text("編集中: %s.json", currentFileName_.c_str());
			ImGui::Separator();

			// 保存ボタン
			if (ImGui::Button("保存"))
				Save();

			ImGui::SameLine();

			if (ImGui::Button("モデル再読み込み"))
			{
				RefreshModelList();
				RefreshAnimationList();
			}
			ImGui::SameLine();


			static char newModelName[64] = "New Model";
			static int newModelTypeIndex = 0; // モデルの種類のインデックス
			static int newModelIndex = 0;
			static int newAnimationIndex = 0;
			static int newSkeletonIndex = 0;

			// 新規モデル追加ボタン
			if (ImGui::Button("新規モデル追加"))
			{
				// ポップアップを開く前に初期値をセット
				strcpy_s(newModelName, "New Model");
				newModelIndex = 0;
				ImGui::OpenPopup("新規モデル作成");
			}

			// 新規モデル作成のポップアップ
			if (ImGui::BeginPopupModal("新規モデル作成", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				// 名前の入力
				ImGui::InputText("名前", newModelName, IM_ARRAYSIZE(newModelName));

				// モデルの選択
				const char* modelTypes[] = { "None", "StaticModel", "AnimationModel", "SkinningModel", "UVSphere", "Ring", "Cylinder" };
				ImGui::Combo("モデルの種類", &newModelTypeIndex, modelTypes, IM_ARRAYSIZE(modelTypes));

				// モデルの種類が StaticModel, AnimationModel, SkinningModel の場合のみ、モデルの選択コンボボックスを表示
				if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::StaticModel) ||
					newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::AnimationModel) ||
					newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::SkinningModel))
				{
					// モデルの選択コンボボックス
					if (!modelNames_.empty())
					{
						std::vector<const char*> items;
						for (const auto& name : modelNames_)
						{
							items.push_back(name.c_str());
						}
						ImGui::Combo("モデル", &newModelIndex, items.data(), static_cast<int>(items.size()));
					} else
					{
						ImGui::TextDisabled("※モデルが見つかりません");
					}
				}

				// モデルの種類が AnimationModel, SkinningModel の場合のみ、アニメーションの選択コンボボックスを表示
				if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::AnimationModel) ||
					newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::SkinningModel))
				{
					// モデルの選択コンボボックス
					if (!animationNames_.empty())
					{
						std::vector<const char*> items;
						for (const auto& name : animationNames_)
						{
							items.push_back(name.c_str());
						}
						ImGui::Combo("アニメーション", &newAnimationIndex, items.data(), static_cast<int>(items.size()));
					} else
					{
						ImGui::TextDisabled("※アニメーションが見つかりません");
					}
				}

				// モデルの種類が SkinningModel の場合のみ、スケルトンの選択コンボボックスを表示
				if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::SkinningModel))
				{
					// モデルの選択コンボボックス
					if (!skeletonNames_.empty())
					{
						std::vector<const char*> items;
						for (const auto& name : skeletonNames_)
						{
							items.push_back(name.c_str());
						}
						ImGui::Combo("スケルトン", &newSkeletonIndex, items.data(), static_cast<int>(items.size()));
					} else
					{
						ImGui::TextDisabled("※スケルトンが見つかりません");
					}
				}

				ImGui::Separator();

				// 作成ボタン
				if (newModelTypeIndex != static_cast<int>(Engine::Render3D::Type::None) && ImGui::Button("作成", ImVec2(120, 0)))
				{
					SaveHistoryState();

					ModelElementData newData;
					newData.modelName = GetUniqueName(newModelName);

					// 選択されたモデルの名前から、ディレクトリとファイル名を特定してハンドルを取得
					if (!modelNames_.empty() && newModelIndex >= 0 && newModelIndex < modelNames_.size())
					{
						if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::StaticModel))
						{
							ModelHandle hModel = 0;
							std::string selectedModelFile = modelNames_[newModelIndex];

							// モデルのハンドルを取得
							for (auto it = loadedModels_.begin(); it != loadedModels_.end(); ++it)
							{
								if (it->first.second == selectedModelFile)
								{
									newData.modelDirectory = it->first.first;
									newData.modelFileName = it->first.second;
									hModel = it->second;
									break;
								}
							}

							newData.type = Engine::Render3D::Type::StaticModel;
							newData.render3D = std::make_unique<Render3DStaticModel>(hModel, newData.modelName);
						} else if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::AnimationModel))
						{
							ModelHandle hModel = 0;
							AnimationHandle hAnimation = 0;
							std::string selectedModelFile = modelNames_[newModelIndex];
							std::string selectedAnimationFile = modelNames_[newAnimationIndex];

							// モデルのハンドルを取得
							for (auto it = loadedModels_.begin(); it != loadedModels_.end(); ++it)
							{
								if (it->first.second == selectedModelFile)
								{
									newData.modelDirectory = it->first.first;
									newData.modelFileName = it->first.second;
									hModel = it->second;
									break;
								}
							}

							// アニメーションのハンドルを取得
							for (auto it = loadedAnimations_.begin(); it != loadedAnimations_.end(); ++it)
							{
								if (it->first.second == selectedAnimationFile)
								{
									newData.animationDirectory = it->first.first;
									newData.animationFileName = it->first.second;
									hAnimation = it->second;
									break;
								}
							}

							newData.type = Engine::Render3D::Type::AnimationModel;
							newData.render3D = std::make_unique<Render3DAnimationModel>(hModel, hAnimation, newData.modelName);
						} else if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::SkinningModel))
						{
							ModelHandle hModel = 0;
							AnimationHandle hAnimation = 0;
							SkeletonHandle hSkeleton = 0;
							std::string selectedModelFile = modelNames_[newModelIndex];
							std::string selectedAnimationFile = modelNames_[newAnimationIndex];
							std::string selectedSkeletonFile = modelNames_[newSkeletonIndex];

							// モデルのハンドルを取得
							for (auto it = loadedModels_.begin(); it != loadedModels_.end(); ++it)
							{
								if (it->first.second == selectedModelFile)
								{
									newData.modelDirectory = it->first.first;
									newData.modelFileName = it->first.second;
									hModel = it->second;
									break;
								}
							}

							// アニメーションのハンドルを取得
							for (auto it = loadedAnimations_.begin(); it != loadedAnimations_.end(); ++it)
							{
								if (it->first.second == selectedAnimationFile)
								{
									newData.animationDirectory = it->first.first;
									newData.animationFileName = it->first.second;
									hAnimation = it->second;
									break;
								}
							}

							// スケルトンのハンドルを取得
							for (auto it = loadedSkeletons_.begin(); it != loadedSkeletons_.end(); ++it)
							{
								if (it->first.second == selectedSkeletonFile)
								{
									newData.skeletonDirectory = it->first.first;
									newData.skeletonFileName = it->first.second;
									hSkeleton = it->second;
									break;
								}
							}

							newData.type = Engine::Render3D::Type::SkinningModel;
							newData.render3D = std::make_unique<Render3DSkinningModel>(hModel, hAnimation, hSkeleton, newData.modelName);
						}
					}

					// モデルの種類が UVSphere, Ring, Cylinder の場合は、特定のハンドルは不要
					if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::UVSphere))
					{
						newData.type = Engine::Render3D::Type::UVSphere;
						newData.render3D = std::make_unique<Render3DUVSphere>(newData.modelName);
					} else if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::Ring))
					{
						newData.type = Engine::Render3D::Type::Ring;
						newData.render3D = std::make_unique<Render3DRing>(newData.modelName);
					} else if (newModelTypeIndex == static_cast<int>(Engine::Render3D::Type::Cylinder))
					{
						newData.type = Engine::Render3D::Type::Cylinder;
						newData.render3D = std::make_unique<Render3DCylinder>(newData.modelName);
					}

					modelElements_.push_back(std::move(newData));
					selectedElementIndex_ = static_cast<int>(modelElements_.size()) - 1;

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				// キャンセルボタン
				if (ImGui::Button("キャンセル", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup(); // 何もせずに閉じる
				}

				ImGui::EndPopup();
			}
		}


		if (std::filesystem::exists(kModelDataDir))
		{
			// ウィンドウの右端の座標を取得（折り返しの計算用）
			float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			// ディレクトリ内のファイルを走査
			for (const auto& entry : std::filesystem::directory_iterator(kModelDataDir))
			{
				// .json ファイルのみを対象とする
				if (entry.path().extension() == ".json")
				{
					std::string filename = entry.path().filename().string();
					std::string path = entry.path().string();

					// ボタンとして表示（50x50のサイズ）
					if (ImGui::Button(filename.c_str(), ImVec2(50, 50)))
					{
						// ファイルを読み込む前に、現在の状態を履歴に保存
						SaveHistoryState();

						selectedElementIndex_ = -1;
						modelElements_ = FromJson(path, loadedModels_, loadedAnimations_, loadedSkeletons_);

						// ファイル名から拡張子を除いた名前を取得して、保存用の入力欄にセット
						std::string nameWithoutExt = entry.path().stem().string();
						strcpy_s(inputFilename_, nameWithoutExt.c_str());

						// 現在開いているファイル名を設定
						currentFileName_ = nameWithoutExt;
						isFileOpen_ = true;
					}

					// 次のアイテムを描画した時にウィンドウの右端をはみ出さないか計算
					float lastItemMaxX = ImGui::GetItemRectMax().x;
					float nextItemMaxX = lastItemMaxX + style.ItemSpacing.x + 100.0f;

					// はみ出さない場合は横に並べる（SameLine）、はみ出す場合はそのまま（自動で改行される）
					if (nextItemMaxX < windowVisibleX)
					{
						ImGui::SameLine();
					}
				}
			}
		}
		else
		{
			ImGui::TextDisabled("ディレクトリが見つかりません: %s", kModelDataDir.c_str());
		}
	}
	ImGui::End();

#endif
}

/// @brief UI要素の名前が重複しないようにユニークな名前を生成する
/// @param baseName 
/// @param ignoreIndex 
/// @return 
std::string ModelEditor::GetUniqueName(const std::string& baseName, int ignoreIndex) const
{
	std::string currentName = baseName;
	int count = 1;

	while (true)
	{
		bool isDuplicate = false;

		for (int i = 0; i < modelElements_.size(); ++i)
		{
			// 自分自身は重複チェックから除外する
			if (i == ignoreIndex) continue;

			// 名前が重複しているかチェック
			if (modelElements_[i].modelName == currentName)
			{
				isDuplicate = true;
				break;
			}
		}

		// 重複がなければその名前で確定
		if (!isDuplicate)
			break;

		// 重複している場合は _1, _2 のように連番を付与して再チェック
		currentName = baseName + "_" + std::to_string(count);
		count++;
	}

	return currentName;
}

/// @brief UIデータをファイルに保存する
void ModelEditor::Save()
{
	// ファイル名が空の場合は保存しない
	if (currentFileName_.empty()) return;

	// 保存先ディレクトリが存在しない場合は作成する
	if (!std::filesystem::exists(kModelDataDir))
		std::filesystem::create_directories(kModelDataDir);

	// 保存するファイル名に .json 拡張子が含まれていない場合は追加する
	std::string filenameStr = currentFileName_;
	if (filenameStr.find(".json") == std::string::npos)
		filenameStr += ".json";
	
	std::string path = kModelDataDir + filenameStr;
	ToJson(path, modelElements_);
}

/// @brief 選択中のUI要素を削除する
void ModelEditor::DeleteSelectedElement()
{
	if (selectedElementIndex_ >= 0 && selectedElementIndex_ < modelElements_.size())
	{
		SaveHistoryState();
		modelElements_.erase(modelElements_.begin() + selectedElementIndex_);
		selectedElementIndex_ = -1; // 選択解除
	}
}

/// @brief 現在の状態を履歴に保存する
void ModelEditor::SaveHistoryState()
{
	// 現在のリストの状態をJSON化してUndoスタックに積む
	undoStack_.push_back(ToJsonData(modelElements_));

	// 新しい操作をしたのでRedoスタックはクリアする
	redoStack_.clear();

	// 履歴の上限を超えたら古いものから削除
	if (undoStack_.size() > 50)
	{
		undoStack_.erase(undoStack_.begin());
	}
}

/// @brief 元に戻す
void ModelEditor::Undo()
{
	if (undoStack_.empty()) return;

	// 現在の状態をRedoスタックに退避
	redoStack_.push_back(ToJsonData(modelElements_));

	// Undoスタックの最新のデータを復元
	modelElements_ = FromJsonData(undoStack_.back(), loadedModels_, loadedAnimations_, loadedSkeletons_);
	undoStack_.pop_back();

	// 選択インデックスが範囲外にならないよう調整
	if (selectedElementIndex_ >= modelElements_.size()) selectedElementIndex_ = -1;
}

/// @brief やり直し
void ModelEditor::Redo()
{
	if (redoStack_.empty()) return;

	// 現在の状態をUndoスタックに退避
	undoStack_.push_back(ToJsonData(modelElements_));

	// Redoスタックの最新のデータを復元
	modelElements_ = FromJsonData(redoStack_.back(), loadedModels_, loadedAnimations_, loadedSkeletons_);
	redoStack_.pop_back();

	if (selectedElementIndex_ >= modelElements_.size()) selectedElementIndex_ = -1;
}

/// @brief ブレンドモードのインスペクターウィンドウ描画
/// @param blendMode 
void ModelEditor::BlenderInspectorUI(BlendMode* blendMode)
{
	const char* blendModeNames[] = { "なし", "ノーマル", "加算", "減算", "乗算", "スクリーン" };
	ImGui::Combo("ブレンドモード", reinterpret_cast<int*>(blendMode), blendModeNames, IM_ARRAYSIZE(blendModeNames));
}

/// @brief トランスフォームのインスペクターウィンドウ描画
/// @param transform 
void ModelEditor::TransformInspectorUI(Engine::Render3D::Transform* transform)
{
	if (ImGui::TreeNode("トランスフォーム"))
	{
		// トランスフォームの編集
		ImGui::DragFloat3("位置", &transform->translate.x, 1.0f);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		ImGui::DragFloat3("大きさ", &transform->scale.x, 0.01f);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		ImGui::DragFloat3("回転", &transform->rotate.x, 0.01f);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		ImGui::TreePop();
	}
}

/// @brief マテリアルのインスペクターウィンドウ描画
/// @param material 
void ModelEditor::MaterialInspectorUI(Engine::Render3D::Material* material)
{
	if (ImGui::TreeNode("マテリアル"))
	{
		ImGui::Text("テクスチャ");

		// ドロップ先のターゲットとなるUI（ボタン等をプレースホルダーとして使用）
		ImGui::Button(std::format("Handle: {}", material->hTexture).c_str(), ImVec2(150, 30));

		// ドラッグ＆ドロップの受け取り処理
		if (ImGui::BeginDragDropTarget())
		{
			// "TEXTURE_ID" という名前のペイロードを受け入れる
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));

				// ペイロードからインデックス（テクスチャハンドル）を取得
				int textureIndex = *(const int*)payload->Data;

				// マテリアルにテクスチャハンドルを設定
				material->hTexture = static_cast<TextureHandle>(textureIndex);

				// 履歴（Undo/Redo用）を保存
				SaveHistoryState();
			}
			ImGui::EndDragDropTarget();
		}
		

		ImGui::ColorEdit4("色 (Color)", &material->color.x);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		ImGui::Checkbox("ライティング有効", &material->enableLighting);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		ImGui::Checkbox("シャドウマップ描画", &material->drawShadowMap);
		if (ImGui::IsItemActivated()) SaveHistoryState();

		if (material->enableLighting)
		{
			ImGui::Checkbox("ディフューズ有効", &material->enableDiffuse);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			if (material->enableDiffuse)
			{
				ImGui::Checkbox("ハーフランバート有効", &material->enableHalfLambert);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}

			ImGui::Checkbox("スペキュラー有効", &material->enableSpecular);

			if (material->enableSpecular)
			{
				ImGui::Checkbox("ブリンフォン有効化", &material->enableBlinnPhong);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				ImGui::DragFloat("光沢度", &material->shininess, 0.1f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}

			ImGui::Checkbox("影", &material->enableShadow);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			ImGui::SliderFloat("環境", &material->environment, 0.0f, 1.0f);
			if (ImGui::IsItemActivated()) SaveHistoryState();
		}

		ImGui::TreePop();
	}
}

/// @brief ブラーのインスペクターウィンドウ描画
/// @param blur 
void ModelEditor::BlurInspectorUI(Engine::Render3D::Blur* blur)
{
	if (Engine::PostEffectStore::IsEnableMotionVector())
	{
		if (ImGui::TreeNode("ブラー"))
		{
			if (Engine::PostEffectStore::IsLoadAfterImage())
			{
				ImGui::DragFloat("残像", &blur->afterImageMask, 0.01f, 0.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}

			if (Engine::PostEffectStore::IsLoadMotionBlur())
			{
				ImGui::DragFloat("モーションブラー", &blur->motionBlurMask, 0.01f, 0.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}

			ImGui::TreePop();
		}
	}
}

/// @brief アウトラインのインスペクターウィンドウ描画
/// @param outline 
void ModelEditor::OutlineInspectorUI(Engine::Render3D::Outline* outline)
{
	// アウトライン描画
	if (Engine::PostEffectStore::IsLoadOutline())
	{
		if (ImGui::TreeNode("ブラー"))
		{
			// アウトライン描画
			ImGui::Checkbox("アウトライン描画", &outline->enableOutline);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			// アウトラインの色
			ImGui::ColorEdit4("アウトラインの色", &outline->color.x);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			ImGui::TreePop();
		}
	}
}