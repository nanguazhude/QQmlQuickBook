
#include <sstd_function.hpp>

/*简单测试...*/
void test_one_add_one() {
    auto varFunctionStack = new sstd::FunctionStack;
    class Function {
    public:
        int a = 1;
        int b = 1;
        int ans = 0;
    };
    auto varPack = std::make_shared<Function>();
    auto varFunction = varFunctionStack->createFunction([varPack](const auto *) {
        varPack->ans = varPack->a + varPack->b;
    });
    varFunctionStack->call(varFunction);
    assert(2 == varPack->ans);
    delete varFunctionStack;
}

/*简单测试yield...*/
void test_add_yield() {
    auto varFunctionStack = new sstd::FunctionStack;
    class Function {
    public:
        int a = 0;
    };
    auto varPack = std::make_shared<Function>();
    auto varFunction1 = varFunctionStack->createFunction([varPack](const auto *) {
        ++(varPack->a);
    });
    auto varFunction2 = varFunctionStack->createFunction([varPack](const auto * L) {
        L->yield();
        ++(varPack->a);
    }, varFunction1);
    varFunctionStack->call(varFunction2);
    assert(1 == varPack->a);
    varFunctionStack->resume();
    assert(2 == varPack->a);
    delete varFunctionStack;
}

void simple_test_if_else(){
    auto varFunctionStack = new sstd::FunctionStack;

    class Function {
    public:
        int a = 0;
    };
    auto varPack = std::make_shared<Function>();

    auto judege = varFunctionStack->createJudgeFunction([](auto) {return false; });
    auto add1 = varFunctionStack->createFunction([varPack](auto) {varPack->a += 1; });
    auto add2 = varFunctionStack->createFunction([varPack](auto) {varPack->a += 2; });
    auto program = varFunctionStack->createIfElseFunction(judege,add1,add2);
    varFunctionStack->call(program);

    assert(2==varPack->a);

    delete varFunctionStack;
}

int main(int, char **) {

    test_one_add_one();
    test_add_yield();
    simple_test_if_else();

    return 0;
}

/*用于简单测试sstd_memory模块*/

