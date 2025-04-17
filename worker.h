#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "decoder.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(const QString &, const ChannelMap &);
public slots:
    void doWork(const QString &);

signals:
    void resultReady(const QString &);
private:
    ChannelMap m_pre;
    Decoder *m_decoder;

};

#endif // WORKER_H
