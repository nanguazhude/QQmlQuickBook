#include <sstd_memory_lock.hpp>
#include <boost/context/fiber.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>

extern void test_boost_context();
#include <iostream>

void test_boost_context() {

    {
        using fiber = boost::context::fiber;
        fiber f([](fiber && f)->fiber {
            std::cout << "Hellow World"sv << std::endl;
            return std::move(f);
        });

        std::move(f).resume();
    }

    {
        using fiber = boost::context::fiber;
        fiber f([](fiber && f)->fiber {
            std::cout << "1"sv << std::endl;
            f = std::move(f).resume();
            std::cout << "3"sv << std::endl;
            return std::move(f);
        });

        f = std::move(f).resume();
        std::cout << "2"sv << std::endl;
        f = std::move(f).resume();

    }

    {
        using fiber = boost::context::fiber;
        fiber f(std::allocator_arg, boost::context::protected_fixedsize_stack{ 128 * 1024 }, [](fiber && f)->fiber {
            class Lock {
            public:
                Lock() {
                    std::cout << "Begin Lock"sv << std::endl;
                }
                ~Lock() {
                    std::cout << "End Lock"sv << std::endl;
                }
            } lock;
            f = std::move(f).resume();
            std::cout << "2"sv << std::endl;
            return std::move(f);
        });

        f = std::move(f).resume();
        std::cout << "1"sv << std::endl;
        f = std::move(f).resume();

    }

    std::thread([] {
        using fiber = boost::context::fiber;
        for (auto i = 0; i < 1000; ++i) {
            new fiber(std::allocator_arg, boost::context::protected_fixedsize_stack{ 1024 * 1024 }, [](fiber && f)->fiber {
                class Lock {
                public:
                    Lock() {
                        std::cout << "Begin Lock"sv << std::endl;
                    }
                    ~Lock() {
                        std::cout << "End Lock"sv << std::endl;
                    }
                } lock;
                f = std::move(f).resume();
                std::cout << "2"sv << std::endl;
                return std::move(f);
            });
        }
    }).detach();

    std::thread([]() { system("pause"); }).join();
    //system("pause");
}

//https://cloud.tencent.com/developer/article/1173539
//https://www.sohu.com/a/233039447_827544
//https://blog.csdn.net/weiwenhp/article/details/8507207

