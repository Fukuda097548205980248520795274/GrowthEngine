#include "GrowthEngine.h"
#include <cassert>
#include "Log/Log.h"
#include "Func/CrushHandler/CrushHandler.h"
#include "Func/ConvertString/ConvertString.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")

// インスタンス
std::unique_ptr<GrowthEngine> GrowthEngine::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @param screenWidth スクリーン横幅
/// @param screenHeight スクリーン縦幅
/// @param title タイトル
/// @return 
GrowthEngine* GrowthEngine::GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	if (instance_ == nullptr)
	{
		instance_.reset(new GrowthEngine());
		instance_->Initialize(screenWidth, screenHeight, title);
	}

	return instance_.get();
}

/// @brief インスタンスを取得する
/// @return 
GrowthEngine* GrowthEngine::GetInstance()
{
	// インスタンスがないと失敗
	assert(instance_ != nullptr);
	return instance_.get();
}


/// @brief 初期化
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
void GrowthEngine::Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// 例外が発生したときに起動する
	SetUnhandledExceptionFilter(Engine::ExportDump);

	// COM初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ログの生成
	log_ = std::make_unique<Engine::Log>();

	// ウィンドウアプリケーションの生成と初期化
	winApp_ = std::make_unique<Engine::WinApp>();
	winApp_->Initialize(screenWidth, screenHeight, title , log_.get());

	// 入力の生成と初期化
	input_ = std::make_unique<Engine::Input>();
	input_->Initialize(winApp_.get(), log_.get());

	// オーディオストアの生成と初期化
	audioStore_ = std::make_unique<Engine::AudioStore>();
	audioStore_->Initialize(log_.get());

	// 描画統括の生成と初期化
	renderContext_ = std::make_unique<Engine::RenderContext>();
	renderContext_->Initialize(winApp_.get(), log_.get());
}

/// @brief デストラクタ
GrowthEngine::~GrowthEngine()
{
	// 描画統括の終了
	renderContext_.reset();
	renderContext_ = nullptr;
	if (log_)log_->Logging("RenderContext released \n");

	// オーディオストアの終了
	audioStore_.reset();
	audioStore_ = nullptr;
	if (log_)log_->Logging("AudioStore released \n");

	// 入力の終了
	input_.reset();
	input_ = nullptr;
	if (log_)log_->Logging("Input released \n");

	// ウィンドウアプリケーションの終了
	winApp_.reset();
	winApp_ = nullptr;
	if (log_)log_->Logging("WinApp released \n");

	// 解放漏れを検知する
#ifdef _DEBUG
	Engine::LeakChecker();
	if (log_)log_->Logging("LeakChecker executed \n");
#endif

	// ログの終了
	log_.reset();
	log_ = nullptr;

	// COM終了
	CoUninitialize();
}


/// @brief 描画前処理
void GrowthEngine::PreDraw() 
{
	// 全ての入力情報を取得する
	input_->CheckInputInfo();

	// 流れていない音楽を削除する
	audioStore_->DeletePlayAudio();

	// 描画前処理
	renderContext_->PreDraw();

	// メニューバー
#ifdef _DEVELOPMENT
	MenuBer();
#endif
}

/// @brief 描画後処理
void GrowthEngine::PostDraw()
{
	// 描画後処理
	renderContext_->PostDraw();

	// 全ての入力情報をコピーする
	input_->CopyInputInfo();
}


// メニューバーの生成
#ifdef _DEVELOPMENT

/// @brief メニューバー
void GrowthEngine::MenuBer()
{
	// メニューバー
	if (ImGui::BeginMainMenuBar())
	{
		// ファイル
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) { /* ... */ }
			if (ImGui::MenuItem("Exit")) { /* ... */ }
			ImGui::EndMenu();
		}

		// 生成
		if (ImGui::BeginMenu("Create"))
		{
			// シーン
			if (ImGui::MenuItem("Scene")) { isSceneCreate_ = true; }
			ImGui::EndMenu();
		}

		// ヘルプ
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About")) { /* ... */ }
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// シーン生成
	CreateScene();
}

/// @brief シーン追加
void GrowthEngine::CreateScene()
{
	// シーン追加操作

	// 生成時のみ
	if (isSceneCreate_ == false)return;

	// 追加するかどうか
	static bool isAdd = false;

	// シーン名
	static char sceneName[24] = "";

	// 追加ボタン
	ImGui::OpenPopup("Scene Create");

	// モーダルウィンドウ
	if (ImGui::BeginPopupModal("Scene Create", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// シーン名入力
		ImGui::InputText("scene Name : ", sceneName, IM_ARRAYSIZE(sceneName));

		ImGui::Separator();

		// 入力チェック
		bool hasText = (sceneName[0] != '\0');

		// --- Add ボタンを無効化する ---
		if (!hasText)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			isAdd = true;
			isSceneCreate_ = false;
			ImGui::CloseCurrentPopup();
		}

		if (!hasText)
		{
			ImGui::PopStyleVar();
			ImGui::PopItemFlag();
		}
		// ------------------------------

		ImGui::SameLine();

		// キャンセル
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			isSceneCreate_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// 追加処理
	if (isAdd)
	{
		// シーンファイルを作成する
		std::string sceneNameString = sceneName;
		CreateSceneFile(sceneNameString);
		
		// 初期化
		isAdd = false;
	}


}


/// @brief シーンファイルを作成する
/// @param fileName 
void GrowthEngine::CreateSceneFile(const std::string& className)
{
	// ディレクトリ
	std::string directory = "./Game/Scene";

	// ディレクトリを掘る
	if (!CreateDirectory(Engine::ConvertString(directory).c_str(), nullptr))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			assert(false);
		}
	}

	directory += "/" + className;

	// ディレクトリを掘る
	if (!CreateDirectory(Engine::ConvertString(directory).c_str(), nullptr))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			assert(false);
		}
	}

	// --- .h ファイル生成 ---
	{
		std::ofstream ofs(directory + "/" + className + ".h");
		ofs << "#pragma once\n\n";
		ofs << "class " << className << "\n";
		ofs << "{\n";
		ofs << "public:\n";
		ofs << "    " << className << "();\n";
		ofs << "    ~" << className << "();\n";
		ofs << "};\n";
	}

	// --- .cpp ファイル生成 ---
	{
		std::ofstream ofs(directory + "/" + className + ".cpp");
		ofs << "#include \"" << className << ".h\"\n\n";
		ofs << className << "::" << className << "()\n";
		ofs << "{\n";
		ofs << "}\n\n";
		ofs << className << "::~" << className << "()\n";
		ofs << "{\n";
		ofs << "}\n";
	}

}

#endif