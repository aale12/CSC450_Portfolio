//	Anthony Le
//	11/23/2024

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// thread synchronization resources
std::mutex mutex;					// mutex to protect the shared counter
std::condition_variable cv;			// condition variable for thread signaling
bool thread1_complete = false;		// flag to allow thread 2 to begin


// counting up function for thread 1
void countUp() {
	// loop 0 to 20
	for (int i = 0; i < 21; ++i) {
		// 100ms delay so counting isnt instantaneous
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// lock mutex
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Thread 1 counting up: " << i << std::endl;	// print count
		}  // mutex unlocked
	}
	{
		std::lock_guard<std::mutex> lock(mutex);		// lock to update shared resource
		thread1_complete = true;						// set bool flag true
	}
	// notifty other threads waiting on cv
	cv.notify_one();
}

// counting down function for thread 2
void countDown() {
	// need to wait for thread 1 to finish
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock, [] {
			return thread1_complete;
	});
	// loop from 20 to 0
	for (int i = 20; i >= 0; --i) {
		// 100ms delay
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << "Thread 2 counting down: " << i << std::endl;		// print count
	}
}

int main() {
	// create thread 1 to count up
	std::thread thread1(countUp);
	// create thread 2 to count down
	std::thread thread2(countDown);

	// waiting for both threads to finish
	thread1.join();
	thread2.join();

	// print message
	std::cout << "Counting complete." << std::endl;
	return 0;
}
