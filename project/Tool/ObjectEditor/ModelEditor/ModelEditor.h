#pragma once
#include "ModelData/ModelData.h"

class ModelEditor
{
public:

	/// @brief コンストラクタ
	ModelEditor();

	/// @brief 読み込む
	/// @param fileName 
	void Load(const std::string& fileName);

	/// @brief 描画処理
	void Draw();

	/// @brief UIを描画する
	void DrawUI();

	/// @brief モデルを取得する
	/// @tparam T 
	/// @param name 
	/// @return 
	template<typename T>
	T* GetModel(const std::string& name)
	{
		for (auto& elem : modelElements_)
		{
			if (elem.modelName == name)
			{
				return static_cast<T*>(elem.render3D.get());
			}
		}

		return nullptr;
	}

private:

	/// @brief モデルフォルダを走査してロードする
	void RefreshModelList();

	/// @brief アニメーションフォルダを走査してロードする
	void RefreshAnimationList();


private:

	/// @brief ヒエラルキーウィンドウ描画
	void DrawHierarchyWindow();

	/// @brief インスペクターウィンドウ描画
	void DrawInspectorWindow();

	/// @brief アセットウィンドウ描画
	void DrawAssetsWindow();


private:

	/// @brief UI要素の名前が重複しないようにユニークな名前を生成する
	/// @param baseName 
	/// @param ignoreIndex 
	/// @return 
	std::string GetUniqueName(const std::string& baseName, int ignoreIndex = -1) const;

	/// @brief UIデータをファイルに保存する
	void Save();

	/// @brief 選択中のUI要素を削除する
	void DeleteSelectedElement();

	/// @brief 現在の状態を履歴に保存する
	void SaveHistoryState();

	/// @brief 元に戻す
	void Undo();

	/// @brief やり直し
	void Redo();


private:

	/// @brief ブレンドモードのインスペクターウィンドウ描画
	/// @param blendMode 
	void BlenderInspectorUI(BlendMode* blendMode);

	/// @brief トランスフォームのインスペクターウィンドウ描画
	/// @param transform 
	void TransformInspectorUI(Engine::Render3D::Transform* transform);

	/// @brief マテリアルのインスペクターウィンドウ描画
	/// @param material 
	void MaterialInspectorUI(Engine::Render3D::Material* material);

	/// @brief ブラーのインスペクターウィンドウ描画
	/// @param blur 
	void BlurInspectorUI(Engine::Render3D::Blur* blur);

	/// @brief アウトラインのインスペクターウィンドウ描画
	/// @param outline 
	void OutlineInspectorUI(Engine::Render3D::Outline* outline);


private:

	// 編集中のモデル要素リスト
	std::vector<ModelElementData> modelElements_;

	// 現在選択されているモデル要素のインデックス
	int selectedElementIndex_ = -1;

	// モデルデータを保存するディレクトリパス
	const std::string kModelDataDir = "./Assets/Parameter/Models/";


private:

	// ファイルを開いている状態かどうかのフラグ
	bool isFileOpen_ = false;

	// 現在実際に開いて編集しているファイル名
	std::string currentFileName_ = "";

	// UIの入力欄用
	char inputFilename_[128] = "";


private:

	// 読み込まれたモデルのハンドルを保持するマップ
	std::map<std::pair<std::string, std::string>, ModelHandle> loadedModels_;
	std::map<std::pair<std::string, std::string>, ModelHandle> loadedAnimations_;
	std::map<std::pair<std::string, std::string>, ModelHandle> loadedSkeletons_;

	// コンボボックス表示用のファイル名リスト
	std::vector<std::string> modelNames_;
	std::vector<std::string> animationNames_;
	std::vector<std::string> skeletonNames_;

	// モデルデータを保存するディレクトリパス
	const std::string kModelDir = "./Assets/Models/";


private:

	// undoスタック
	std::vector<nlohmann::json> undoStack_;

	// redoスタック
	std::vector<nlohmann::json> redoStack_;
};

