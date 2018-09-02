//#pragma once

namespace sstd{

class StateStackBasic {
private:
    class State {
    protected:
        std::function<void(void)> destory_;
        State() = default;
        virtual ~State() { try { destory_(); } catch (...) {} }
        void setState(const std::function<void(void)> &a) { destory_ = a; }
        void setState(std::function<void(void)> &&a) { destory_ = std::move(a); }
        SSTD_MEMORY_DEFINE(State)/**/
    };

    template<typename T___>
    class TypedState :public State {
        using T__ = std::remove_reference_t<T___>;
        using T_ = std::remove_cv_t<T__>;
    public:
        template<typename FGet, typename FSet, typename FReset>
        TypedState(FGet && argFGEt, FSet && argFSet, FReset && argFReset) {
            T_ varBackValue = argFGEt()/*get the state*/;
            argFSet()/*set the state*/;
            this->setState(std::bind(std::forward<FReset>(argFReset), std::move(varBackValue)));
        }
    };

    sstd::forward_list< std::unique_ptr<State> > stack_;

public:

    template<typename T,typename FGet, typename FSet, typename FReset>
    inline void push(FGet && argFGEt, FSet && argFSet, FReset && argFReset) {
        stack_.push_front(sstd::make_unique<TypedState<T>>(
            std::forward<FGet>(argFGEt),
            std::forward<FSet>(argFSet),
            std::forward<FReset>(argFReset)) );
    }

    bool pop() {
        if (!stack_.empty()) { stack_.pop_front(); return true; }
        return false;
    }

    ~StateStackBasic() { while (this->pop()); }
public:
    StateStackBasic() = default;
    StateStackBasic(const StateStackBasic&) = delete;
    StateStackBasic(StateStackBasic&&) = default;
    StateStackBasic&operator=(const StateStackBasic &)=delete;
    StateStackBasic&operator=(StateStackBasic&&)=default;
};

}/*namespace sstd*/













