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

#ifdef _MSC_VER
  // Many many warnings coming from Ogre...
  #pragma warning (push)
  #pragma warning (once: 4251)
  #pragma warning (once: 4244)
#endif

#include <Network/Main.h>

#ifdef _WIN32
#include <windows.h> 
#endif //_WIN32

#include <boost/archive/text_oarchive.hpp>

#include <Base/CLogger.h>

#include <Network/ClientChannel.h>
#include <Network/Event.h>
#include <Network/ServerChannel.h>


namespace BFG {
namespace Network {

EventLoop* Main::mLoop = NULL;

Main::Main(EventLoop* loop) :
    mShutdown(false),
	mAcceptor(0)
{
	assert(loop && "Main: EventLoop is invalid");

	if (mLoop)
		throw std::logic_error("Main gets initialized twice!");

	Main::mLoop = loop;

	mLoop->connect(ID::NE_SHUTDOWN, this, &Main::eventHandler);
	mLoop->connect(ID::NE_STARTCLIENT, this, &Main::eventHandler);
	mLoop->connect(ID::NE_STARTSERVER, this, &Main::eventHandler);

	mLoop->registerLoopEventListener<Main>(this, &Main::loopEventHandler);
}

Main::~Main()
{
	mLoop->disconnect(ID::NE_SHUTDOWN, this);
	mLoop->disconnect(ID::NE_STARTCLIENT, this);
	mLoop->disconnect(ID::NE_STARTSERVER, this);
	
	mLoop->unregisterLoopEventListener(this);
}

EventLoop* Main::eventLoop()
{
	return Main::mLoop;
}



//-----------------------------------------------------------------------------
// Network stuff
void Main::listen(const int port)
{
	dbglog << "Network::Main: Listening on port " << port << " ...";

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);

	if (!mAcceptor)
	{
		mAcceptor = new boost::asio::ip::tcp::acceptor(mService, ep);
		startAccept();
	}
	else
	{
		throw std::logic_error("Network::Main::init(): Initialized twice!");
	}
}

void Main::connect(IpPort& ipPort)
{
	std::string address(ipPort.get<0>().c_array());
	dbglog << "Network::Main: Connecting to " << address << ":" << port << " ...";
	
	boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(address);
	boost::asio::ip::tcp::endpoint ep(addr, ipPort.get<1>());
	boost::asio::ip::tcp::socket socket(mService);
	dbglog << "Endpoint: address: " << ep.address().to_string() << " port: " << ep.port();
	ClientChannel::Pointer newConnection =
		ClientChannel::create(mService);

	newConnection->connect(ep);

	mService.run();
}

#if 0
void Main::transmitPool(BaseEventPool* pool)
{
	if (!pool)
		return;

	std::ostringstream dataStream;
	boost::archive::text_oarchive archive(dataStream);

	archive & (*pool);

	std::string outData = dataStream.str();

	std::ostringstream headerStream;
	headerStream << std::setw(8)
	             << std::hex
	             << outData.size();
	std::string outHeader = headerStream.str();

	std::vector<NetworkChannel*>::iterator ncIter = mConnections.begin();
	for (; ncIter != mConnections.end(); ++ncIter)
	{
		NetworkChannel::Pointer channel(*ncIter, null_deleter());

		channel->sendPacket(outHeader, outData);
	}

// #if defined(_DEBUG) || !defined(NDEBUG)
// 	// output
// 	std::cout << "EventManager has sent:" << std::endl 
// 	          << (*pool) << std::endl;
// #endif
}
#endif

#if 0
void Main::publishNetworkPool(BaseEventPool* pool)
{
	boost::mutex::scoped_lock scoped_lock(mEventChannelListMutex);

	if (mThreadCount > 0)
	{
		BaseEventPool* poolClone = requestPool();
		pool->copyTo(poolClone);
		mEventChannelList[0]->receiveEventPool(poolClone);
	}
	freePool(pool);
}
#endif

void Main::eventHandler(Event* networkEvent)
{
	switch (networkEvent->getId())
	{
	case ID::NE_SHUTDOWN:
		mShutdown = true;
		break;
	case ID::NE_STARTCLIENT:
		{
			IpPort addr = boost::get<IpPort>(networkEvent->getData());
			connect(addr);
		}
		break;
	case ID::NE_STARTSERVER:
		listen(boost::get<u16>(networkEvent->getData()));
		break;

	default:
		throw std::logic_error("Network::Main::eventHandler: received unhandled event!");
	}
}

void Main::loopEventHandler(LoopEvent* loopEvent)
{
	if (mShutdown)
	{
		loopEvent->getData().getLoop()->setExitFlag();
	}

	if (! mConnections.empty())
		mService.poll();

}

void Main::startAccept()
{
	ServerChannel::Pointer newConnection =
		ServerChannel::create(mService);

	infolog << "Start Accept";
	mAcceptor->async_accept
	(
		newConnection->socket(),
		boost::bind
		(
			&Main::handleAccept,
			this,
			newConnection,
			boost::asio::placeholders::error
		)
	);

	boost::system::error_code error;
	mService.run(error);
	if (error)
	{
		infolog << error.message();
	}
}

void Main::handleAccept(NetworkChannel::Pointer newConnection,
                        const boost::system::error_code& error)
{
	infolog << "New Incoming connection";
	if (!error)
	{
		newConnection->startHandShake();
		startAccept();
	}
}




#ifdef _WIN32

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // _WIN32

} // namespace Network

} // namespace BFG

#ifdef _MSC_VER
  #pragma warning (pop)
#endif
