#pragma once

#include "Thread.hpp"
#include <Utils/Singleton.hh>
#include <Utils/Containers/Vector.hpp>
#include <memory>
#include <atomic>
#include <array>
#include <Utils/SpinLock.hpp>

namespace TMQ
{
	class HybridQueue;
}

namespace AGE
{
	class MainThread;
	class RenderThread;
	class Engine;

	class ThreadManager
	{
	public:
		Thread *getCurrentThread() const;
		MainThread *getMainThread() const;
		RenderThread *getRenderThread() const;
		Engine *createEngine();
		Engine *getEngine();
		bool initAndLaunch();
		void exit();

		void forEachThreads(std::function<void(AGE::Thread *)> &&fn);
		void setAsWorker(bool mainThread, bool prepareThread, bool renderThread);
	private:
		Engine *_engine;
		SpinLock _mutex;
		std::size_t _iterator = Thread::Main;

		
		ThreadManager();
		virtual ~ThreadManager();
		ThreadManager(const ThreadManager &o) = delete;
		ThreadManager(ThreadManager &&o) = delete;
		ThreadManager &operator=(const ThreadManager &o) = delete;
		ThreadManager &operator=(ThreadManager &&o) = delete;
		void registerThreadId(std::size_t systemHash, Thread::ThreadType type);

		AGE::Vector<AGE::Thread*> _threads;
		AGE::Vector<std::size_t>  _threadIdReference;

		friend class Singleton < ThreadManager > ;
		friend class Thread;
	};

	ThreadManager *GetThreadManager();
	Thread *CurrentThread();
	MainThread *GetMainThread();
	RenderThread *GetRenderThread();
	bool InitAGE();
	void ExitAGE();
}