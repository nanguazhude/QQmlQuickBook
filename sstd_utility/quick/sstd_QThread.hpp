#pragma once

#include "sstd_quick_library_global.hpp"
#include <sstd_memory.hpp>
#include <QtCore/qthread.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <mutex>
#include <future>
#include <shared_mutex>

namespace sstd {

    template<typename ... Args>
    std::shared_ptr< const sstd::vector< std::future<void> > > runHere(Args && ...);
    template<typename Tuple>
    std::shared_ptr< const sstd::vector< std::future<void> > > applyHere(Tuple &&);

    template<typename ... Args>
    std::shared_ptr< const sstd::vector< std::future<void> > > runInMainThread(Args && ...);
    template<typename Tuple>
    std::shared_ptr< const sstd::vector< std::future<void> > > applyInMainThread(Tuple &&);

    /*this is private function , never used it direct ... */
    _1_SSTD_QUICK_LIBRARY_EXPORT std::shared_ptr< const sstd::vector< std::future<void> > > ppp_run_in_main_thread(sstd::vector<std::packaged_task<void(void)>> && arg);
    _1_SSTD_QUICK_LIBRARY_EXPORT std::shared_ptr< const sstd::vector< std::future<void> > > ppp_run_here(sstd::vector<std::packaged_task<void(void)>> &&);

    class MainThread {
    public:
        template<typename ... Args>
        static std::shared_ptr< const sstd::vector< std::future<void> > > runInMainThread(Args && ... args) {
            return sstd::runInMainThread(std::forward<Args>(args)...);
        }
        template<typename Tuple>
        static std::shared_ptr< const sstd::vector< std::future<void> > > applyInMainThread(Tuple && arg) {
            return sstd::applyInMainThread(std::forward<Tuple>(arg));
        }
        template<typename ... Args>
        static std::shared_ptr< const sstd::vector< std::future<void> > > runHere(Args && ... args) {
            return sstd::runHere(std::forward<Args>(args)...);
        }
        template<typename Tuple>
        static std::shared_ptr< const sstd::vector< std::future<void> > > applyHere(Tuple && arg) {
            return sstd::applyHere(std::forward<Tuple>(arg));
        }
    private:
        SSTD_MEMORY_DEFINE(MainThread)
    };

    namespace private_quick_thread_sstd {

        class _1_SSTD_QUICK_LIBRARY_EXPORT RunEventObject : public QObject {
            Q_OBJECT
        public:
            RunEventObject(std::shared_ptr<std::atomic_bool>);
        protected:
            bool event(QEvent *) override;
            std::shared_ptr<std::atomic_bool> mmm_logical_quit;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(RunEventObject)
        };

    }/*private_sstd*/

    class _1_SSTD_QUICK_LIBRARY_EXPORT QuickThread : public QThread, public sstd::MainThread {
        Q_OBJECT
    public:
        ~QuickThread();
        QuickThread();

        template<typename ... Args>
        std::shared_ptr< const sstd::vector< std::future<void> > > runInThisThread(Args && ... args) {

            if constexpr ((sizeof...(Args)) == 0) {
                return {};
            } else {
                if (isLogicalQuit()) {
                    return{};
                }
                sstd::vector<std::packaged_task<void(void)>> varCall;
                varCall.reserve(sizeof...(Args));
                (QuickThread::ppp_push_back(&varCall, std::forward<Args>(args)), ...);
                return ppp_Call(std::move(varCall));
            }

        }

        template<typename Tuple>
        std::shared_ptr< const sstd::vector< std::future<void> > > applyInThisThread(Tuple && arg) {
            const static constexpr auto varTupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
            return QuickThread::ppp_apply(this, std::forward < Tuple >(arg), std::make_index_sequence<varTupleSize>{});
        }

        void setLogicalQuit(bool a) {
            mmm_logical_quit->store(a);
        }

        bool isLogicalQuit() const {
            return mmm_logical_quit->load();
        }

