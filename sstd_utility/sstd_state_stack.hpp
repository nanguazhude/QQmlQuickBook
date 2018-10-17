#pragma once

namespace sstd {

    class StateStackBasic {
    private:

        class State {
        public:
            virtual ~State() { try { destory_(); } catch (...) {} }
        protected:
            std::function<void(void)> destory_;
            State() = default;
            void setState(const std::function<void(void)> &a) { destory_ = a; }
            void setState(std::function<void(void)> &&a) { destory_ = std::move(a); }
            SSTD_MEMORY_DEFINE(State)/**/
        };

        template<typename T___>
        class TypedState :public State {
            using T__ = std::remove_reference_t<T___>;
            using T_ = std::remove_cv_t<T__>;
        public:
            template<typename FGet, typename FReset>
            TypedState(FGet && argFGEt, FReset && argFReset) {
                this->setState(std::bind(
                    std::forward<FReset>(argFReset), argFGEt()));
            }
        };

        sstd::forward_list< std::unique_ptr<State> > stack_;

    public:

        template<
            typename FGet,
            typename = std::void_t< std::enable_if_t<std::is_invocable_v<FGet&&> > > ,
            typename FReset,
            typename = std::result_of_t< FReset( std::result_of_t< FGet() > ) >
        >
        inline void push(FGet && argFGEt, FReset && argFReset) {
            using T___ = std::result_of_t<FGet()>;
            using T__  = std::remove_reference_t<T___>;
            using T    = std::remove_cv_t<T__>;
            stack_.push_front(sstd::make_unique< TypedState<T> >(
                std::forward<FGet>(argFGEt),
                std::forward<FReset>(argFReset)));
        }

        template< 
            typename T,
            typename = void **,
            typename = int **,
            typename FReset,
            typename =std::result_of_t<FReset(T&&)> 
        >
        inline void push_value(T && argFGEt, FReset && argFReset) {
            using T_ = std::remove_cv_t<std::remove_reference_t<T>/**/>;
            auto p = &argFGEt;
            stack_.push_front(sstd::make_unique< TypedState<T_> >(
                [p]() mutable ->T&& { return std::forward<T>(*p); },
                std::forward<FReset>(argFReset)));
        }

        bool pop() {
            if (!stack_.empty()) { stack_.pop_front(); return true; }
            return false;
        }

        virtual ~StateStackBasic() { while (this->pop()); }
    public:
        StateStackBasic() = default;
        StateStackBasic(const StateStackBasic&) = delete;
        StateStackBasic(StateStackBasic&&) = default;
        StateStackBasic&operator=(const StateStackBasic &) = delete;
        StateStackBasic&operator=(StateStackBasic&&) = default;
        SSTD_MEMORY_DEFINE(StateStackBasic)/**/
    };/*class StateStackBasic*/

}/*namespace sstd*/













