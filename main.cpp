#include <QCoreApplication>
#include <QTimer>
#include <QCommandLineParser>

#include "decoder.h"
#include "mapperparser.h"
#include "mapperquery.h"

void process(const MapperQuery &query);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer::singleShot(0, [] ()
    {
        QCommandLineParser parser;
        MapperQuery query;
        MapperParser mapperParser(parser, query);
        auto parseResult = mapperParser.parseResult();
        if (parseResult)
        {
            process(query);
        }
        QCoreApplication::exit(0);
    });

    return a.exec();
}

void process(const MapperQuery &query)
{
    ChannelMap pre{ChannelMap::mapNAP()};

//    QString path = "/home/egor/build-adcmmodifier-Desktop-Debug/adcm.dat.mod";
    QString inputPath{query.input}, outputPath{query.output};
    Decoder *decoder = new Decoder(inputPath.toStdString(), outputPath.toStdString(), pre);
    decoder->process();
}
