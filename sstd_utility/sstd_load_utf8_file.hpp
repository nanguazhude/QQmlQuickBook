#pragma once 

#include <fstream>
#include <filesystem>

#if defined(QT_CORE_LIB)
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#endif

#include "sstd_memory.hpp"

namespace sstd {

/*读取一个utf8文件，如果有bom则去除，并为文件最后加一行空行*/
inline sstd::string load_file_remove_utf8(const std::filesystem::path & arg,
    char * varTmpBuffer = nullptr,
    std::streamsize varTmpBufferSize = 0) {
    std::ifstream varStream{ arg,std::ios::binary };
    if (false == varStream.is_open()) { return {}; }
    constexpr const static int varSize = 1024;
    using _tmp_buffer_type = std::array<char, varSize>;
    class tmp_buffer_type : public _tmp_buffer_type {
        SSTD_MEMORY_DEFINE(tmp_buffer_type)
    };
    std::unique_ptr< tmp_buffer_type> varTmpBufferTmp;
    do {
        if (varTmpBuffer && (varTmpBufferSize >= 16)) { break; }
        varTmpBufferTmp = sstd::make_unique<tmp_buffer_type>();
        varTmpBuffer = varTmpBufferTmp->data();
        varTmpBufferSize = varSize;
    } while (false);

    bool varFirstRead = true;
    const static constexpr char globalBom[] = "\xEF\xBB\xBF";
    sstd::string varAns;
    while (varStream.good()) {
        varStream.read(varTmpBuffer, varTmpBufferSize);
        const auto varRead = varStream.gcount();
        if (varRead < 1) { break; }
        std::string_view varDataOut = { varTmpBuffer,
            static_cast<std::string_view::size_type>(varRead) };
        if (varFirstRead) {/*remove bom*/
            varFirstRead = false;
            if (varRead > 2) {
                if ((globalBom[0] == varTmpBuffer[0]) &&
                    (globalBom[1] == varTmpBuffer[1]) &&
                    (globalBom[2] == varTmpBuffer[2])) {
                    varDataOut = varDataOut.substr(3);
                }
            }
        }
        varAns += varDataOut;
    }
    varAns += "\n"sv;
    return std::move(varAns);
}

#if defined(QT_CORE_LIB)

inline sstd::string load_file_remove_utf8(const QString & a,
    char * b = nullptr,
    std::streamsize c = 0){
    const auto varU8D = a.toUtf8();
    const auto varU8Path = std::filesystem::u8path(
        varU8D.data(),
        varU8D.data()+varU8D.size()
     );
    return load_file_remove_utf8( varU8Path,b,c );
}

#else

#endif

}/*namespace sstd*/

