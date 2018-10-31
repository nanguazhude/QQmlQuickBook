#include "../sstd_function.hpp"

namespace sstd {

    namespace private_sstd_function_stack {

        DataBasic::~DataBasic() {
        }


    }/*private_sstd_function_stack*/


    YieldAns * YieldAns::create() {
        const static auto varAns = sstdNew<YieldAns>();
        return varAns;
    }

    Function::~Function() {
    }

    FunctionStackState::~FunctionStackState() {
    }

    FunctionStackError::FunctionStackError() {
    }

    FunctionStackError::~FunctionStackError() {
        delete mmm_errorString;
    }

    FunctionStackError::FunctionStackError(const std::string_view & arg) : mmm_errorString(
        sstdNew<sstd::string>(arg)) {
    }

    std::shared_ptr<FunctionStackError> FunctionStackError::create(const std::string_view & arg) {
        return sstd::make_shared<FunctionStackError>(arg);
    }

    const sstd::string & FunctionStackError::unknowError() {
        const static sstd::string varAns{ "unknow error!"sv };
        return varAns;
    }

    std::shared_ptr<FunctionStackError> FunctionStackError::createUnknowError() {
        const static auto varAns = sstd::make_shared<FunctionStackError>();
        return varAns;
    }

    namespace {
        class _0_ThisError : public FunctionData {
        public:
            std::shared_ptr<FunctionData> data;
        };
    }/**/

    FunctionStack::FunctionStack() {
        mmm_MemoryPool = sstdNew<memory_pool_type>();
        mmm_ThisError = &(createData< _0_ThisError >()->data);
    }

    FunctionStack::~FunctionStack() {
        delete mmm_MemoryPool;
    }

    FunctionData * FunctionStack::call(Function * arg) try {

        if (arg == nullptr) {
            this->error("empty input"sv);
        }

        /*清除状态*/
        this->hasError = false;
        this->isEndl = false;
        this->isYield = false;

        /*set current ...*/
        this->currentFunction = arg;

        return next_call();


    } catch (...) {
        return when_error();
    }

    FunctionData *FunctionStack::resume() try {
        if (this->isYield == false) {
            error("can not resume function not yield!"sv);
        }
        this->isYield = false;
        this->isYieldInLoop = false;
        return this->next_call();
    } catch (...) {
        return this->when_error();
    }

    void FunctionStack::yield() const {

        if (isEndl) {
            this->error("can not yield function endl!"sv);
        }

        if (hasError) {
            this->error("can not yield function error!"sv);
        }

        const_cast<bool &>(isYield) = true;
    }

    FunctionData *FunctionStack::next_call() {
        for (;;) {
            currentFunction->call(this);
            if (currentFunction->next) {
                if ((this->isYield) && (this->isYieldInLoop)) {
                    /*在while , for ,dowhile 里面yield...*/
                    return YieldAns::create();
                }
                currentFunction = currentFunction->next;
                if (this->isYield) {
                    /*在非循环函数里面yield...*/
                    return YieldAns::create();
                }
            } else {
                /*获得结果*/
                isEndl = true;
                isYield = false;
                hasError = false;
                /*当前函数栈应当被删除...*/
                return currentFunction->ans;
            }
        }
    }

    void FunctionStack::error(std::string_view arg) const {
        throw(FunctionStackError::create(arg));
    }

    FunctionData * FunctionStack::when_error() {
        hasError = true;
        isEndl = true;
        isYield = false;
        currentFunction = nullptr;

        try {
            std::rethrow_exception(std::current_exception());
        } catch (const std::shared_ptr<FunctionStackError> & e) {
            *mmm_ThisError = e;
        } catch (...) {
            *mmm_ThisError = FunctionStackError::createUnknowError();
        }

        return mmm_ThisError->get();
    }

    void IfElseFunction::call(const FunctionStack * L) {
        if (whenJudge ? (whenJudge->call(L)) : true) {
            if (whenIf) {
                whenIf->call(L);
                this->ans = whenIf->ans;
                this->next = whenIf->next;
            }
        } else {
            if (whenElse) {
                whenElse->call(L);
                this->ans = whenElse->ans;
                this->next = whenElse->next;
            }
        }
    }

    bool Function::set_to_yield(const FunctionStack * L) {
        if (L->isYield) {
            const_cast<bool &>(L->isYieldInLoop) = true;
            return true;
        }
        return false;
    }

    void ForFunction::call(const FunctionStack * L) {
        for (; (whenJudge ? (whenJudge->call(L)) : true); ) {
            if (whenRun) {
                whenRun->call(L);
            }
            if (whenNext) {
                whenNext->call(L);
            }
            /*yield ? ...*/
            if (set_to_yield(L)) {
                return;
            }
        }
        /*set the ans...*/
        if (whenRun) {
            this->ans = whenRun->ans;
            this->next = whenRun->next;
        }
    }

    void DoWhileFunction::call(const FunctionStack * L) {
        if (whenRun) {
            whenRun->call(L);
            this->ans = whenRun->ans;
        }
        assert(whenWhile);
        auto & varNext = whenWhile;
        varNext->whenJudge = this->whenJudge;
        varNext->whenRun = this->whenRun;
        varNext->ans = this->ans;
        this->next = varNext;
    }

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
        /*set the ans...*/
        if (whenRun) {
            this->ans = whenRun->ans;
            this->next = whenRun->next;
        }
    }


}/*namespace sstd*/

















