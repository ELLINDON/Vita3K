// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cerrno>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <string>
#include <utility>

// A conservative startup guard, not a crash-cause detector. No background polling or thread.
class DriverStartupGuard {
    std::string path_;
    bool armed_=false,seen_frame_=false;
    uint64_t first_frame_=0;
    double first_seen_at_=0;
public:
    enum class Health { Waiting,Confirmed,ClearFailed };
    explicit DriverStartupGuard(std::string path):path_(std::move(path)){}
    bool pending()const{std::ifstream file(path_,std::ios::binary);return file.is_open();}
    bool armed()const{return armed_;}
    bool arm(const std::string&driver){
        std::ofstream file(path_,std::ios::binary|std::ios::trunc);
        if(!file)return false;
        file<<driver<<'\n';file.flush();const bool written=bool(file);file.close();
        armed_=written&&!file.fail();seen_frame_=false;
        return armed_;
    }
    bool clear(){
        const bool ok=std::remove(path_.c_str())==0||errno==ENOENT;
        armed_=false;return ok;
    }
    Health observe(uint64_t last_frame_vblank,double elapsed){
        if(!armed_)return Health::Waiting;
        if(!seen_frame_){if(last_frame_vblank>0){seen_frame_=true;first_frame_=last_frame_vblank;first_seen_at_=elapsed;}return Health::Waiting;}
        if(elapsed-first_seen_at_<5 || last_frame_vblank<=first_frame_)return Health::Waiting;
        return clear()?Health::Confirmed:Health::ClearFailed;
    }
};
