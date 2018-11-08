
#include <sstd_function.hpp>
extern void test_boost_context();

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

void simple_test_if_else() {
    auto varFunctionStack = new sstd::FunctionStack;

    class Function {
    public:
        int a = 0;
    };
    auto varPack = std::make_shared<Function>();

    auto judege = varFunctionStack->createJudgeFunction([](auto) {return false; });
    auto add1 = varFunctionStack->createFunction([varPack](auto) {varPack->a += 1; });
    auto add2 = varFunctionStack->createFunction([varPack](auto) {varPack->a += 2; });
    auto program = varFunctionStack->createIfElseFunction(judege, add1, add2);
    varFunctionStack->call(program);

    assert(2 == varPack->a);

    delete varFunctionStack;
}

class FibonacciFunctionStack : public sstd::FunctionStack {
public:

    std::map<int, int> tmp;
    FibonacciFunctionStack() {
        tmp.emplace(0, 0);
        tmp.emplace(1, 1);
    }

    void start(int a) {
        this->call(this->createFunction([this, arg = a](auto) { eval(arg); }));
    }

    void eval(int a) {
        auto varAns = tmp.find(a);
        if (varAns != tmp.end()) {/*finished*/
            return;
        }

        auto varN_1 = tmp.find(a - 1);
        auto varN_2 = tmp.find(a - 2);
        if ((varN_1 != tmp.end()) && (varN_2 != tmp.end())) {/*get this*/
            tmp.emplace(a, (varN_1->second) + (varN_2->second));
            return;
        }

        if (varN_1 == tmp.end()) {
            buildEval(a - 1);
        }

        if (varN_2 == tmp.end()) {
            buildEval(a - 2);
        }

    }

    void buildEval(int a) {
        auto varLastNext = currentFunction->next;
        auto varNext = this->createFunction([this, arg = a](auto) { eval(arg); });
        varNext->next = varLastNext;
        currentFunction->next = varNext;
    }

};

void test_fibonacci_sequence() {
    auto varFunctionStack = new FibonacciFunctionStack;
    varFunctionStack->start(10);
    delete varFunctionStack;
}


int main(int, char **) {

    test_one_add_one();
    test_add_yield();
    simple_test_if_else();
    test_fibonacci_sequence();
    test_boost_context();

    return 0;
}

/*用于简单测试sstd_memory模块*/

