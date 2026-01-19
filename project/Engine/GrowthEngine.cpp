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
	renderContext_->PreDraw();

	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) { /* ... */ }
			if (ImGui::MenuItem("Exit")) { /* ... */ }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About")) { /* ... */ }
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}



	// シーン追加
	AddScene();
}

/// @brief 描画後処理
void GrowthEngine::PostDraw()
{
	renderContext_->PostDraw();
}



/// @brief シーン追加
void GrowthEngine::AddScene()
{
	// シーン追加操作
#ifdef _DEVELOPMENT

	// 追加するかどうか
	static bool isAdd = false;

	// シーン名
	static char sceneName[24] = "";

	ImGui::Begin("Scene");

	// 追加ボタン
	if (ImGui::Button("Add Scene"))
	{
		ImGui::OpenPopup("Add Scene Option");
	}

	// モーダルウィンドウ
	if (ImGui::BeginPopupModal("Add Scene Option", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
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
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();

	// 追加処理
	if (isAdd)
	{
		// シーンファイルを作成する
		std::string sceneNameString = sceneName;
		CreateSceneFile(sceneNameString);
		
		// 初期化
		isAdd = false;
	}


#endif
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