    private:
        template<typename ... Args>
        std::shared_ptr< const sstd::vector< std::future<void> > > friend runHere(Args && ...);
        template<typename Tuple>
        std::shared_ptr< const sstd::vector< std::future<void> > > friend applyHere(Tuple &&);
        template<typename ... Args>
        std::shared_ptr< const sstd::vector< std::future<void> > > friend runInMainThread(Args && ...);
        template<typename Tuple>
        std::shared_ptr< const sstd::vector< std::future<void> > > friend applyInMainThread(Tuple &&);
    protected:
        void run() override;
    private:
        std::shared_ptr<std::atomic_bool> mmm_logical_quit;
        std::shared_mutex mmm_Mutex;
        private_quick_thread_sstd::RunEventObject * mmm_CallObject{ nullptr };
        Q_SLOT void ppp_on_finished();
    private:
        template<typename T>
        static inline void ppp_push_back(sstd::vector<std::packaged_task<void(void)>> * a, T && v) {
            a->emplace_back(std::forward<T>(v));
        }
        static inline void ppp_push_back(sstd::vector<std::packaged_task<void(void)>> * a, std::packaged_task<void(void)> && v) {
            a->push_back(std::move(v));
        }
        template<typename Tuple, std::size_t... I>
        static inline std::shared_ptr< const sstd::vector< std::future<void> > >
            ppp_apply(QuickThread *d, Tuple && t, const std::index_sequence<I...> &) {
            return d->runInThisThread(std::get<I>(std::forward<Tuple>(t))...);
        }
    private:
        std::shared_ptr< const sstd::vector< std::future<void> > > ppp_Call(sstd::vector<std::packaged_task<void(void)>> &&);
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickThread)
    };

    template<typename ... Args>
    std::shared_ptr< const sstd::vector< std::future<void> > > runInMainThread(Args && ... args) {
        if constexpr ((sizeof...(Args)) == 0) {
            return {};
        } else {
            sstd::vector<std::packaged_task<void(void)>> varCall;
            varCall.reserve(sizeof...(Args));
            (QuickThread::ppp_push_back(&varCall, std::forward<Args>(args)), ...);
            return ppp_run_in_main_thread(std::move(varCall));
        }
    }

    template<typename Tuple, std::size_t... I>
    static inline std::shared_ptr< const sstd::vector< std::future<void> > >
        _1_ppp_apply(Tuple && t, const std::index_sequence<I...> &) {
        return runInMainThread(std::get<I>(std::forward<Tuple>(t))...);
    }

    template<typename Tuple>
    std::shared_ptr< const sstd::vector< std::future<void> > > applyInMainThread(Tuple && arg) {
        const static constexpr auto varTupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        return _1_ppp_apply(std::forward < Tuple >(arg), std::make_index_sequence<varTupleSize>{});
    }

    template<typename ... Args>
    std::shared_ptr< const sstd::vector< std::future<void> > > runHere(Args && ... args) {
        if constexpr ((sizeof...(Args)) == 0) {
            return{};
        } else {
            sstd::vector<std::packaged_task<void(void)>> varCall;
            varCall.reserve(sizeof...(Args));
            (QuickThread::ppp_push_back(&varCall, std::forward<Args>(args)), ...);
            return ppp_run_here(std::move(varCall));
        }
    }

    template<typename Tuple, std::size_t... I>
    static inline std::shared_ptr< const sstd::vector< std::future<void> > >
        _2_ppp_apply(Tuple && t, const std::index_sequence<I...> &) {
        return runHere(std::get<I>(std::forward<Tuple>(t))...);
    }

    template<typename Tuple>
    std::shared_ptr< const sstd::vector< std::future<void> > > applyHere(Tuple && arg) {
        const static constexpr auto varTupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        return _2_ppp_apply(std::forward < Tuple >(arg), std::make_index_sequence<varTupleSize>{});
    }

    template<typename ... K>
    class _2_tuple {
    };

    template<std::size_t N, std::size_t I, typename T, typename Tuple>
    class _1_tuple_size;

    template<std::size_t N, std::size_t I, typename T, typename T0, template<typename  > class Tuple>
    class _1_tuple_size<N, I, T, Tuple<T0>> {
    public:
        constexpr const static std::size_t value = std::is_same_v<T, T0> ? I : N;
    };

    template<std::size_t N, std::size_t I, typename T, typename T0,typename T1,typename ...TN, template<typename ...> class Tuple>
    class _1_tuple_size<N, I, T, Tuple<T0,T1,TN...>> {
    public:
        constexpr const static std::size_t value = std::is_same_v<T, T0> ? I : 
            _1_tuple_size<N,1+I,T,Tuple<T1,TN...>>::value ;
    };

    template<typename T,typename Tuple>
    class tuple_index;

    template<typename T, typename ... U, template<typename ...> class Tuple>
    class tuple_index<T, Tuple<U...>> {
    public:
        constexpr const static std::size_t value = _1_tuple_size<sizeof...(U),0,T, _2_tuple<U...>>::value;
        static_assert(value!=(sizeof...(U)),"can not find type in tuple");
    };

}/*namespace sstd*/












