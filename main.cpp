//
// Created by 刘思健 on 2017/11/2.
//


#include <fstream>
#include "extra_info.h"

#include <string>
#include <ctime>



using namespace std;


fstream::pos_type getSize(const std::string& address) {
    std::fstream motd(address.c_str(), std::ios::binary|std::ios::in|std::ios::ate);
    if(motd) {
        std::fstream::pos_type size = motd.tellg();
        std::cout << address << " " << size << "\n";
        return size;
    } else {
        perror(address.c_str());
    }
}


int main(int argc,char **argv){

    ExtraInfo extra;
    if(! extra.ReadInfo(100)) {
        cout<<"open payload file fail"<<endl;
        return 0;
    }

    extra.ParseInfo();

    cout<<"reading whole payload,will take a little time"<<endl;
    clock_t begin = clock();

   // cout<<"length: "<<getSize("payload.bin")<<endl;

    if(! extra.ReadInfo(getSize("payload.bin"))) {
        cout<<"read fail"<<endl;
        return 0;
    }

    clock_t end =  clock();
    cout<<"time elapsed:"<<double(end-begin)/CLOCKS_PER_SEC<<endl;

    //cout<<"veclength:"<<extra.GetVecLength()<<endl;

    extra.ParseManifist();
    extra.SetPartitions();
    extra.setNumOps();
    extra.ListOpsType();


    cout<<"done"<<endl;

   
    return 0;
}