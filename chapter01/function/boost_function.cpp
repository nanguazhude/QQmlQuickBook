#include <boost/function.hpp>
#include <iostream>
#include <memory_resource>
#include <boost/thread.hpp>
#include <future>

void test_boost_function(){

    boost::function<void(void)> hellowWorld([](){
        std::cout << "Hellow World!" <<std::endl;
    }, std::allocator<int>{});
    hellowWorld();

}

















