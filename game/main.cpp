#include "game/pch.hpp"

#include <iostream>

#include "Game.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"

int main()
{
	Game::MainSingleton::Create();

	auto& main_instance = Game::MainSingleton::Instance();

	std::unique_ptr<Events::Dispatcher> event_dispatcher_system = std::make_unique<Events::Dispatcher>();
	main_instance.SetEventDispatcher(std::move(event_dispatcher_system));

	std::unique_ptr<fs::Manager> filesystem_manager = std::make_unique<fs::Manager>();
	main_instance.SetFilesystemManager(std::move(filesystem_manager));

	#if defined(DEBUG)
	std::unique_ptr<Game::DebugHelper> game_debug_helper = std::make_unique<Game::DebugHelper>();
	game_debug_helper->InitializeDebugEventListeners();

	std::unique_ptr<Tools::Manager> tools_manager = std::make_unique<Tools::Manager>();
	tools_manager->InitializeEventListeners();
	main_instance.SetToolsManager(std::move(tools_manager));
	#endif

	#if defined (_WIN32)
	std::cout << "DEBUG: [Main] Running on Windows\n";

	char* win_appdata_buf = nullptr;
	size_t win_appdata_buf_size = 0;

	if (_dupenv_s(&win_appdata_buf, &win_appdata_buf_size, "APPDATA") != 0)
	{
		std::cout << "ERROR: [Main] Failed to get %appdata% path\n";
		return -1;
	}

	std::string appdata{ win_appdata_buf };
	free(win_appdata_buf);

	std::cout << "%appdata%: " << appdata << "\n";

	std::string engine_appdata_path{ appdata + "\\caspian\\engine\\" };

	std::cout << "DEBUG: [Main] Engine appdata path: " << engine_appdata_path << "\n"
						<< "DEBUG: [Main] Engine appdata alias: " << Game::Windows::S_ENGINE_APPDATA_ALIAS << "\n";

	// INITIALIZE FS HERE.
	std::unique_ptr<fs::NativeFileSystem> engine_appdata_fs = std::make_unique<fs::NativeFileSystem>(engine_appdata_path);
	engine_appdata_fs->Initialize();

	main_instance.GetFilesystemManager()->Mount(Game::Windows::S_ENGINE_APPDATA_ALIAS, std::move(engine_appdata_fs));	
	#endif
	

	while (main_instance.IsRunning())
	{
		main_instance.MainLoop();
	}
}