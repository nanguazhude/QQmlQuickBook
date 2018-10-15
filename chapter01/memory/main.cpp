#include <sstd_memory.hpp>
#include <cassert>
#include <list>
#include <vector>

class StaticClass1 {
public:
    int value;
public:
    SSTD_MEMORY_DEFINE(StaticClass1)
};

class StaticClass2 {
public:
    int value;
    StaticClass2(int v) :value(v) {
    }
public:
    SSTD_MEMORY_DEFINE(StaticClass2)
};

class VirtualClass3 {
public:
    int value;
    virtual ~VirtualClass3() = default;
public:
    SSTD_MEMORY_DEFINE(VirtualClass3)
};

class VirtualClass4 {
public:
    int value_0;
    int value_1;
    int value_2;
    VirtualClass4(int a, int b, int c) :value_0(a), value_1(b), value_2(c) {
    }
    VirtualClass4(int a, int b) :VirtualClass4(a, b, 8) {
    }
    VirtualClass4(int a) :VirtualClass4(a, 7, 8) {
    }
    virtual ~VirtualClass4() = default;
public:
    SSTD_MEMORY_DEFINE(VirtualClass4)
};

class VirtualClass5 {
public:
    int value_0;
    virtual ~VirtualClass5() = default;
public:
    SSTD_MEMORY_DEFINE(VirtualClass5)
};

class VirtualClass6 {
public:
    int value_0;
    int value_1;
    virtual ~VirtualClass6() = default;
public:
    SSTD_MEMORY_DEFINE(VirtualClass6)
};

class VirtualClass7 {
public:
    int value_0;
    int value_1;
    int value_2;
    virtual ~VirtualClass7() = default;
public:
    SSTD_MEMORY_DEFINE(VirtualClass7)
};

class alignas(double) ClassT7 {
public:
    bool a : 1;
    bool b : 2;
    bool c : 3;
    int a1;
    double b1;
    float c1;
};

#include <iostream>
#include <sstd_time.hpp>

class TestDeleteCopyAssign {
    SSTD_DELETE_COPY_ASSIGN(TestDeleteCopyAssign);
};

#include <sstd_boost.hpp>

int main(int, char **) {

    {
        boost::circular_buffer<int,sstd::allocator<int>> testBuffer(32);
        testBuffer.push_back(1);
        testBuffer.push_back(3);
    }

    {
        auto var = sstd::getTimeStamp();
        var->getCurrentTime();

        var->postFunction([]() {
            auto var = sstd::getTimeStamp();
            std::cout << "Y : " << var->getCurrentTime() << std::endl;
            var->postFunction([]() {
                auto var = sstd::getTimeStamp();
                std::cout << "Z : " << var->getCurrentTime() << std::endl;
            });
        });

    }

    {
        class A {
        public: virtual ~A() {
        }
        };
        class B : public A {
        };
        class C {
        public:virtual ~C() {
        };
        };
        A * a = new B;
        B * b = sstd_dynamic_cast<B*>(a);
        a = sstd_dynamic_cast<A*>(b);
        C * c = sstd_dynamic_cast<C*>(a);
        delete a;
        (void)c;
    }

    {
        int test = 1;
        {
            sstd::StateStackBasic sm;
            if constexpr (true) {
                sm.push(
                    [&test]() {return test; },
                    [&test](auto v) {test = v; });
            } else {
                sm.push_value(test, [&test](auto v) {test = v; });
            }
            test = 2;
            assert(test == 2);
        }
        assert(test == 1);
    }

    {
        assert(0 == *sstd::make_unique<int>());
        assert(0 == *sstd::make_shared<double>());
        assert(0 == sstd::make_unique<StaticClass1>()->value);
    }

    {
        std::list<ClassT7, sstd::allocator<ClassT7>> test;
        test.emplace_back();
        test.emplace_back();
        test.emplace_back();
        test.emplace_back();
    }

    {
        std::vector<ClassT7, sstd::allocator<ClassT7>> test;
        test.emplace_back();
        test.emplace_back();
        test.emplace_back();
        test.emplace_back();
    }

    {
        auto varAns = sstdNew<VirtualClass4>(6, 7, 8);
        assert(varAns->value_0 == 6);
        assert(varAns->value_1 == 7);
        assert(varAns->value_2 == 8);
        delete varAns;
    }
    {
        auto varAns = sstdNew<VirtualClass4>(6, 7);
        assert(varAns->value_0 == 6);
        assert(varAns->value_1 == 7);
        assert(varAns->value_2 == 8);
        delete varAns;
    }
    //{
    //    auto varAns = sstdNew<VirtualClass7>( 6, 7, 8 );
    //    assert(varAns->value_0 == 6);
    //    assert(varAns->value_1 == 7);
    //    assert(varAns->value_2 == 8);
    //    delete varAns;
    //}
    //{
    //    auto varAns = sstdNew<VirtualClass6>(6, 7);
    //    assert(varAns->value_0 == 6);
    //    assert(varAns->value_1 == 7);
    //    delete varAns;
    //}
    //{
    //    auto varAns = sstdNew<VirtualClass5>(6);
    //    assert(varAns->value_0 == 6);
    //    delete varAns;
    //}
    {
        auto varAns = sstdNew<VirtualClass4>(6);
        assert(varAns->value_0 == 6);
        assert(varAns->value_1 == 7);
        assert(varAns->value_2 == 8);
        delete varAns;
    }
    {
        auto varAns = sstdNew<VirtualClass3>();
        assert(varAns->value == 0);
        delete varAns;
    }
    {
        auto varAns = sstdNew<VirtualClass3>();
        assert(varAns->value == 0);
        delete varAns;
    }
    {
        auto varAns = sstdNew<StaticClass1>();
        assert(varAns->value == 0);
        delete varAns;
    }
    {
        auto varAns = sstdNew<StaticClass1>(1);
        assert(varAns->value == 1);
        delete varAns;
    }
    {
        auto varAns = sstdNew<StaticClass2>(2);
        assert(varAns->value == 2);
        delete varAns;
    }

#if defined( _WIN32 )
    system("pause");
#endif

    return 0;
}

/*用于简单测试sstd_memory模块*/

