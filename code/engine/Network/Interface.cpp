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

#include <Network/Interface.h>
#include <Network/Main.h>


namespace BFG {
namespace Network {

Base::IEntryPoint* Interface::getEntryPoint()
{
	return new Base::CClassEntryPoint<Interface>
	(
		new Interface(),
		&Interface::startNetwork,
		NULL,
		"Network Interface for distributing data throughout the application."
	);
}

Interface::Interface()
{}

void* Interface::startNetwork(void* ptr)
{
	assert(ptr && "Network::Interface::startNetwork() EventLoop pointer invalid!");

	EventLoop * loop = reinterpret_cast<EventLoop*> (ptr);

	mMain.reset(new Main(loop));

	return 0;
}

} // namespace Network
} // namespace BFG
