#include "RootWindow.hpp"
#include <QtCore/qurl.h>
#include <QtCore/qdiriterator.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qregularexpression.h>
#include <QtConcurrent>
#include <QtCore/qlist.h>
#include <memory>

namespace {
    inline int constexpr get_fileName_role() {
        return Qt::UserRole + 16;
    }
}/**/

RootWindow::RootWindow(QObject *parent) : Super(parent) {

}

void RootWindow::checkTheDir(QVariant arg) {

    QUrl varDir = arg.value<QUrl>();
    if (varDir.isValid() == false) { return; }

    QDirIterator varIt{
        varDir.toLocalFile(),
        QDir::Files,
        QDirIterator::Subdirectories
    };

    QList<QFileInfo> varAddBomInfo;
    QList<QFileInfo> varRemoveBomInfo;
    const static QRegularExpression varHasBomType{ QStringLiteral(
    R"(^(h|hpp|c|cpp|qml)$)"),QRegularExpression::CaseInsensitiveOption };
    const static QRegularExpression varIgnorType{ QStringLiteral(
    R"(^(user|log)$)"),QRegularExpression::CaseInsensitiveOption };
    const static QRegularExpression varIgnorHasBomType{ QStringLiteral(
    R"(^(moc_|qrc_)[^/]*)"),QRegularExpression::CaseInsensitiveOption };
    while (varIt.hasNext()) {
        varIt.next();
        const auto varFileInfo = varIt.fileInfo();
        const auto varSuffix = varFileInfo.suffix();
        if (varIgnorType.match(varSuffix).hasMatch()) { continue; }
        if (varHasBomType.match(varSuffix).hasMatch()) {
            if (varIgnorHasBomType.match(varFileInfo.fileName()).hasMatch()) {
                continue;
            }
            varAddBomInfo.push_back(varFileInfo);
        }
        else {
            varRemoveBomInfo.push_back(varFileInfo);
        }
    }

    const static constexpr char globalBom[] = "\xEF\xBB\xBF";

    auto varNotAddBomInfo = QtConcurrent::filtered(varAddBomInfo, [](const QFileInfo & arg)->bool {
        /*find the file not has utf8 bom*/
        QFile varFile{ arg.absoluteFilePath() };
        varFile.open(QIODevice::ReadOnly);
        alignas(4) char varBom[4];
        const auto varSize = varFile.read(varBom, 3);
        if (varSize < 3) { return true; }
        return (0 != std::memcmp(globalBom, varBom, 3));
    });

    auto varNotRemoveBomInfo = QtConcurrent::filtered(varRemoveBomInfo, [](const QFileInfo & arg)->bool {
        /*find the file has utf8 bom*/
        QFile varFile{ arg.absoluteFilePath() };
        varFile.open(QIODevice::ReadOnly);
        alignas(4) char varBom[4];
        const auto varSize = varFile.read(varBom, 3);
        if (varSize < 3) { return false; }
        return (0 == std::memcmp(globalBom, varBom, 3));
    });

    varAddBomInfo = varNotAddBomInfo.results();
    varRemoveBomInfo = varNotRemoveBomInfo.results();

    for (const auto & varI : std::as_const(varAddBomInfo)) {
        _m_add_bom.push_back(varI.canonicalFilePath());
    }

    for (const auto & varI : std::as_const(varRemoveBomInfo)) {
        _m_remove_bom.push_back(varI.canonicalFilePath());
    }

    finishedCheckTheDir();

}


QVariant RootWindow::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        if (section == 0) {
            return QStringLiteral("序列");
        }
    }

    return {};
}

QModelIndex RootWindow::index(int row, int column, const QModelIndex &parent) const {
    if (column > 0) { return {}; }

    if ((parent.isValid() == false) || (parent.internalId() == 0)) {
        if (row > 1) { return {}; }
        return this->createIndex(row, column, static_cast<std::uintptr_t>(1));
    }

    if (parent.internalId() == 1) {
        if (parent.row() > 1) { return {}; }
        if (parent.row() == 0) {
            if (row >= _m_add_bom.size()) { return {}; }
        }
        else {
            if (row >= _m_remove_bom.size()) { return {}; }
        }
        return this->createIndex(row, column, static_cast<std::uintptr_t>(parent.row() ? 3 : 2));
    }
    return {};
}

QModelIndex RootWindow::parent(const QModelIndex &index) const {
    if (index.internalId() == 1) { return this->createIndex(0, 0, static_cast<std::uintptr_t>(0)); }
    if (index.internalId() == 2) { return this->createIndex(0, 0, static_cast<std::uintptr_t>(1)); }
    if (index.internalId() == 3) { return this->createIndex(1, 0, static_cast<std::uintptr_t>(1)); }
    return this->createIndex(0, 0, static_cast<std::uintptr_t>(0));
}

int RootWindow::rowCount(const QModelIndex &parent) const {

    if (parent.internalId() == 0) { return 2; }
    if (parent.internalId() == 1) {
        if (parent.row() == 0) { return _m_add_bom.count(); }
        return _m_remove_bom.count();
    }
    return 0;
}

int RootWindow::columnCount(const QModelIndex &parent) const {
    if (parent.internalId() == 0) { return 1; }
    if (parent.internalId() == 1) { return 1; }
    return 0;
}

QVariant RootWindow::data(const QModelIndex &index, int role) const {
    if (false == index.isValid()) { return {}; }

    if ((role == Qt::DisplayRole) || (role == get_fileName_role())) {
        if (index.internalId() == 1) {
            if (index.row() == 0) {
                return QStringLiteral(R"(需要添加BOM)");
            }
            else if (index.row() == 1) {
                return QStringLiteral(R"(需要删除BOM)");
            }
        }
        else if (index.internalId() == 2) {
            if (index.row() < _m_add_bom.size()) {
                return _m_add_bom[index.row()];
            }
        }
        else if (index.internalId() == 3) {
            if (index.row() < _m_remove_bom.size()) {
                return _m_remove_bom[index.row()];
            }
        }
    }

    return {};

}

QHash<int, QByteArray> RootWindow::roleNames() const {
    const static QHash<int, QByteArray> varAns = []() {
        QHash<int, QByteArray> varAns;
        varAns.insert(get_fileName_role(), "fileName");
        return std::move(varAns);
    }();
    return varAns;
}


