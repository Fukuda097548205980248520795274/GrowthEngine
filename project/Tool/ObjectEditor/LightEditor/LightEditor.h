#pragma once
#include "LightData/LightData.h"

class LightEditor
{
public:

	/// @brief コンストラクタ
	LightEditor() = default;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

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

	/// @brief コントロールウィンドウ描画
	void DrawControlWindow();

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

	/// @brief ファイルに保存する
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

	// 保存・読み込み用のファイル名
	char saveFilename_[128] = "light_new";

	// 編集中のUI要素リスト
	std::vector<LightElementData> lightElements_;

	// 現在選択されているUI要素のインデックス
	int selectedElementIndex_ = -1;

	// UIデータを保存するディレクトリパス
	const std::string kLightDir = "./Assets/Parameter/Light/";


private:

	// undoスタック
	std::vector<nlohmann::json> undoStack_;

	// redoスタック
	std::vector<nlohmann::json> redoStack_;
};

