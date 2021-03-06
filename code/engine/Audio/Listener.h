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

#ifndef BFG_AUDIO_LISTENER_H
#define BFG_AUDIO_LISTENER_H

#include <map>
#include <boost/shared_ptr.hpp>

#include <al.h>
#include <alc.h>

#include <EventSystem/Core/EventLoop.h>

#include <Audio/Defines.h>
#include <Audio/AudioEvent.h>


namespace BFG {
namespace Audio {

struct Level;

//! Listener means the human who is listening in other words the user.
//! The class provides the location of the "listener" in 3d space and
//! the master gain. This class is complete event driven.
class BFG_AUDIO_API Listener
{
public:
	Listener() {}
	~Listener() {}

private:
	virtual void eventHandler(AudioEvent* AE) = 0;

	virtual void onEventMasterGain(const AudioPayloadT& payload) = 0;
	
	virtual void onVelocityPlayer(const AudioPayloadT& payload) = 0;
	virtual void onOrientationPlayer(const AudioPayloadT& payload) = 0;
	virtual void onEventPositionPlayer(const AudioPayloadT& payload) = 0;
};

//! Abstract creation method for Listener instance.
boost::shared_ptr<Listener> createListener();


} // namespace Audio
} // namespace BFG

#endif
