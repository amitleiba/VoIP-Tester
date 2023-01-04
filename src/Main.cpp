#include<iostream>
#include<string>

#include"../include/tester/ssp/SoftphoneManager.hpp"
#include"../include/tester/ssp/SoftphoneArguments.hpp"

int main(){
    SoftphoneManager manager(5060, "192.168.1.46");

    manager.pjLibraryInit(1);
    manager.runSpamTest(80);
}