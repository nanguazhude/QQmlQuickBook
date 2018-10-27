
#include <string>
#include <memory>
#include <exception>
#include <string_view>
#include <type_traits>
#include <cassert>

using namespace std::string_view_literals;

class FunctionStack;
class ForFunction;
class WhileFunction;

class Data {
public:
    virtual ~Data() {
    }
    virtual bool isFunction() const {
        return false;
    }
    virtual bool isError() const {
        return false;
    }
    virtual bool isJudgeFunction() const {
        return false;
    }
    virtual bool isYield() const {
        return false;
    }
};

class YieldAns : public Data {
public:
    virtual bool isYield() const override {
        return true;
    }
    static std::shared_ptr<YieldAns> create() {
        const static std::shared_ptr<YieldAns> varAns = std::make_shared<YieldAns>();
        return varAns;
    }
};

class Function :
    public Data {
public:
    virtual ~Function() {
    }
    virtual bool isFunction() const override {
        return true;
    }
    std::shared_ptr<Function> next;
    std::shared_ptr<Data>     ans;
    virtual void call(const FunctionStack *) {
    }
protected:
    bool set_to_yield(const FunctionStack *);
};

class JudgeFunction : public Data {
public:
    virtual bool call(const FunctionStack *) {
        return false;
    }
    bool isJudgeFunction() const override {
        return true;
    }
};

class ForFunction final : public Function {
public:
    std::shared_ptr<Function> whenRun;
    std::shared_ptr<JudgeFunction> whenJudge;
    std::shared_ptr<Function> whenNext;
    virtual void call(const FunctionStack * L) override;
};

class IfElseFunction final : public Function {
public:
    std::shared_ptr<JudgeFunction> whenJudge;
    std::shared_ptr<Function>      whenIf;
    std::shared_ptr<Function>      whenElse;
    virtual void call(const FunctionStack * L) override;
};

class DoWhileFunction final : public Function {
public:
    std::shared_ptr<JudgeFunction> whenJudge;
    std::shared_ptr<Function>      whenRun;
    virtual void call(const FunctionStack * L) override;
};

class WhileFunction final : public Function {
public:
    std::shared_ptr<JudgeFunction> whenJudge;
    std::shared_ptr<Function>      whenRun;
    virtual void call(const FunctionStack * L) override;
};

class FunctionStackState {
    friend class ForFunction;
    friend class WhileFunction;
    friend class Function;
protected:
    std::shared_ptr<Function> currentFunction;
    bool isYieldInLoop{ false };
    virtual ~FunctionStackState() {
    }
public:
    bool isEndl{ false };
    bool hasError{ false };
    bool isYield{ false };

};

class FunctionStackError final : public Data {
public:
    std::string errorString;
    FunctionStackError() {
    }
    FunctionStackError(const std::string_view & arg) :errorString(arg) {
    }
    virtual bool isError() const {
        return true;
    }
};

class FunctionStack :
    public FunctionStackState {
public:

    std::shared_ptr<Data> call(std::shared_ptr<Function> arg) try {

        if (false == bool(arg)) {
            error("function is Null!"sv);
        }

        /*clean the state ... */
        hasError = false;
        isEndl = false;
        isYield = false;

        currentFunction = std::move(arg);
        return next_call();

    } catch (...) {
        return when_error();
    }

    void error(std::string_view arg) const {
        throw (std::make_shared<FunctionStackError>(arg));
    }

    virtual ~FunctionStack() {
    }

    template<typename T>
    inline static std::shared_ptr<Function> makeFunction(
        T && argCurrentCall,
        std::shared_ptr<Function> argNext = {},
        std::shared_ptr<Data> argAns = {}) {
        using U0 = std::remove_reference_t<T>;
        using U = U0;
        class AnsFunction final : public Function {
            U mmm_Function;
        public:
            virtual void call(const FunctionStack *L) override {
                mmm_Function(L);
            }
            AnsFunction(T && d) : mmm_Function(std::forward<T>(d)) {
            }
        };
        auto varAns = std::make_shared<AnsFunction>(std::forward<T>(argCurrentCall));
        varAns->next = std::move(argNext);
        varAns->ans = std::move(argAns);
        return std::move(varAns);
    }

    template<typename T>
    inline static std::shared_ptr<JudgeFunction> makeJudgeFunction(
        T && argCurrentCall) {
        using U0 = std::remove_reference_t<T>;
        using U = U0;
        class AnsFunction final : public JudgeFunction {
            U mmm_Function;
        public:
            virtual bool call(const FunctionStack *L) override {
                return mmm_Function(L);
            }
            AnsFunction(T && d) : mmm_Function(std::forward<T>(d)) {
            }
        };
        auto varAns = std::make_shared<AnsFunction>(std::forward<T>(argCurrentCall));
        return std::move(varAns);
    }

    template< typename WhenJudge, typename WhenNext, typename WhenRun>
    static inline std::shared_ptr<Function> makeForFunction(
        WhenJudge && whenJudge,
        WhenNext && whenNext,
        WhenRun && whenRun) {
        auto varAns = std::make_shared< ForFunction >();
        varAns->whenRun = std::forward<WhenRun>(whenRun);
        varAns->whenNext = std::forward<WhenNext>(whenNext);
        varAns->whenJudge = std::forward<WhenJudge>(whenJudge);
        return std::move(varAns);
    }

public:
    static std::shared_ptr< FunctionStack > create() {
        return std::shared_ptr< FunctionStack > { new FunctionStack };
    }

    std::shared_ptr<Data> resume() try {
        if (isYield == false) {
            error("can not resume function not yield!"sv);
        }
        isYield = false;
        isYieldInLoop = false;
        return next_call();
    } catch (...) {
        return when_error();
    }

    void yield() const {
        if (isEndl) {
            error("can not yield function endl!"sv);
        }
        if (hasError) {
            error("can not yield function error!"sv);
        }
        const_cast<bool &>(isYield) = true;
    }

protected:
    FunctionStack() {
    }

private:
    std::shared_ptr<Data> when_error() {
        hasError = true;
        isEndl = true;
        isYield = false;
        currentFunction.reset();
        try {
            std::rethrow_exception(std::current_exception());
        } catch (const std::shared_ptr<FunctionStackError> & e) {
            return e;
        } catch (...) {
            return std::make_shared<FunctionStackError>("unknow error!"sv);
        }
    }

    std::shared_ptr<Data> next_call() {
        for (;;) {
            currentFunction->call(this);
            if (currentFunction->next) {
                if (isYield&&isYieldInLoop) {
                    return YieldAns::create();
                }
                currentFunction = std::move(currentFunction->next);
                if (isYield) {
                    return YieldAns::create();
                }
                continue;
            } else {
                /*set when finished */
                isEndl = true;
                isYield = false;
                hasError = false;
                auto varAns = std::move(currentFunction->ans);
                currentFunction.reset();
                return std::move(varAns);
            }
        }/*call until finished ...*/
    }

};

