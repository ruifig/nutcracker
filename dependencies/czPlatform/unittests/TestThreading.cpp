#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"

#include "czPlatform/czThread.h"
#include "czPlatform/czFunctional.h"
#include "czPlatform/czMutex.h"
#include "czPlatform/czConditionVariable.h"
#include "czPlatform/czRandom.h"
#include "czPlatform/czFile.h"

#include <iostream>
#include <time.h>
#include <queue>
#include <math.h>


namespace
{
	cz::atomic_count count(0);
	void Func1()
	{
		cz::this_thread::yield();
		cz::this_thread::sleep_ms(250);
		count.inc();
	}

	void Func2(int a, cz::recursive_mutex& /*m*/)
	{
		while(a--)
		{
			count.inc();
		}
	}

	void AccurateSleep(int ms)
	{
#if CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
	    cz::this_thread::sleep_ms(ms);
#else
        // Because Windows sleep_ms doesn't work properly for low values (e.g: 1ms)
        UnitTest::Timer timer;
        cz::u64 t = timer.GetTimeInMicro();
        while(true)
        {
            cz::u64 tt = timer.GetTimeInMicro();
            if (tt-t >= ms*1000)
                return;
            cz::this_thread::yield();
        }
#endif
	}

	void FakeBusy(int ms)
	{
        UnitTest::Timer timer;
        cz::u64 t = timer.GetTimeInMicro();
        while(true)
        {
            cz::u64 tt = timer.GetTimeInMicro();
            if (tt-t >= ms*1000)
                return;
        }
	}

	template< class T >
	double Average(std::vector<T>& v)
	{
		double sum=0;
		for (int i=0; i<v.size(); i++)
			sum += v[i];
		return sum/v.size();
	}

	void Producer(bool& finished, std::queue<int>& workQueue, cz::recursive_mutex& workMutex, cz::condition_variable& workAvailable, cz::condition_variable& workStarted)
	{
		cz::RandomGenerator rng((cz::u32)time( NULL ));
		int i=0;
		while(true)
		{

			//
			// Simulate producing a new item
			//
		    cz::this_thread::yield();

			//
			// Item produced, so put it in the queue (or wait until consumers do some work, so we have space in the queue)
			//
			cz::unique_lock<cz::recursive_mutex> lk(workMutex);
			// 4 is our maximum queue size, so wait until a consumer removes some, or the main thread asks us to stop
			while (workQueue.size()==5000 && !finished)
				workStarted.wait(lk);

			// Check if we woke up because main thread asked us to finish...
			if (finished)
				break;

			// Simulate putting work in the queue
			workQueue.push(i++);
			// notify consumers that we have work available
			workAvailable.notify_all();

		}

	}

	cz::atomic_count numConsumersStarted(0);
	volatile int nNumConsumers=0;
	void Consumer(int consumerNumber, bool& finished, std::queue<int>& workQueue, cz::recursive_mutex& workQueueMutex, std::vector< cz::tuple<int, int> >& dest, cz::recursive_mutex& destMutex, cz::condition_variable& workAvailable, cz::condition_variable& workStarted)
	{
	    numConsumersStarted.inc();
		cz::RandomGenerator rng((cz::u32)time( NULL ));
		while(true)
		{
			cz::unique_lock<cz::recursive_mutex> queueLock(workQueueMutex);
			// Keep on waiting until producer tells us we're finished, or the queue has items
			while (!finished && workQueue.empty())
				workAvailable.wait(queueLock);

			// check if we woke up because main thread asked us to finish
			if (finished)
				break;

			//
			// Remove work item from work queue
			//
			int workItem = workQueue.front();
			workQueue.pop();
			workStarted.notify_one(); // notify the producer that we did some work
			queueLock.unlock(); // unlock, so other threads can access the workQueue

			//
			// Simulate doing some long work, and write to the destination when work completed
			//
			//AccurateSleep(static_cast<int>(rng.genInclusive(3,5)));
            AccurateSleep(static_cast<int>(5,8));
			cz::unique_lock<cz::recursive_mutex> destLock(destMutex); // lock dest, so we can add the item
			dest.push_back(cz::tuple<int,int>(workItem,consumerNumber));
			destLock.unlock();
		}

	}

    static volatile float priorityTestDummy=0; // just used to hold a value, used to to waste time
	void PriorityTest(int mstowork, int &counter, volatile bool& start, volatile bool& finish)
	{
	    while(!start) {cz::this_thread::yield();}; // wait until we signal a go

        UnitTest::Timer timer;
        cz::u64 t = timer.GetTimeInMicro();
        while(((timer.GetTimeInMicro() - t) < mstowork*1000) && !finish)
        {
            priorityTestDummy = sqrtf(static_cast<float>(++counter));
        }
        finish=true;

	}

	bool SortWorkDone(const cz::tuple<int, int>& a, const cz::tuple<int, int>& b)
	{
		return a.v0 < b.v0;
	}

} // anonymous namespace

