/**@file teststdthread.cpp
 * A multi threaded example with std::thread
 * @include teststdthread.cpp
 * @author [Md Jahidul Hamid](https://github.com/neurobin)
 * */

#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include "jpcre2.hpp"

typedef jpcre2::select<char> jp;

std::mutex mtx1, mtx2, mtx3;

void sleep(double sec){
    clock_t st = clock();
    while(((double)(clock()-st)/CLOCKS_PER_SEC) < sec);
}


//This is an example how you can use pre-defined data objects in multithreaded program which
//will act independently of one another without any lock:
//The logic is to wrap your objects inside another class and initialize them with constructor.
//Thus when you create objects of this wrapper class, they will have identical member objects.
//This has the overhead of creating new objects instead of using the same ones.
struct MyRegex{
    jp::Regex re[5];
    MyRegex(){
        re[0].compile("\\w","i");
        re[1].compile("\\d","i");
        re[2].compile("\\d\\w+","i");
        re[3].compile("\\d\\w\\s","m");
        re[4].compile("[\\w\\s]+","m");
    }
};

void thread_safe_fun1(){
    MyRegex re;
    std::string sub[5] = {"subject1", "123456789", "1a2b3c", "1a 2b 3c ", "I am a string"};
    for(int i = 0;i<5; ++i){
        re.re[i].match(sub[i], "g");
    }
    int c=0;
    while(c++<4){
        mtx2.lock();
        std::cout<<"\t1";
        mtx2.unlock();
        sleep(0.01);
    }
}

void thread_safe_fun2(){ //uses no global or static variable, thus thread safe.
	jp::Regex re("\\w", "i"); 
    jp::RegexMatch rm(&re);
	rm.setSubject("fdsf").setModifier("g").match();
    int c=0;
    while(c++<4){
        mtx2.lock();
        std::cout<<"\t2";
        mtx2.unlock();
        sleep(0.009);
    }
}

void thread_safe_fun3(){//uses no global or static variable, thus thread safe.
    jp::Regex re("\\w", "g");
    jp::RegexMatch rm(&re);
    rm.setSubject("fdsf").setModifier("g").match();
    int c=0;
    while(c++<4){
        mtx2.lock();
        std::cout<<"\t3";
        mtx2.unlock();
        sleep(0.0095);
    }
}

jp::Regex rec("\\w", "g");

void* thread_pseudo_safe_fun4(){
    //uses global variable 'rec', but uses
    //mutex lock, thus thread safe when the thread is joined with the main thread.
    //But when thread is detached from the main thread, it won't be thread safe any more,
    //because, the main thread can destroy the rec object while possibly being used by the detached child thread.
    mtx1.lock();
    jp::RegexMatch rm(&rec);
    rm.setSubject("fdsf").setModifier("g").match();
    mtx1.unlock();
    int c=0;
    while(c++<4){
        mtx2.lock();
        std::cout<<"\t4";
        mtx2.unlock();
        sleep(0.008);
    }
    return 0;
}

int main(){
    std::thread th1(thread_safe_fun1);
    std::thread th2(thread_safe_fun2);
    std::thread th3(thread_safe_fun3);
    std::thread th4(thread_pseudo_safe_fun4);
    th1.detach();
    th2.detach();
    th3.detach();
    th4.join(); //detach is unsafe for this one.
    return 0;
}
