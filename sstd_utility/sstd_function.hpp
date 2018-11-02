#pragma once

#include "sstd_memory.hpp"

#ifndef _7_SSTD_FUNCTION_EXPORT_
#define _7_SSTD_FUNCTION_EXPORT_ _1_SSTD_CORE_EXPORT
#endif

namespace sstd {

    class FunctionStack/*函数栈*/;
    class ForFunction/*for*/;
    class WhileFunction/*while*/;
    class DoWhileFunction/*do while*/;
    class JudgeFunction/*get bool*/;
    class IfElseFunction/*if else*/;
    class YieldAns/*reutrn yield*/;
    class Function/*function*/;
    class FunctionStackState/*函数栈的数据*/;

    namespace private_sstd_function_stack {

        class _7_SSTD_FUNCTION_EXPORT_ DataBasic {
            SSTD_DELETE_COPY_ASSIGN(DataBasic);
        protected:
            inline DataBasic() = default;
            friend class sstd::FunctionStack;
        public:
            virtual ~DataBasic();
            inline virtual bool isFunction() const {
                return false;
            }
            inline virtual bool isError() const {
                return false;
            }
            inline virtual bool isJudgeFunction() const {
                return false;
            }
            inline virtual bool isYield() const {
                return false;
            }
        private:
            SSTD_MEMORY_DEFINE(DataBasic)
        };

    }/*private_sstd_function_stack*/

    using FunctionData = private_sstd_function_stack::DataBasic;

    class _7_SSTD_FUNCTION_EXPORT_ YieldAns :
        public private_sstd_function_stack::DataBasic {
    public:
        inline virtual bool isYield() const override {
            return true;
        }
        static YieldAns * create();
    protected:
        friend class sstd::FunctionStack;
    };

    class _7_SSTD_FUNCTION_EXPORT_ Function :
        public private_sstd_function_stack::DataBasic {
    public:
        virtual ~Function();
        inline virtual bool isFunction() const override {
            return true;
        }
        Function * next{ nullptr }/*下一个执行函数*/;
        FunctionData * ans{ nullptr }/*只有当next==nullptr才应当设置这个值*/;
        virtual void call(const FunctionStack *) = 0;
    protected:
        bool set_to_yield(const FunctionStack *);
    protected:
        friend class sstd::FunctionStack;
    };

    class JudgeFunction :
        public private_sstd_function_stack::DataBasic {
    public:
        inline virtual bool call(const FunctionStack *) {
            return false;
        }
        inline bool isJudgeFunction() const override {
            return true;
        }
    protected:
        friend class sstd::FunctionStack;
    };

    /*if else 容器*/
    class _7_SSTD_FUNCTION_EXPORT_ IfElseFunction final : public Function{
        friend class sstd::FunctionStack;
    protected:
        JudgeFunction * whenJudge;
        Function *      whenIf;
        Function *      whenElse;
    public:
        virtual void call(const FunctionStack * L) override;
    };

    /*for 容器*/
    class _7_SSTD_FUNCTION_EXPORT_ ForFunction final : public Function{
        friend class sstd::FunctionStack;
    protected:
        Function * whenRun;
        JudgeFunction * whenJudge;
        Function * whenNext;
    public:
        virtual void call(const FunctionStack * L) override;
    };

    /*while 容器*/
    class _7_SSTD_FUNCTION_EXPORT_ WhileFunction final : public Function{
        friend class sstd::FunctionStack;
        friend class sstd::DoWhileFunction;
    protected:
        JudgeFunction * whenJudge;
        Function *      whenRun;
    public:
        virtual void call(const FunctionStack * L) override;
    };

