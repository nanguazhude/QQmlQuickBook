#include <boost/function.hpp>
#include <iostream>
#include <memory_resource>

void test_boost_function(){

    boost::function<void(void)> hellowWorld([](){
        std::cout << "Hellow World!" <<std::endl;
    }, std::allocator<int>{});
    hellowWorld();

}

















