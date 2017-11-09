//
// Created by 刘思健 on 2017/11/2.
//
#include <fstream>
#include "extra_info.h"

#include <string>
#include <ctime>
#include <unistd.h>
#include <cstdlib>

using namespace std;

map<string,string> args;

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

static void GetArgs(int argc,char ** argv){
    int c;
    while( (c=getopt(argc,argv,"o:")) != -1){
        switch (c){
            case 'o':
            case 'O':
                if(optarg){
                    args["output"] = optarg;
                }
                break;
        }
    }
}

static void check_and_close(FILE* fp){
    fflush(fp);
    if( fsync(fileno(fp)) == -1) cout<<"fsync fail"<<endl;
    if( ferror(fp)) cout<<"error in "<<args["output"]<<endl;
    fclose(fp);
}
static void redirect_stdio(){
    map<string,string>::iterator it = args.find("output");
    if( it == args.end()){
        cout<<"not found redirect files,dump to screen"<<endl;
        return;
    }

    cout<<"dump log to "<< args["output"]<<endl;

    int pipefd[2];
    if(pipe(pipefd) == -1){
        cout<<"pipe fail"<<endl;
        freopen(args["output"].c_str(),"a",stdout);
        freopen(args["output"].c_str(),"a",stderr);
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
        FILE * log_fp = fopen(args["output"].c_str(),"a");
        if(log_fp == nullptr){
            cout<<"fdopen failed"<<endl;
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

    GetArgs(argc,argv);
    redirect_stdio(); //record the log to file

    InstallInfo info;
    if(! info.ReadDataInBytes(100)) {
        cout<<"open payload file fail"<<endl;
        return 0;
    }

    info.GetBasicHeaderInfo();

    if( ! info.ReadDataInBytes(getSize("payload.bin"))){
        cout<<"read fail"<<endl;
        return 0;
    }

    info.ParseManifest();

    info.ListOpsType(false);
    cout<<"done"<<endl;


    return 0;
}