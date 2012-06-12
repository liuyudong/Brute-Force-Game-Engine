/*    ___  _________     ____          __         
     / _ )/ __/ ___/____/ __/___ ___ _/_/___ ___ 
    / _  / _// (_ //___/ _/ / _ | _ `/ // _ | -_)
   /____/_/  \___/    /___//_//_|_, /_//_//_|__/ 
                               /___/             

This file is part of the Brute-Force Game Engine, BFG-Engine

For the latest info, see http://www.brute-force-games.com

Copyright (c) 2011 Brute-Force Games GbR

The BFG-Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The BFG-Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the BFG-Engine. If not, see <http://www.gnu.org/licenses/>.
*/

#include <Base/CLogger.h>
#include <Base/Network.h>
#include <Base/Pause.h>
#include <Core/ShowException.h>
#include <Core/Types.h>
#include <EventSystem/Core/EventLoop.h>
#include <EventSystem/Emitter.h>
#include <EventSystem/Core/EventManager.h>

#include <Network/Interface.h>
#include <Network/Event_fwd.h>

using namespace BFG;

template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

int main( int argc, const char* argv[] ) try
{
	EventManager::getInstance();

	bool server = false;
	if  (argc == 2)
		server = true;
	else if (argc == 3)
		server = false;
	else
	{
		std::cerr << "For Server use: bfgNetworkTest <Port>\nFor Client use: bfgNetworkTest <IP> <Port>\n";
		BFG::Base::pause();
		return 0;
	}
			
	if (server)
	{
		u16 port;
		if (!from_string(port, argv[1], std::dec))
		{
			std::cerr << "Port not a number: " << argv[1] << std::endl;
			BFG::Base::pause();
			return 0;
		}

		Base::Logger::Init(Base::Logger::SL_DEBUG, "Logs/NetworkServerTest.log");

		dbglog << "Starting as Server";

		EventLoop loop1
		(
			true,
			new EventSystem::BoostThread<>("Loop1"),
			new EventSystem::InterThreadCommunication()
		);

		loop1.addEntryPoint(BFG::Network::Interface::getEntryPoint());
		dbglog << "Starting Network loop";
		loop1.run();

		dbglog << "Giving Network some time to start.";

		Emitter emitter(&loop1);

		for (int i = 5; i > 0; --i)
		{
			dbglog << i;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
		
		emitter.emit<Network::Event>(ID::NE_STARTSERVER, port);

		BFG::Base::pause();

		loop1.stop();
		loop1.cleanUpEventSystem();
		loop1.setExitFlag();

		dbglog << "Goodbye";
	}
	else
	{
		std::string ip(argv[1]);
		u16 port;

		if (!from_string(port, argv[2], std::dec))
		{
			std::cerr << "Port not a number: " << argv[2] << std::endl;
			BFG::Base::pause();
			return 0;
		}

		Base::Logger::Init(Base::Logger::SL_DEBUG, "Logs/NetworkClientTest.log");

		dbglog << "Starting as Client";

		EventLoop loop1
		(
			true,
			new EventSystem::BoostThread<>("Loop1"),
			new EventSystem::InterThreadCommunication()
		);

		loop1.addEntryPoint(BFG::Network::Interface::getEntryPoint());
		dbglog << "Starting Network loop";
		loop1.run();

		dbglog << "Giving Network some time to start.";

		for (int i = 5; i > 0; --i)
		{
			dbglog << i;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		Emitter emitter(&loop1);

		emitter.emit<Network::Event>(ID::NE_STARTCLIENT, BFG::Network::IpPort(stringToArray<128>(ip), port));

		BFG::Base::pause();

		loop1.stop();
		loop1.cleanUpEventSystem();
		loop1.setExitFlag();

		dbglog << "Goodbye";
	}
}
catch (std::exception& ex)
{
	showException(ex.what());
}
catch (...)
{
	showException("Unknown exception");
}
