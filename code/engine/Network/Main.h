/*    ___  _________     ____          __         
     / _ )/ __/ ___/____/ __/___ ___ _/_/___ ___ 
    / _  / _// (_ //___/ _/ / _ | _ `/ // _ | -_)
   /____/_/  \___/    /___//_//_|_, /_//_//_|__/ 
                               /___/             

This file is part of the Brute-Force Game Engine, BFG-Engine

For the latest info, see http://www.brute-force-games.com

Copyright (c) 2012 Brute-Force Games GbR

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

#ifndef BFG_NETWORKMAIN_H
#define BFG_NETWORKMAIN_H

#ifdef _MSC_VER
#pragma warning (push)
// "class foo needs to have dll-interface to be used by clients of class bar"
#pragma warning (disable:4251)
#endif

#include <boost/scoped_ptr.hpp>

#include <EventSystem/Core/EventLoop.h>
#include <Network/Defs.h>
#include <Network/Event_fwd.h>
#include <Network/NetworkChannel.h>


namespace BFG {
namespace Network {


class NETWORK_API Main
{
public:
	Main(EventLoop* loop);
	virtual ~Main();

	static EventLoop* eventLoop();

	void listen(const int port);
	void connect(IpPort& ipPort);

	void transmitPool(BaseEventPool* pool);
	void publishNetworkPool(BaseEventPool* pool);

private:
	void eventHandler(Event* networkEvent);

	void loopEventHandler(LoopEvent* loopEvent);

	void startAccept();

	void handleAccept(NetworkChannel::Pointer newConnection,
	                  const boost::system::error_code& error);

	static EventLoop* mLoop;
	bool              mShutdown;

	u16 port;

	boost::asio::io_service mService;
	boost::asio::ip::tcp::acceptor* mAcceptor;

	std::vector<NetworkChannel*> mConnections;

};

} // namespace Network
} // namespace BFG

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif
