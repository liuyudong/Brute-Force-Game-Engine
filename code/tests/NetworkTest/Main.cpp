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

#include <Base/CEntryPoint.h>
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

#include <Physics/Interface.h>
#include <View/Interface.h>

using namespace BFG;

template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

class ServerTest
{
public:
	ServerTest(EventLoop* loop, const u16 port)
	{
		dbglog << "Giving Network some time to start.";

		Emitter emitter(loop);

		for (int i = 2; i > 0; --i)
		{
			//dbglog << i;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		dbglog << "Emitting NE_STARTSERVER with port: " << port;
		emitter.emit<Network::Event>(ID::NE_STARTSERVER, port);
		loop->doLoop();

		for (int i = 10; i > 0; --i)
		{
			//dbglog << i;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		dbglog << "Emitting NE_SHUTDOWN";
		emitter.emit<Network::Event>(ID::NE_SHUTDOWN, 0);
		loop->doLoop();
	}
};

class ClientTest
{
public:
	ClientTest(EventLoop* loop, const u16 port, const std::string& ip)
	{
		dbglog << "Giving Network some time to start.";

		for (int i = 2; i > 0; --i)
		{
			dbglog << i;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		Emitter emitter(loop);

		dbglog << "Emitting NE_STARTCLIENT with ip: " << ip << " and port: " << port;
		emitter.emit<Network::Event>(ID::NE_STARTCLIENT, BFG::Network::IpPort(stringToArray<128>(ip), port));
	}
};

class TestInterface
{
public:
	// This is your hooking place
	static Base::IEntryPoint* getEntryPoint(const u16 port, const std::string& ip = "")
	{
		if (ip != "")
		{
			return new Base::CClassEntryPoint<TestInterface>
			(
				new TestInterface(port, ip),
				&TestInterface::startClientTest
			);
		}
		else
		{
			return new Base::CClassEntryPoint<TestInterface>
			(
				new TestInterface(port),
				&TestInterface::startServerTest
			);
		}
	}

	TestInterface(const u16 port, const std::string& ip = "") :
	mPort(port),
	mIp(ip)
	{}

	friend class Base::CClassEntryPoint<TestInterface>;

private:
	void* startServerTest(void* ptr)
	{
		assert(ptr && "TestInterface::startServerTest() EventLoop pointer invalid!");

		EventLoop * loop = reinterpret_cast<EventLoop*> (ptr);

		mServerTest.reset(new ServerTest(loop, mPort));

		return 0;
	}

	void* startClientTest(void* ptr)
	{
		assert(ptr && "TestInterface::startClientTest() EventLoop pointer invalid!");

		EventLoop * loop = reinterpret_cast<EventLoop*> (ptr);

		mClientTest.reset(new ClientTest(loop, mPort, mIp));

		return 0;
	}

	u16 mPort;
	std::string mIp;
	boost::scoped_ptr<ServerTest> mServerTest;
	boost::scoped_ptr<ClientTest> mClientTest;
};

int main( int argc, const char* argv[] ) try
{
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

		EventLoop loop2
		(
			true,
			new EventSystem::BoostThread<>("Loop2"),
			new EventSystem::InterThreadCommunication()
		);

		loop2.addEntryPoint(TestInterface::getEntryPoint(port));
		dbglog << "Starting ServerTest loop";
		loop2.run();

		BFG::Base::pause();

		loop1.stop();
		loop1.cleanUpEventSystem();
		loop1.setExitFlag();

		loop2.stop();
		loop2.cleanUpEventSystem();
		loop2.setExitFlag();

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

		EventLoop loop2
		(
			true,
			new EventSystem::BoostThread<>("Loop2"),
			new EventSystem::InterThreadCommunication()
		);

		loop2.addEntryPoint(TestInterface::getEntryPoint(port, ip));
		dbglog << "Starting ClientTest loop";
		loop2.run();

		BFG::Base::pause();

		loop1.stop();
		loop1.cleanUpEventSystem();
		loop1.setExitFlag();

		loop2.stop();
		loop2.cleanUpEventSystem();
		loop2.setExitFlag();

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