    /*do while 容器*/
    class _7_SSTD_FUNCTION_EXPORT_ DoWhileFunction final : public Function{
       friend class sstd::FunctionStack;
   protected:
       JudgeFunction * whenJudge;
       Function *      whenRun;
       WhileFunction * whenWhile;
   public:
       virtual void call(const FunctionStack * L) override;
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStackState {
        friend class sstd::FunctionStack/*函数栈*/;
        friend class sstd::ForFunction/*for*/;
        friend class sstd::WhileFunction/*while*/;
        friend class sstd::DoWhileFunction/*do while*/;
        friend class sstd::JudgeFunction/*get bool*/;
        friend class sstd::IfElseFunction/*if else*/;
        friend class sstd::YieldAns/*reutrn yield*/;
        friend class sstd::Function/*function*/;
    protected:
        Function * currentFunction{ nullptr };
        bool isYieldInLoop{ false };
        virtual ~FunctionStackState();
    public:
        bool isEndl{ false };
        bool hasError{ false };
        bool isYield{ false };
    protected:
        friend class sstd::FunctionStack;
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStackError final :
    public private_sstd_function_stack::DataBasic{
private:
    sstd::string * mmm_errorString{nullptr};
public:
    FunctionStackError();
    ~FunctionStackError();
    FunctionStackError(const std::string_view & arg);
    inline virtual bool isError() const {
        return true;
    }
    static std::shared_ptr<FunctionStackError> create(const std::string_view & arg);
    inline const sstd::string & what() const {
        return mmm_errorString ? (*mmm_errorString) : unknowError();
    }
    static const sstd::string & unknowError();
    static std::shared_ptr<FunctionStackError> createUnknowError();
    };

    class _7_SSTD_FUNCTION_EXPORT_ FunctionStack :
        public FunctionStackState {
        using memory_pool_type = sstd::MemoryLock<std::unique_ptr<private_sstd_function_stack::DataBasic>>;
        memory_pool_type * mmm_MemoryPool{ nullptr };
        std::shared_ptr<FunctionData> * mmm_ThisError{ nullptr };
    public:
        FunctionStack();
        ~FunctionStack();
        /*在当前内存池中创建对象,对象必须继承自FunctionData*/
        template<typename T, typename ... K>
        inline T * createData(K && ... arg) const;
    public:
        template<typename T>
        inline Function * createFunction(T && argCurrentCall, Function * argNext = nullptr, FunctionData * argAns = nullptr) const;
        template<typename T>
        inline JudgeFunction * createJudgeFunction(T && argCurrentCall) const;
        inline Function * createIfElseFunction(JudgeFunction * whenJudge, Function * whenIf, Function * whenElse = nullptr) const;
        inline ForFunction * createForFunction(JudgeFunction * whenJudge, Function * whenRun, Function * whenNext = nullptr) const;
        inline WhileFunction * createWhileFunction(JudgeFunction * whenJudge, Function * whenRun) const;
        inline DoWhileFunction * createDoWhileFunction(Function * whenRun, JudgeFunction * whenJudge) const;
    public:
        FunctionData * call(Function * arg);
        void error(std::string_view) const/*throw error!*/;
        FunctionData *resume()/*重启标记为yield的函数*/;
        void yield() const/*将当前函数标记为yield*/;
    protected:
        FunctionData * when_error();
        FunctionData *next_call();
    };
}/*namespace sstd*/


namespace sstd {
    template<typename T, typename ... K>
    inline T * FunctionStack::createData(K && ... arg) const {
        static_assert(false == std::is_reference_v<T>);
        static_assert(false == std::is_array_v<T>);
        auto varAnsUnique = sstd::make_unique< std::remove_cv_t<T> >(std::forward<K>(arg)...);
        auto varAns = varAnsUnique.get();
        mmm_MemoryPool->emplace_back(std::move(varAnsUnique));
        return varAns;
    }

    template<typename T>
    inline Function * FunctionStack::createFunction(T && argCurrentCall,
        Function * argNext,
        FunctionData * argAns) const {
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
        auto varAns = this->createData<AnsFunction>(std::forward<T>(argCurrentCall));
        varAns->next = argNext;
        varAns->ans = argAns;
        return varAns;
    }

    template<typename T>
    inline JudgeFunction * FunctionStack::createJudgeFunction(T && argCurrentCall) const {
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
        return this->createData<AnsFunction>(std::forward<T>(argCurrentCall));
    }

    inline Function * FunctionStack::createIfElseFunction(JudgeFunction * whenJudge,
        Function * whenIf,
        Function * whenElse) const {
        auto varAns = this->createData<IfElseFunction>();
        varAns->whenJudge = whenJudge;
        varAns->whenIf = whenIf;
        varAns->whenElse = whenElse;
        return varAns;
    }

    ForFunction * FunctionStack::createForFunction(JudgeFunction * whenJudge, Function * whenRun, Function * whenNext) const {
        auto varAns = this->createData<ForFunction>();
        varAns->whenJudge = whenJudge;
        varAns->whenRun = whenRun;
        varAns->whenNext = whenNext;
        return varAns;
    }

    inline WhileFunction * FunctionStack::createWhileFunction(JudgeFunction * whenJudge, Function * whenRun) const {
        auto varAns = this->createData<WhileFunction>();
        varAns->whenJudge = whenJudge;
        varAns->whenRun = whenRun;
        return varAns;
    }

    inline DoWhileFunction * FunctionStack::createDoWhileFunction(Function * whenRun, JudgeFunction * whenJudge) const {
        auto varAns = this->createData<DoWhileFunction>();
        varAns->whenJudge = whenJudge;
        varAns->whenRun = whenRun;
        varAns->whenWhile = this->createData<WhileFunction>();
        varAns->whenWhile->whenJudge = nullptr;
        varAns->whenWhile->whenRun = nullptr;
        return varAns;
    }

}/*namespace sstd*/

/*提供一个C++原生实现的，类似于LUA的动态函数运行环境*/

