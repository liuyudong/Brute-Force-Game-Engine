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

#include <Network/Event.h>


namespace BFG {
namespace Network {

EventLoop* Main::mLoop = NULL;

Main::Main(EventLoop* loop) :
    mShutdown(false)
{
	assert(loop && "Main: EventLoop is invalid");

	if (mLoop)
		throw std::logic_error("Main gets initialized twice!");

	Main::mLoop = loop;

	mLoop->connect(ID::NE_SHUTDOWN, this, &Main::eventHandler);

	mLoop->registerLoopEventListener<Main>(this, &Main::loopEventHandler);
}

Main::~Main()
{
	mLoop->disconnect(ID::NE_SHUTDOWN, this);
	
	mLoop->unregisterLoopEventListener(this);
}

EventLoop* Main::eventLoop()
{
	return Main::mLoop;
}

void Main::eventHandler(Event* networkEvent)
{
	switch (networkEvent->getId())
	{
	case ID::NE_SHUTDOWN:
		mShutdown = true;
		break;

	default:
		throw std::logic_error("Network::Main::eventHandler: received unhandled event!");
	}
}

void Main::loopEventHandler(LoopEvent* loopEvent)
{
	if (mShutdown)
	{
		// Error happend, while Rendering
		loopEvent->getData().getLoop()->setExitFlag();
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
