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

#ifndef AUDIO_LEVEL_H
#define AUDIO_LEVEL_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <Core/Utils.h>

#include <Audio/Defines.h>
#include <Audio/AudioObjectFactory.h>

namespace BFG {
namespace Audio {

class StreamWatch;
class AudioObject;

struct BFG_AUDIO_API Level
{
	typedef std::map<GameHandle, boost::shared_ptr<AudioObject> > ObjectMapT; 

	Level(std::vector<std::string>&);
	~Level();

	void load();
	void unload();
	void pause();

	void createAudioObject(const AOCreation& aoc);

	std::vector<std::string> mFiles;
	ObjectMapT mObjects;

	//boost::scoped_ptr<AudioObjectFactory> mObjectFactory;
	boost::shared_ptr<StreamWatch> mStreamWatch;
};

} // namespace Audio
} // namespace BFG

#endif
