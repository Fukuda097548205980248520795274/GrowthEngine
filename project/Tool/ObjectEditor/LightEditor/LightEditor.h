#pragma once
#include "LightData/LightData.h"

class LightEditor
{
public:

	/// @brief コンストラクタ
	LightEditor() = default;

	/// @brief 読み込む
	/// @param filename 
	void Load(const std::string& filename);

	/// @brief UIを描画する
	void DrawUI();

	/// @brief 平行光源を取得する
	/// @param name 
	/// @return 
	LightDirectional* GetDirectionalLight(const std::string& name) const;

	/// @brief 点光源を取得する
	/// @param name 
	/// @return 
	LightPoint* GetPointLight(const std::string& name) const;

	/// @brief スポットライトを取得する
	/// @param name 
	/// @return 
	LightSpot* GetSpotLight(const std::string& name) const;


private:

	/// @brief ヒエラルキーウィンドウ描画
	void DrawHierarchyWindow();

	/// @brief インスペクターウィンドウ描画
	void DrawInspectorWindow();

	/// @brief アセットウィンドウ描画
	void DrawAssetsWindow();


private:

	/// @brief ライト要素の名前が重複しないようにユニークな名前を生成する
	/// @param baseName 
	/// @param ignoreIndex 
	/// @return 
	std::string GetUniqueName(const std::string& baseName, int ignoreIndex = -1) const;

	/// @brief ファイルに保存する
	void Save();

	/// @brief 選択中のライト要素を複製する
	void DuplicateSelectedElement();

	/// @brief 選択中のライト要素を削除する
	void DeleteSelectedElement();

	/// @brief 現在の状態を履歴に保存する
	void SaveHistoryState();

	/// @brief 元に戻す
	void Undo();

	/// @brief やり直し
	void Redo();


private:

	// 編集中のUI要素リスト
	std::vector<LightElementData> lightElements_;

	// 現在選択されているUI要素のインデックス
	int selectedElementIndex_ = -1;

	// UIデータを保存するディレクトリパス
	const std::string kLightDir = "./Assets/Parameter/Light/";


private:

	// ファイルを開いている状態かどうかのフラグ
	bool isFileOpen_ = false;

	// 現在実際に開いて編集しているファイル名
	std::string currentFileName_ = "";

	// UIの入力欄用
	char inputFilename_[128] = "";


private:

	// undoスタック
	std::vector<nlohmann::json> undoStack_;

	// redoスタック
	std::vector<nlohmann::json> redoStack_;
};