void WhileFunction::call(const FunctionStack * L) {
    while (whenJudge ? (whenJudge->call(L)) : true) {
        if (whenRun) {
            whenRun->call(L);
        }
        /*yield*/
        if (set_to_yield(L)) {
            return;
        }
    }
    if (whenRun) {
        this->ans = std::move(whenRun->ans);
        this->next = std::move(whenRun->next);
    }
}

void ForFunction::call(const FunctionStack * L) {
    for (; (whenJudge ? (whenJudge->call(L)) : true); ) {
        if (whenRun) {
            whenRun->call(L);
        }
        if (whenNext) {
            whenNext->call(L);
        }
        /*yield*/
        if (set_to_yield(L)) {
            return;
        }
    }
    if (whenRun) {
        this->ans = std::move( whenRun->ans );
        this->next = std::move(whenRun->next);
    }
}

bool Function::set_to_yield(const FunctionStack * L) {
    if (L->isYield) {
        const_cast<bool &>(L->isYieldInLoop) = true;
        return true;
    }
    return false;
}

void IfElseFunction::call(const FunctionStack * L) {
    if (whenJudge ? (whenJudge->call(L)) : true) {
        if (whenIf) {
            whenIf->call(L);
            this->ans = std::move(whenIf->ans);
            this->next = std::move(whenIf->next);
        }
    } else {
        if (whenElse) {
            whenElse->call(L);
            this->ans = std::move(whenElse->ans);
            this->next = std::move(whenElse->next);
        }
    }
}

void DoWhileFunction::call(const FunctionStack * L) {
    if (whenRun) {
        whenRun->call(L);
        this->ans = whenRun->ans;
    }
    auto varNext = std::make_shared< WhileFunction >();
    varNext->whenJudge = std::move(this->whenJudge);
    varNext->whenRun = std::move(this->whenRun);
    varNext->ans = std::move(this->ans);
    this->next = std::move(varNext);
}

#include <iostream>

int main(int, char **) {

    auto  state = FunctionStack::create();

    class Pack {
    public:
    };

    auto pack = std::make_shared<Pack>();

    auto varHellowWorld = state->makeFunction([pack](const auto *) {
        std::cout << "Hellow World!" << std::endl;
    });

    {
        auto hw1 = state->makeFunction([pack](const auto * L) {
            std::cout << "Hellow World!" << std::endl;
            L->yield();
        });
        hw1->next = varHellowWorld;
        state->call(hw1);
        state->resume();
    }


    class ForPack {
    public:
        std::shared_ptr<Pack> pack;
        int current = 0;
        int max = 100;
        ForPack(std::shared_ptr<Pack> v) :pack(std::move(v)) {
        }
    };
    auto forPack = std::make_shared<ForPack>(pack);

    auto varCall100 = state->makeForFunction(
        state->makeJudgeFunction([forPack](const auto *)->bool {return forPack->current < forPack->max; }),
        state->makeFunction([forPack](const auto *) { ++(forPack->current); }),
        varHellowWorld
    );

    state->call(varCall100);


    system("pause");
}





















