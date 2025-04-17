#include "worker.h"

Worker::Worker(const QString &path, const ChannelMap &pre) : m_pre(pre)
{
    m_decoder = new Decoder(path.toStdString(), m_pre);
}

void Worker::doWork(const QString &parameter) {
    const auto start = std::chrono::steady_clock::now();
    m_decoder->positionsOfCMAPHeadersAndNDHeaders();
    const auto stop = std::chrono::steady_clock::now();
    std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;
    emit(resultReady("Result ready"));
}
