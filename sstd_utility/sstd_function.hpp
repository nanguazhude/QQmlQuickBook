#pragma once

#include "sstd_memory.hpp"

#ifndef _7_SSTD_FUNCTION_EXPORT_
#define _7_SSTD_FUNCTION_EXPORT_
#endif

namespace sstd{

    class FunctionStack/*函数栈*/;
    class ForFunction/*for*/;
    class WhileFunction/*while*/;
    class DoWhileFunction/*do while*/;
    class JudgeFunction/*get bool*/;
    class IfElseFunction/*if else*/;
    class FunctionStackState/*函数栈的数据*/;

    namespace private_sstd_function_stack {

        class _7_SSTD_FUNCTION_EXPORT_ DataBasic {
            SSTD_DELETE_COPY_ASSIGN(DataBasic)
        public:
            virtual ~DataBasic();
        private:
            SSTD_MEMORY_DEFINE(DataBasic)
        };

    }/*private_sstd_function_stack*/




}/*namespace sstd*/


/*提供一个C++原生实现的，类似于LUA的动态函数运行环境*/

