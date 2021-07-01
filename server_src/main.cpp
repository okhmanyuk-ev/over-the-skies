#include <shared/all.h>
#include "server.h"

void run()
{
	Core::Engine engine;
	
	ENGINE->addSystem<Common::Event::System>(std::make_shared<Common::Event::System>());
	ENGINE->addSystem<Common::FrameSystem>(std::make_shared<Common::FrameSystem>());
	ENGINE->addSystem<Console::Device>(std::make_shared<Common::NativeConsoleDevice>());
	ENGINE->addSystem<Console::System>(std::make_shared<Console::System>());
	ENGINE->addSystem<Network::System>(std::make_shared<Network::System>());
	ENGINE->addSystem<Server>(std::make_shared<Server>());
	
	Common::ConsoleCommands consoleCommands;
	Common::FramerateCounter framerateCounter;

	Common::Timer timer;
	timer.setInterval(Clock::FromSeconds(1.0f));
	timer.setCallback([&] {
		NATIVE_CONSOLE_DEVICE->setTitle(fmt::format("OverTheSkies - {} fps, {} clients", 
			framerateCounter.getFramerate(), SERVER->getChannels().size()));
	});

	bool shutdown = false;
	consoleCommands.setQuitCallback([&shutdown] { 
		shutdown = true; 
	});

	CONSOLE->execute("sys_framerate 60");
	CONSOLE->execute("sys_sleep 1");

	CONSOLE->registerCommand("print", "broadcast message to all clients", { "text" }, { "text.." }, [](CON_ARGS) {
		SERVER->broadcastPrint(CON_ARGS_ACCUMULATED_STRING);
	});

	CONSOLE->registerCommand("save", [](CON_ARGS) {
		SERVER->save();
	});

	//LOGF(" - protocol version {}", Shared::NetworkingUDP::Networking::ProtocolVersion);
	//LOGF(" - port {}", SERVER->getSocket().getPort());

	while (!shutdown)
	{
		try
		{
			FRAME->frame();
		}
		catch (const std::exception& e)
		{
			CONSOLE_DEVICE->writeLine(e.what(), Console::Color::Red);
		}
	}

	SERVER->save();
}

int main(int argc, char* argv[])
{
	try
	{
		run();
	}
	catch (const std::exception& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		system("pause");
	}
	return 0;
}