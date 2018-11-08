#include <sstd_memory_lock.hpp>
#include <boost/context/fiber.hpp>


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
        fiber f(std::allocator_arg, boost::context::fixedsize_stack{ 8*1024 }, [](fiber && f)->fiber {
            std::cout << "Hellow World"sv << std::endl;
            return std::move(f);
        });

        f = std::move(f).resume();
    }


}





