#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czSharedQueue.h"
#include "czPlatform/czMonitor.h"
#include <thread>
#include <atomic>
#include <memory>

using namespace std;
using namespace UnitTest;

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


namespace {

	struct ConsumerThread
	{
		ConsumerThread(cz::SharedQueue<int>& q_, std::vector<int>& v_) : q(q_), v(v_)
		{
			t = std::thread( [this] ()
			{
				while(!finish || q.size()!=0)
				{
					int i;
					if (q.try_and_pop(i))
						v.push_back(i);
				}
			});
		}

		~ConsumerThread()
		{
			finish = true;
			t.join();
		}
		
		cz::SharedQueue<int>& q;
		std::vector<int>& v;
		std::thread t;
		std::atomic<bool> finish{false};
	};

	TEST(TestSharedQueue)
	{
		using namespace cz;
		SharedQueue<int> q;
		const int itemsperproducer = 40000;
		const int numproducers = 4;
		const int numconsumers = 4;
		std::vector<int> vectors[numconsumers];

		// Create consumer threads
		// Each thread reads from the queue and writes to their own vector (reference to a vector in "vectors")
		std::vector< std::shared_ptr<ConsumerThread>> consumers;
		for(int i=0; i<numconsumers; i++)
			consumers.push_back( std::make_shared<ConsumerThread>(q, vectors[i]));


		// Create producer threads.
		// Each thread writes a range of values to the queue
		std::vector< std::thread > producers;
		for(int i=0; i<numproducers; i++)
		{
			int start = i * itemsperproducer;
			producers.push_back( std::thread( [start,itemsperproducer, &q] ()
			{
				for (int i=start; i<start+itemsperproducer; i++)
					q.push(i);
			}));
		}


		for(auto& t : producers)
			t.join();

		// Clearing the consumers waits for their threads to finish
		consumers.clear();

		//
		// Collect all results into one single vector and make sure we have all the items
		//
		std::vector<int> all;
		for(auto& v : vectors)
			for(auto i: v)
				all.push_back(i);
		CHECK(all.size()==numproducers*itemsperproducer);
		// Make sure there are interleaved items (so we know there was some async stuff going on
		bool wasasync=false;
		int previous=-1;
		for(auto i : all)
		{
			if (i<previous)
			{
				wasasync = true;
				break;
			}
			previous = i;
		}
		CHECK(wasasync);

		// Sort so we can check if we have all the items
		std::sort(begin(all), end(all));
		bool ok=true;
		for(size_t i=0; i<all.size(); i++)
		{
			if (all[i]!=i)
			{
				ok = false;
				break;
			}
		}
		CHECK(ok);

	}


}
