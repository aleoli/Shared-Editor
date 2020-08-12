#pragma once

#include <QObject>
#include <memory>

#include "File.h"
#include "FifoMap.h"

class DiskManager: public QObject {
    Q_OBJECT
public:
    DiskManager(const DiskManager &) = delete;
    DiskManager(DiskManager &&) = delete;
    DiskManager &operator=(const DiskManager &) = delete;
    DiskManager &operator=(DiskManager &&) = delete;

    static std::shared_ptr <DiskManager> get();

    ~DiskManager() override;

    void setFileMap(FifoMap<int, std::pair<QString, File>>* fileMap);

public slots:
    void save();

signals:
    void quit();

private:
    static std::shared_ptr<DiskManager> instance;
    explicit DiskManager(QObject *parent = nullptr);

    FifoMap<int, std::pair<QString, File>> *_fileMap = nullptr;
};
