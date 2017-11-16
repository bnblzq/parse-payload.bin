//
// Created by 刘思健 on 2017/11/2.
//
#include <fstream>
#include "extra_info.h"

#include <string>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include "gflags/gflags.h"

using namespace std;

DEFINE_string(input,"payload.bin","file waiting for parsed");
DEFINE_string(output,"","dump log to output");
DEFINE_bool(print,false,"print redundant operations info");


static fstream::pos_type getSize(const std::string& address) {
    std::fstream motd(address.c_str(), std::ios::binary|std::ios::in|std::ios::ate);
    if(motd) {
        std::fstream::pos_type size = motd.tellg();
        std::cout << address << " " << size << "\n";
        return size;
    } else {
        perror(address.c_str());
    }
}


static void check_and_close(FILE* fp){
    fflush(fp);
    if( fsync(fileno(fp)) == -1) cout<<"fsync fail"<<endl;
    if( ferror(fp)) cout<<"error in "<<FLAGS_output<<endl;
    fclose(fp);
}
static void redirect_stdio(){
    if(FLAGS_output.empty()) return ;

    cout<<"dump log to "<< FLAGS_output<<endl;

    int pipefd[2];
    if(pipe(pipefd) == -1){
        cout<<"pipe fail"<<endl;
        freopen(FLAGS_output.c_str(),"a",stdout);
        freopen(FLAGS_output.c_str(),"a",stderr);
        setbuf(stdout,NULL);
        setbuf(stderr,NULL);
        return;
    }

    pid_t pid = fork();
    if(pid == -1){
        cout<<"fork fail"<<endl;
        return;
    }
    if(pid == 0){//child process
        close(pipefd[1]);
        FILE * log_fp = fopen(FLAGS_output.c_str(),"w");
        if(log_fp == nullptr){
            cout<<"fopen failed"<<endl;
            close(pipefd[0]);
            _exit(EXIT_FAILURE);
        }

        FILE * pipe_fp = fdopen(pipefd[0],"r");
        if(pipe_fp == nullptr){
            cout<<"pipe open failed"<<endl;
            check_and_close(log_fp);
            close(pipefd[0]);
            _exit(EXIT_FAILURE);
        }

        char * line = nullptr;
        size_t len {0};
        while( getline(&line,&len,pipe_fp) != -1){
            fprintf(log_fp,line);
            fflush(log_fp);
        }

        free(line);
        check_and_close(log_fp);
        close(pipefd[0]);
        _exit(EXIT_FAILURE);

    }else{ // parent process
        close(pipefd[0]);
        setbuf(stdout, nullptr);
        setbuf(stderr, nullptr);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            cout << "dup2 stdout failed";
        }
        if (dup2(pipefd[1], STDERR_FILENO) == -1) {
            cout << "dup2 stderr failed";
        }

        close(pipefd[1]);
    }
}


int main(int argc,char **argv){

    gflags::SetVersionString("1.0.0");
    gflags::SetUsageMessage("Usage : ./a.out [-input file] [-output file] [-print]");
    gflags::ParseCommandLineFlags(&argc,&argv,false);

    if(argc == 1) {
        gflags::ShowUsageWithFlagsRestrict(gflags::ProgramInvocationShortName(), "");
        return 0;
    }

    redirect_stdio(); //record the log to file

    InstallInfo info;
    if(! info.ReadDataInBytes(FLAGS_input, 100)) {
        cout<<"open payload file fail"<<endl;
        return 0;
    }

    info.GetBasicHeaderInfo();
    if( ! info.ReadDataInBytes(FLAGS_input,getSize(FLAGS_input))){
        cout<<"read fail"<<endl;
        return 0;
    }

    info.ParseManifest();
    info.ListOpsType(FLAGS_print);
    info.ListPostInfo();

    cout<<"done"<<endl;

    return 0;
}