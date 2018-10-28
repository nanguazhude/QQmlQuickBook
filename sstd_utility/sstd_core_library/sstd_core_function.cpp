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
        sstdNew<sstd::string>(arg)){
    }

    std::shared_ptr<FunctionStackError> FunctionStackError::create(const std::string_view & arg) {
        return sstd::make_shared<FunctionStackError>(arg);
    }

    const sstd::string & FunctionStackError::unknowError() {
        const static sstd::string varAns{"unknow error!"sv};
        return varAns;
    }

    std::shared_ptr<FunctionStackError> FunctionStackError::createUnknowError() {
        const static auto varAns = sstd::make_shared<FunctionStackError>();
        return varAns;
    }

    FunctionStack::FunctionStack() {
        mmm_MemoryPool = sstdNew<memory_pool_type>();
    }

    FunctionStack::~FunctionStack() {
        delete mmm_LastError;
        delete mmm_MemoryPool;
    }

    FunctionData * FunctionStack::call(Function * arg) try{

        if (arg == nullptr) {
            this->error("empty input"sv);
        }



    } catch (...) {
        return when_error();
    }

    void FunctionStack::error(std::string_view arg) const {
        throw( FunctionStackError::create(arg) );
    }

    FunctionData * FunctionStack::when_error() {
        hasError = true;
        isEndl = true;
        isYield = false;
        currentFunction = nullptr;

        if (mmm_LastError == nullptr) {
            mmm_LastError = sstdNew<std::shared_ptr<FunctionData>>();
        }

        try {
            std::rethrow_exception(std::current_exception());
        } catch (const std::shared_ptr<FunctionStackError> & e) {
            *mmm_LastError = e;
            return mmm_LastError->get();
        } catch (...) {
            *mmm_LastError = FunctionStackError::createUnknowError();
            return mmm_LastError->get();
        }
    }

}/*namespace sstd*/

















