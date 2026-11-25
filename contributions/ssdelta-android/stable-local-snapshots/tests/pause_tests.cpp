#include <util/cooperative_pause.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
using namespace std::chrono_literals;
void check(bool ok,const char*message){if(!ok)throw std::runtime_error(message);}
int main(){
 CooperativePause gate;std::atomic<bool> cancel{false};auto canceled=[&]{return cancel.load();};
 check(gate.wait(canceled),"initial runnable");gate.pause();
 std::promise<void> entered;auto future=std::async(std::launch::async,[&]{entered.set_value();return gate.wait(canceled);});entered.get_future().wait();
 check(future.wait_for(20ms)==std::future_status::timeout,"paused thread advanced");gate.resume();check(future.get(),"resume failed");
 // A thread becoming ready AFTER pause must be blocked too; no snapshot of statuses.
 gate.pause();auto late=std::async(std::launch::async,[&]{return gate.wait(canceled);});check(late.wait_for(20ms)==std::future_status::timeout,"late wake bypassed pause");
 cancel=true;gate.notify();check(!late.get(),"cancellation did not release waiter");check(gate.paused(),"cancel must not unpause other threads");cancel=false;gate.resume();
 for(int i=0;i<1000;i++){gate.pause();auto f=std::async(std::launch::async,[&]{return gate.wait(canceled);});gate.resume();check(f.get(),"rapid pause/resume lost notification");}
 // Queue admission must serialize with pause: no submissions can cross a completed pause().
 CooperativePause audio;std::atomic<int> uploads{0};std::atomic<bool> stop{false};
 std::thread producer([&]{while(!stop.load()){audio.admit([&]{return stop.load();},[&]{uploads++;});std::this_thread::yield();}});
 while(uploads.load()<100)std::this_thread::yield();audio.pause();int frozen=uploads.load();std::this_thread::sleep_for(30ms);check(uploads.load()==frozen,"audio enqueued while paused");audio.resume();
 auto deadline=std::chrono::steady_clock::now()+1s;while(uploads.load()==frozen&&std::chrono::steady_clock::now()<deadline)std::this_thread::yield();check(uploads.load()>frozen,"audio did not resume");
 audio.pause();stop=true;audio.notify();producer.join();
 gate.pause();std::vector<std::future<bool>> parked;for(int i=0;i<8;i++)parked.push_back(std::async(std::launch::async,[&]{return gate.wait(canceled);}));gate.resume();for(auto&f:parked)check(f.get(),"shutdown release failed");
 std::cout<<"PASS: running/late wake, cancellation, 1000 rapid pause cycles, serialized audio admission, 8-worker shutdown release\n";
}
