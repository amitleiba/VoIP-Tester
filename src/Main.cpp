#include<iostream>
#include<string>

#include"tester/ssp/SoftphoneManager.hpp"

int main(){
    SoftphoneManager manager(5060, "192.168.132.93");

    manager.pjLibraryInit(0);
    manager.runSpamTest(100);
}