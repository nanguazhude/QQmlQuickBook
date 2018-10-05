#include "QuickImageProvider.hpp"
#include <map>
#include <mutex>
#include <atomic>
#include <shared_mutex>

namespace {

    inline auto image_header() { return QStringLiteral("image://quickqimage/"); }

    class Map {
    public:
        FINAL_CLASS_TYPE_ASSIGN(ImageType, QImage);
        FINAL_CLASS_TYPE_ASSIGN(ImageKeyType, QString);
        std::map<ImageKeyType, ImageType, std::less<void>,
        sstd::allocator<std::pair<const ImageKeyType, ImageType>>> mmm_Map;
        std::shared_mutex mmm_Mutex;

        void erase(const QString & arg) {
            std::unique_lock varLock{ mmm_Mutex };
            const auto varPos = mmm_Map.find(arg);
            if (varPos == mmm_Map.end()) { return; }
            mmm_Map.erase(varPos);
        }

        void add(const QString & argk, const QImage & argv) {
            std::unique_lock varLock{ mmm_Mutex };
            do {
                auto varPos = mmm_Map.find(argk);
                if (varPos == mmm_Map.end()) { break; }
                varPos->second = sstd::aswr(argv);
                return;
            } while (false);
            mmm_Map.emplace(sstd::aswr(argk), sstd::aswr(argv));
        }

        QImage get(const QString & argk) {
            std::shared_lock varLock{ mmm_Mutex };
            auto varPos = mmm_Map.find(argk);
            if (varPos == mmm_Map.end()) { return {}; }
            return varPos->second;
        }

        SSTD_MEMORY_DEFINE(Map)

    };

    Map * instanceMap() {
        static auto varAns = sstd::make_unique<Map>();
        return varAns.get();
    }

}/*namespace*/

sstd::QuickImageProvider::QuickImageProvider() : Super(QQmlImageProviderBase::Image) {

}

/** image://quickqimage/[a-z]{32}/ **/
QImage sstd::QuickImageProvider::requestImage(const QString &id,
                                              QSize *size,
                                              const QSize& requestedSize) {
    const auto varAns = instanceMap()->get(image_header()+id);
    if (size) { *size = varAns.size(); }
    return varAns;
    (void)requestedSize;
}

namespace {

    std::array< char16_t, 32 > getIndex() {
        static std::array< char16_t, 32 > varData{
            'z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z',
            'z','z','z','z','z','z','z','z',
        };
        class SpinMutex {
            std::atomic_flag flag { ATOMIC_FLAG_INIT };
        public:  SpinMutex() = default;
            SpinMutex(const SpinMutex&) = delete;
            SpinMutex& operator= (const SpinMutex&) = delete;
            inline void lock() {
                while (flag.test_and_set(std::memory_order_acquire));
            }
            inline void unlock() {
                flag.clear(std::memory_order_release);
            }
        };
        static SpinMutex varMutex;

        auto varPos = varData.begin();
        const auto varEnd = varData.end();

        std::unique_lock varLock{ varMutex };
        for (; varPos != varEnd; ++varPos) {
            if ('z' == *varPos) {
                *varPos = 'a';
                continue;
            }
            else {
                ++(*varPos);
                break;
            }
        }
        return varData;

    }

}/*namespace*/

QString sstd::QuickImageProvider::getIndexHeader() {
    return QStringLiteral("quickqimage");
}

QString sstd::QuickImageProvider::getNextIndexHeader() {
    const auto varIndex = getIndex();
    const auto varAns = image_header() +
            QString::fromUtf16(varIndex.data(), 32) ;
    return varAns;
}

void sstd::QuickImageProvider::addImage(const QString & argID, const QImage & argValue) {
    if ((argValue.width() < 1) || (argValue.height() < 1)) {
        instanceMap()->erase(argID);
    }
    else {
        instanceMap()->add(argID, argValue);
    }
}

sstd::QuickImageProvider * sstd::QuickImageProvider::instance() {
    auto varAns = sstd::make_unique<QuickImageProvider>();
    return varAns.release();
}