namespace unittests
{

TEST(STLCompatibleThread)
{
	// Test with no arguments
	{
		cz::thread t1(Func1);
		t1.join();
		CHECK(count==1);
		CHECK(!t1.joinable());
	}

	// Test with arguments
	{
		cz::recursive_mutex m;
		cz::thread t1(Func2, 2, cz::ref(m));
		t1.join();
		CHECK(count==3);
		CHECK(!t1.joinable());
	}
}

cz::File* logFile=NULL;
void logToFile(const char* fmt, ...)
{
    char buf1[1024];
    va_list args;
    va_start(args, fmt);
    cz_vsnprintf(buf1, 1024, fmt, args);
    va_end(args);
    //cz_snprintf(buf2, 1024, "%s:%d: %s\n", file, line, buf1);
    logFile->write(buf1, static_cast<int>(strlen(buf1)));
    cz::u16 nl = 0x0A0D;
    logFile->write(&nl, 2);
}

TEST(TestConditionVariables_Producer_Consumer)
{
    cz::File logFile_("out.log", cz::File::FILEMODE_WRITE);
    logFile = &logFile_;

	// Test condition variables with a 1 producer and multiple consumers
	cz::recursive_mutex queueMutex;
	cz::recursive_mutex destMutex;
	cz::condition_variable workAvailable;
	cz::condition_variable workStarted;
	std::queue<int> workQueue;
	std::vector< cz::tuple<int, int> > dest;
	bool finished=0;

	UnitTest::Timer timer;
	timer.Start();
	cz::thread producer(Producer, cz::ref(finished), cz::ref(workQueue), cz::ref(queueMutex) , cz::ref(workAvailable), cz::ref(workStarted));
	cz::thread consumer1(Consumer, 1, cz::ref(finished), cz::ref(workQueue), cz::ref(queueMutex), cz::ref(dest), cz::ref(destMutex), cz::ref(workAvailable), cz::ref(workStarted));
	cz::thread consumer2(Consumer, 2, cz::ref(finished), cz::ref(workQueue), cz::ref(queueMutex), cz::ref(dest), cz::ref(destMutex), cz::ref(workAvailable), cz::ref(workStarted));
	cz::thread consumer3(Consumer, 3, cz::ref(finished), cz::ref(workQueue), cz::ref(queueMutex), cz::ref(dest), cz::ref(destMutex), cz::ref(workAvailable), cz::ref(workStarted));
	cz::thread consumer4(Consumer, 4, cz::ref(finished), cz::ref(workQueue), cz::ref(queueMutex), cz::ref(dest), cz::ref(destMutex), cz::ref(workAvailable), cz::ref(workStarted));

	cz::this_thread::sleep_ms(500);

	CHECK(numConsumersStarted==4);

	{
		cz::lock_guard<cz::recursive_mutex> lk(queueMutex);
		finished = true;
		workStarted.notify_one(); // wake up producer
		workAvailable.notify_all(); // wake up consumers
	}

	producer.join();
	consumer1.join();
	consumer2.join();
	consumer3.join();
	consumer4.join();

	//
	// Check if all work items got to the destination vector
	//
	std::sort(dest.begin(), dest.end(), SortWorkDone);
    logToFile("-- Items done = %d Items in queue: %d", dest.size(), workQueue.size());

	int workPerConsumer[4] = {0,0,0,0};
	for(size_t i=0; i<dest.size(); i++)
	{
	    logToFile("  %d, %d", cz::get<0>(dest[i]), cz::get<1>(dest[i]));
		workPerConsumer[ cz::get<1>(dest[i]) - 1 ] += 1;
		if(cz::get<0>(dest[i])!=i)
		{
			CHECK(cz::get<0>(dest[i])==i);
			break;
		}
	}

	logToFile("WorkPerConsumer: %d, %d, %d, %d \n", workPerConsumer[0], workPerConsumer[1] , workPerConsumer[2] , workPerConsumer[3]);
	// Check if all consumers worked
	CHECK(workPerConsumer[0] && workPerConsumer[1] && workPerConsumer[2] && workPerConsumer[3]);
}

/*
TEST(TestThreadPriority)
{
    int c[6] = {0,0,0,0,0,0};

    for(int i=0; i<cz::THREADPRIORITY_COUNT; i++)
    {
        volatile bool finish=false;
        volatile bool start=false;
        int mstowork=500;
        cz::thread t1(PriorityTest, 500, cz::ref(c[i]), cz::ref(start), cz::ref(finish));
        t1._setpriority(static_cast<cz::thread_priority>(i));
        start=true;

        UnitTest::Timer timer;
        int mycounter=0;
        cz::u64 t = timer.GetTimeInMicro();
        while(((timer.GetTimeInMicro() - t) < mstowork*1000) && !finish)
        {
            priorityTestDummy = std::sqrt(static_cast<float>(++mycounter));
        }

        t1.join();
    }


    printf("Finishing threads.\n");

    printf("Counters: %d, %d, %d, %d, %d, %d\n", c[0], c[1], c[2], c[3], c[4], c[5]);

    // Calculate work relative to normal priority
    for(int i=0; i<cz::THREADPRIORITY_COUNT; i++)
    {
        double p = double(c[i])/c[THREADPRIORITY_NORMAL];
        c[i] = static_cast<int>(p*100);
    }
    printf("Counters: %d%%, %d%%, %d%%, %d%%, %d%%, %d%%\n", c[0], c[1], c[2], c[3], c[4], c[5]);

}
*/

} // namespace unittests
