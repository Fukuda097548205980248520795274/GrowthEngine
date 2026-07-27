#pragma once
#include "UIData/UIData.h"

class UIEditor
{
public:

	/// @brief コンストラクタ
	UIEditor();

	/// @brief 描画処理
	void Draw();

	/// @brief UIを描画する
	void DrawUI();

	/// @brief スプライトを取得する
	/// @param name 
	/// @return 
	Sprite* GetSprite(const std::string& name) const;

private:

	/// @brief テクスチャフォルダを走査してロードする
	void RefreshTextureList();


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

	/// @brief データをファイルに保存する
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

	// 編集中のUI要素リスト
	std::vector<UIElementData> uiElements_;

	// 現在選択されているUI要素のインデックス
	int selectedElementIndex_ = -1;

	// 読み込み済みのテクスチャリスト（ファイル名 -> ハンドル）
	std::unordered_map<std::string, TextureHandle> loadedTextures_;

	// コンボボックス表示用のファイル名リスト
	std::vector<std::string> textureNames_;

	// テクスチャフォルダのパス
	const std::string kTextureDir = "./Assets/Textures/";

	// UIデータを保存するディレクトリパス
	const std::string kUIDir = "./Assets/Parameter/UI/";


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

