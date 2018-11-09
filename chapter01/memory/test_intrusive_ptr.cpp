#include <boost/intrusive_ptr.hpp>
#include <iostream>

class TestA{
public:
    int count = 0;

    ~TestA(){
        std::cout << "~TestA" <<std::endl ;
    }

    void foo(){

    }

};

void intrusive_ptr_add_ref( TestA * p){
    ++(p->count);
}

void intrusive_ptr_release( TestA * p){
    if((--(p->count))<1){
        delete p;
    }
}

extern void test_intrusive_ptr();
void test_intrusive_ptr(){

    boost::intrusive_ptr<TestA> test{ new TestA };

}
















