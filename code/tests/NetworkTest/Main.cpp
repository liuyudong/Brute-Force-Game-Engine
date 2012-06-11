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
#include <Core/ClockUtils.h>
#include <Core/ShowException.h>
#include <Core/Path.h>
#include <Core/Types.h>
#include <Core/Utils.h>
#include <EventSystem/Core/EventLoop.h>
#include <EventSystem/Emitter.h>
#include <EventSystem/Core/EventManager.h>

#include <Controller/Action.h>
#include <Controller/ControllerEvents.h>
#include <Controller/Interface.h>
#include <Model/Interface.h>
#include <Physics/Interface.h>
#include <View/ControllerMyGuiAdapter.h>
#include <View/Event.h>
#include <View/HudElement.h>
#include <View/Interface.h>
#include <View/State.h>
#include <View/WindowAttributes.h>

using namespace BFG;

const s32 A_EXIT = 10000;

struct ServerState : Emitter
{
	ServerState(GameHandle handle, EventLoop* loop) :
	Emitter(loop),
	mClock(new Clock::StopWatch(Clock::milliSecond)),
	mExitNextTick(false)
	{
		mClock->start();
	}

	void LoopEventHandler(LoopEvent* iLE)
	{
		if (mExitNextTick)
		{
			// Error happened, while doing stuff
			iLE->getData().getLoop()->setExitFlag();
		}

		long timeSinceLastFrame = mClock->stop();
		if (timeSinceLastFrame)
			mClock->start();

		f32 timeInSeconds = static_cast<f32>(timeSinceLastFrame) / Clock::milliSecond;
		tick(timeInSeconds);
	}

	void tick(const f32 timeSinceLastFrame)
	{
		if (timeSinceLastFrame < EPSILON_F)
			return;
	}
	
	boost::scoped_ptr<Clock::StopWatch> mClock;
	bool mExitNextTick;
};

struct ClientState : Emitter
{
	ClientState(GameHandle handle, EventLoop* loop) :
	Emitter(loop),
	mClock(new Clock::StopWatch(Clock::milliSecond)),
	mExitNextTick(false)
	{
		mClock->start();
	}

	void LoopEventHandler(LoopEvent* iLE)
	{
		if (mExitNextTick)
		{
			// Error happened, while doing stuff
			iLE->getData().getLoop()->setExitFlag();
		}

		long timeSinceLastFrame = mClock->stop();
		if (timeSinceLastFrame)
			mClock->start();

		f32 timeInSeconds = static_cast<f32>(timeSinceLastFrame) / Clock::milliSecond;
		tick(timeInSeconds);
	}

	void tick(const f32 timeSinceLastFrame)
	{
		if (timeSinceLastFrame < EPSILON_F)
			return;
	}

	boost::scoped_ptr<Clock::StopWatch> mClock;
	bool mExitNextTick;
};

struct ViewControllerState : Emitter
{
	ViewControllerState(GameHandle handle, EventLoop* loop) :
	Emitter(loop),
	mClock(new Clock::StopWatch(Clock::milliSecond)),
    mExitNextTick(false)
//	mMenu("PongMain.layout","MainPanel")
	{
		mClock->start();
	}

	void LoopEventHandler(LoopEvent* iLE)
	{
		if (mExitNextTick)
		{
			// Error happened, while doing stuff
			iLE->getData().getLoop()->setExitFlag();
		}

		long timeSinceLastFrame = mClock->stop();
		if (timeSinceLastFrame)
			mClock->start();

		f32 timeInSeconds = static_cast<f32>(timeSinceLastFrame) / Clock::milliSecond;
		tick(timeInSeconds);
	}

	void tick(const f32 timeSinceLastFrame)
	{
		if (timeSinceLastFrame < EPSILON_F)
			return;
	}

	void ControllerEventHandler(Controller_::VipEvent* iCE)
	{
		switch(iCE->getId())
		{
			case A_EXIT:
			{
				mExitNextTick = true;
				emit<View::Event>(ID::VE_SHUTDOWN, 0);
				break;
			}
		}
	}

	boost::scoped_ptr<Clock::StopWatch> mClock;
	bool mExitNextTick;

//	BFG::View::HudElement mMenu;
};

struct ClientViewState : public View::State
{
public:
	ClientViewState(GameHandle handle, EventLoop* loop) :
	State(handle, loop),
	mControllerMyGuiAdapter(handle, loop)
	{}

	~ClientViewState()
	{}

	virtual void pause()
	{}

	virtual void resume()
	{}
	
private:
	BFG::View::ControllerMyGuiAdapter mControllerMyGuiAdapter;
};

