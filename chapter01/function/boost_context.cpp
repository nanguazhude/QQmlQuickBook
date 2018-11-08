#include <sstd_memory.hpp>
#include <boost/context/fiber.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>

#include <future>

extern void test_boost_context();
#include <iostream>

class Fiber : public boost::context::fiber {
    using Super = boost::context::fiber;
    using Stack = boost::context::protected_fixedsize_stack;
public:
    template<typename Fun, typename =
        std::enable_if_t<false == std::is_same_v<Fiber, std::remove_cv_t<
        std::remove_reference_t< Fun > > > > >
        inline Fiber(Fun &&);
    Fiber(Fiber &&) = default;
    Fiber&operator=(Fiber &&) = default;
    Fiber() = default;
public:
    Fiber(const Fiber &) = delete;
    Fiber&operator=(const Fiber &) = delete;
};

template<typename Fun, typename >
inline Fiber::Fiber(Fun && arg) :
    Super(std::allocator_arg, Stack{ 10 * 1024 * 1024 }, std::forward<Fun>(arg)) {
}

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

    if constexpr (true) {/*virtual memory ...*/
        enum {
            test_memory_size = 1024
        };
        using fiber = boost::context::fiber;
        std::vector<fiber *> fibers;
        fibers.reserve(test_memory_size);
        for (auto i = 0; i < test_memory_size; ++i) {
            fibers.push_back(new fiber(std::allocator_arg, boost::context::protected_fixedsize_stack{ 1024 * 1024 }, [](fiber && f)->fiber {
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
            }));
        }
        for (auto i : fibers) {
            *i = std::move(*i).resume();
        }
    }



    {
        class Function;
        class Data {
        public:
            std::vector<int> data;
            std::forward_list< std::shared_ptr<Function> > call_list;
        };

        class Function : public boost::context::fiber {
        public:
            using Super = boost::context::fiber;
            Data * data;
            int arg;
            bool isfinished = false;
            Function(Data * d, int a) :data(d), arg(a) {
                Super::operator=(Super([this](Super &&arg)->Super {
                    return this->call_function(std::move(arg)); }));
            }
            Super call_function(Super && f) {

                if (arg < data->data.size()) {
                    isfinished = true;
                    return std::move(f);
                }

                auto input0 = arg - 1;
                auto input1 = arg - 2;
                auto ans0 = 1;
                auto ans1 = 2;

            redo_input_0:
                if (input0 < data->data.size()) {
                    ans0 = data->data[input0];
                } else {
                    data->call_list.push_front(std::make_shared< Function >(data, input0));
                    f = std::move(f).resume();
                    goto redo_input_0;
                }

            redo_input_1:
                if (input1 < data->data.size()) {
                    ans1 = data->data[input1];
                } else {
                    data->call_list.push_front(std::make_shared< Function >(data, input1));
                    f = std::move(f).resume();
                    goto redo_input_1;
                }

                data->data.push_back(ans1 + ans0);
                std::cout << "get value : "sv << arg << std::endl;
                isfinished = true;
                return std::move(f);
            }
        };

        Data varData;
        varData.data.push_back(0);
        varData.data.push_back(1);
        varData.data.push_back(1);
        varData.call_list.push_front(std::make_shared< Function >(&varData, 10));

        while (varData.call_list.empty() == false) {
            if (varData.call_list.front()->isfinished) {
                varData.call_list.pop_front();
                continue;
            }
            varData.call_list.front()->Super::operator=(
                std::move(*varData.call_list.front()).resume());
        }


    }

    //std::thread([]() { system("pause"); }).join();

}

//https://cloud.tencent.com/developer/article/1173539
//https://www.sohu.com/a/233039447_827544
//https://blog.csdn.net/weiwenhp/article/details/8507207

