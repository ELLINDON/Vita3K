#include "driver_startup_guard.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
void check(bool b,const char*m){if(!b)throw std::runtime_error(m);}
int main(int argc,char**argv){if(argc<2)return 2;std::filesystem::path root=argv[1];std::filesystem::create_directories(root);auto path=(root/"startup-test.pending").string();std::remove(path.c_str());
 {DriverStartupGuard system(path);check(!system.pending()&&!system.armed(),"system launch must not arm");}
 {DriverStartupGuard crash(path);check(crash.arm("test-turnip"),"persist marker");check(crash.pending(),"marker missing before native load");}
 {DriverStartupGuard next(path);check(next.pending(),"next launch did not detect unconfirmed start");check(next.clear()&&!next.pending(),"fallback acknowledgement failed");}
 {DriverStartupGuard success(path);check(success.arm("another-driver"),"reselect custom");check(success.observe(0,100)==DriverStartupGuard::Health::Waiting&&success.pending(),"wall time without frames cleared marker");check(success.observe(10,101)==DriverStartupGuard::Health::Waiting,"first frame prematurely confirmed");check(success.observe(11,105.9)==DriverStartupGuard::Health::Waiting,"too short health window");check(success.observe(10,200)==DriverStartupGuard::Health::Waiting,"frozen frame confirmed");check(success.observe(20,106)==DriverStartupGuard::Health::Confirmed&&!success.pending()&&!success.armed(),"healthy startup not cleared");}
 {DriverStartupGuard clean(path);check(clean.arm("driver"),"clean arm");check(clean.clear()&&!clean.pending(),"early clean quit must not cause fallback");}
 {DriverStartupGuard fail((root/"missing-parent"/"marker").string());check(!fail.arm("driver")&&!fail.armed(),"write failure falsely armed");}
 for(int i=0;i<100;i++){DriverStartupGuard first(path);check(first.arm("custom"),"cycle arm");DriverStartupGuard restarted(path);check(restarted.pending(),"restart persistence");check(restarted.clear(),"cycle clear");}
 std::cout<<"PASS: custom startup persistence, next-launch detection, system no-op, 5-second/frame health gate, clean quit, write failure and 100 reselect/recovery cycles\n";
}
