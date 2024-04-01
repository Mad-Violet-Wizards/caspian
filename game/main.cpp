#include "game/pch.hpp"

#include <iostream>

#include "Application.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Core/Level.hpp"
#include "engine/Core/Rendering.hpp"

int main()
{
	ApplicationSingleton::Create();

	auto& main_instance = ApplicationSingleton::Instance();

	auto& engine_module = main_instance.GetEngineController();

	std::unique_ptr<Scenes::StateMachine> scenes_state_machine = std::make_unique<Scenes::StateMachine>();
	engine_module.SetScenesStateMachine(std::move(scenes_state_machine));

	std::unique_ptr<Events::Dispatcher> event_dispatcher_system = std::make_unique<Events::Dispatcher>();
	engine_module.SetEventDispatcher(std::move(event_dispatcher_system));

	std::unique_ptr<fs::Manager> filesystem_manager = std::make_unique<fs::Manager>();
	engine_module.SetFilesystemManager(std::move(filesystem_manager));

	std::unique_ptr<Assets::Storage> assets_storage = std::make_unique<Assets::Storage>();
	engine_module.SetAssetsStorage(std::move(assets_storage));

	std::unique_ptr<Level::World> world = std::make_unique<Level::World>();
	main_instance.SetWorld(std::move(world));

	std::unique_ptr<Collisions::Manager> collisions_manager = std::make_unique<Collisions::Manager>();
	collisions_manager->SetCollisionSolver(Collisions::ECollisionSolver::BruteForce);
	engine_module.SetCollisionsManager(std::move(collisions_manager));

	std::unique_ptr<Rendering::System> rendering_system = std::make_unique<Rendering::System>();
	main_instance.SetRenderingSystem(std::move(rendering_system));

	std::unique_ptr<GameObjectCollection> game_objects = std::make_unique<GameObjectCollection>();
	engine_module.SetGameObjectStorage(std::move(game_objects));

	#if defined(DEBUG)
	std::unique_ptr<Projects::Manager> projects_manager = std::make_unique<Projects::Manager>();
	main_instance.SetProjectsManager(std::move(projects_manager));

	std::unique_ptr<DebugHelper> game_debug_helper = std::make_unique<DebugHelper>();
	game_debug_helper->InitializeDebugEventListeners();

	std::unique_ptr<Tools::Manager> tools_manager = std::make_unique<Tools::Manager>();
	tools_manager->InitializeEventListeners();
	engine_module.SetToolsManager(std::move(tools_manager));
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
						<< "DEBUG: [Main] Engine appdata alias: " << Windows::S_ENGINE_APPDATA_ALIAS << "\n";

	std::unique_ptr<fs::NativeFileSystem> engine_appdata_fs = std::make_unique<fs::NativeFileSystem>(engine_appdata_path);
	engine_appdata_fs->SetProjectFilesystem(false);

	engine_appdata_fs->Initialize();

	if (engine_appdata_fs->FileExists("projects.json"))
	{
		if (std::shared_ptr<fs::IFile> projects_json_file = engine_appdata_fs->OpenFile("projects.json", fs::io::OpenMode::In))
		{
			std::shared_ptr<ISerializable::JSON> projects_json = std::make_shared<Serializable::JSON::ProjectsInfo>();

			projects_json_file->Seek(0, fs::io::Origin::Begin);
			projects_json_file->DeserializeJson(projects_json);

			auto projects_info_casted = std::static_pointer_cast<Serializable::JSON::ProjectsInfo>(projects_json);
			main_instance.GetProjectsManager()->FillProjectsList(projects_info_casted);
		}
	}

	main_instance.GetEngineController().GetFilesystemManager()->Mount(Windows::S_ENGINE_APPDATA_ALIAS, std::move(engine_appdata_fs));	
	#endif
	

	while (main_instance.IsRunning())
	{
		main_instance.MainLoop();
	}
}