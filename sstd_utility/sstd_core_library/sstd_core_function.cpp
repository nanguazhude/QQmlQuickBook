#include "../sstd_function.hpp"
/*use boost::context::fiber*/
#include <boost/context/fiber.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>

namespace sstd {

    namespace private_sstd_function_stack {

        DataBasic::~DataBasic() {
        }


    }/*private_sstd_function_stack*/


    YieldAns * YieldAns::create() {
        const static auto varAns = sstdNew<YieldAns>();
        return varAns;
    }

    EmptyAns * EmptyAns::create() {
        const static auto varAns = sstdNew<EmptyAns>();
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

    class FunctionStack::Fiber : public FunctionData {
        boost::context::fiber mmm_Fiber;
    public:
        Fiber() = default;
        Fiber(Fiber &&) = delete;
        Fiber&operator=(boost::context::fiber && arg) {
            mmm_Fiber = std::move(arg);
            return *this;
        }
        template<typename Fun, typename =
            std::enable_if_t<false == std::is_same_v<Fiber, std::remove_cv_t<
            std::remove_reference_t< Fun > > > > >
            inline Fiber(Fun &&arg) : mmm_Fiber(std::forward<Fun>(arg)) {
        }
        inline operator bool() const {
            return bool(mmm_Fiber);
        }
        void resume() {
            mmm_Fiber = std::move(mmm_Fiber).resume();
        }
    };

    FunctionStack::FunctionStack() {
        mmm_MemoryPool = sstdNew<memory_pool_type>();
        mmm_ThisError = &(createData< _0_ThisError >()->data);
        mmm_Fiber = createData<Fiber>();
    }

    FunctionStack::~FunctionStack() {
        delete mmm_MemoryPool;
    }

    FunctionData * FunctionStack::call(Function * arg) {
        return ppp_call(arg);
    }

    FunctionData * FunctionStack::ppp_call(Function * arg) try {

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
        mmm_Fiber->resume();
    }

    FunctionData *FunctionStack::next_call() {
        assert(currentFunction);

        if (false == bool(*mmm_Fiber)) {
            *mmm_Fiber = boost::context::fiber(std::allocator_arg,
                boost::context::protected_fixedsize_stack{ 10 * 1024 * 1024 },
                [this](boost::context::fiber && f) ->boost::context::fiber {
                for (;;) {

                    try {
                        this->currentFunction->call(this);
                    } catch (...) {
                        this->when_error();
                        return std::move(f);
                    }

                    if (currentFunction->next) {
                        continue;
                    } else {
                        /*获得结果*/
                        isEndl = true;
                        isYield = false;
                        hasError = false;
                        return std::move(f);
                    }

                }
            });
        }/***************************************/

        mmm_Fiber->resume();

        if (isYield) {
            return YieldAns::create();
        }

        if (hasError) {
            return this->mmm_ThisError->get();
        }

        assert(currentFunction);
        if (currentFunction->ans) {
            return currentFunction->ans;
        }

        return EmptyAns::create();
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

    void ForFunction::call(const FunctionStack * L) {
        for (; (whenJudge ? (whenJudge->call(L)) : true); ) {
            if (whenRun) {
                whenRun->call(L);
            }
            if (whenNext) {
                whenNext->call(L);
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
        }
        /*set the ans...*/
        if (whenRun) {
            this->ans = whenRun->ans;
            this->next = whenRun->next;
        }
    }


}/*namespace sstd*/

