void initController(Emitter& emitter, GameHandle stateHandle)
{
	// At the beginning, the Controller is "empty" and must be filled with
	// states and actions. A Controller state corresponds to a Model state
	// or a View state and in fact, they must have the same handle
	// (GameHandle).
	// This part here is necessary for Action deserialization.
	Controller_::ActionMapT actions;
	actions[A_EXIT] = "A_EXIT";
	Controller_::fillWithDefaultActions(actions);
	Controller_::sendActionsToController(emitter.loop(), actions);

	// Actions must be configured by XML
	BFG::Path path;
	const std::string configPath = path.Expand("NetworkTest.xml");
	const std::string stateName = "NetworkTestClient";

	// The Controller must know about the size of the window for the mouse
	BFG::View::WindowAttributes wa;
	BFG::View::queryWindowAttributes(wa);
	
	// Finally, send everything to the Controller
	BFG::Controller_::StateInsertion si(configPath, stateName, stateHandle, true, wa);
	emitter.emit<BFG::Controller_::ControlEvent>
	(
		BFG::ID::CE_LOAD_STATE,
		si
	);
}

void* ServerEntryPoint(void *iPointer)
{
	EventManager::getInstance()->listen(31876);

	EventLoop* loop = static_cast<EventLoop*>(iPointer);
	
	GameHandle NTHandle = BFG::generateHandle();
	
	// Hack: Using leaking pointers, because vars would go out of scope
	ServerState* ss = new ServerState(NTHandle, loop);

	assert(loop);
	loop->registerLoopEventListener(ss, &ServerState::LoopEventHandler);
	return 0;
}

void* ClientEntryPoint(void *iPointer)
{
	std::string ip;
	BFG::Base::resolveDns("217.189.233.39", ip);
	EventManager::getInstance()->connect(ip, 31876);

	EventLoop* loop = static_cast<EventLoop*>(iPointer);
	assert(loop);

	GameHandle NTHandle = BFG::generateHandle();

	// Hack: Using leaking pointers, because vars would go out of scope
	ClientState* cs = new ClientState(NTHandle, loop);

	loop->registerLoopEventListener(cs, &ClientState::LoopEventHandler);
	return 0;
}

void* ViewControllerEntryPoint(void *iPointer)
{
	EventLoop* loop = static_cast<EventLoop*>(iPointer);
	assert(loop);

	Emitter emitter(loop);

	GameHandle NTHandle = BFG::generateHandle();

	// Hack: Using leaking pointers, because vars would go out of scope
	ViewControllerState* cs = new ViewControllerState(NTHandle, loop);
	ClientViewState* vcs = new ClientViewState(NTHandle, loop);

	initController(emitter, NTHandle);

	loop->connect(A_EXIT, cs, &ViewControllerState::ControllerEventHandler);

	loop->registerLoopEventListener(cs, &ViewControllerState::LoopEventHandler);

	dbglog << "ViewControllerEntryPoint complete";
	return 0;
}

int main( int argc, const char* argv[] ) try
{
	bool server = false;
	if  (argc > 1)
		server = true;
			
	if (server)
	{
		Base::Logger::Init(Base::Logger::SL_DEBUG, "Logs/NetworkServerTest.log");

		EventLoop loop1
		(
			true,
			new EventSystem::BoostThread<>("Loop1"),
			new EventSystem::InterThreadCommunication()
		);

		loop1.addEntryPoint(BFG::ModelInterface::getEntryPoint());
		loop1.addEntryPoint(BFG::Physics::Interface::getEntryPoint());
		dbglog << "Starting Model and Physics loop";
		loop1.run();

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		EventLoop loop2
		(
			true,
			new EventSystem::BoostThread<>("Loop2"),
			new EventSystem::InterThreadCommunication()
		);

		loop2.addEntryPoint(new BFG::Base::CEntryPoint(ServerEntryPoint));
		dbglog << "Starting Network loop";
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
		Base::Logger::Init(Base::Logger::SL_DEBUG, "Logs/NetworkClientTest.log");

		EventLoop loop1
		(
			true,
			new EventSystem::BoostThread<>("Loop1"),
			new EventSystem::InterThreadCommunication()
		);

		size_t controllerFrequency = 1000;

		loop1.addEntryPoint(BFG::View::Interface::getEntryPoint("BFG-Engine: Network Test"));
		loop1.addEntryPoint(BFG::ControllerInterface::getEntryPoint(controllerFrequency));
		loop1.addEntryPoint(BFG::Physics::Interface::getEntryPoint());
		loop1.addEntryPoint(new BFG::Base::CEntryPoint(ViewControllerEntryPoint));
		dbglog << "Starting View and Controller loop";
		loop1.run();

		EventLoop loop2
		(
			true,
			new EventSystem::BoostThread<>("Loop2"),
			new EventSystem::InterThreadCommunication()
		);

		loop2.addEntryPoint(new BFG::Base::CEntryPoint(ClientEntryPoint));
		dbglog << "Starting Network loop";
		loop2.run();

		bool exitFlag = false;
		do 
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			exitFlag = loop1.shouldExit();
		} while (!exitFlag);

		loop1.stop();
		loop1.cleanUpEventSystem();

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
