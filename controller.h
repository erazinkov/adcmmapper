#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

#include "worker.h"
#include "channelmap.h"

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(const QString &, const ChannelMap &);
    ~Controller() override;

signals:
    void handleResults(const QString &);
signals:
    void operate(const QString &);
};

#endif // CONTROLLER_H
