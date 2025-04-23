#include <QCoreApplication>
#include <QTimer>
#include <QCommandLineParser>

#include "decoder.h"
#include "mapperparser.h"
#include "mapperquery.h"

#include <QDateTime>

#include <chrono>

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

std::chrono::system_clock::time_point timePointFromGeo(const std::string &str)
{
    auto posNs = str.find_last_of(".");
    if (posNs == str.npos)
    {
        posNs = str.length();
    }
    else
    {
        posNs += 1;
    }
    std::string strNs{str.substr(posNs, str.length())};
    const auto reqPre{9};
    const auto curPre{strNs.length()};
    for (size_t i{0}; i < reqPre - curPre; ++i)
    {
        strNs += '0';
    }
    std::cout << strNs << std::endl;
    std::tm tm = {};
    std::stringstream ss{str};
    ss >> std::get_time(&tm, "%Y-%m-%d  %H:%M:%S");
    std::chrono::system_clock::time_point tp{std::chrono::system_clock::from_time_t(std::mktime(&tm))};
    tp += std::chrono::nanoseconds(std::atoll(strNs.c_str()));
    return tp;
}

std::chrono::system_clock::time_point timePointFromMap(const std::string &str)
{
    std::chrono::system_clock::time_point tp{std::chrono::nanoseconds(std::atoll(str.c_str()))};
    return tp;
}

void printTimePoint(const std::chrono::system_clock::time_point &timePoint)
{
    std::time_t tt{std::chrono::system_clock::to_time_t(timePoint)};
    std::cout << "time_point tp is: " << ctime(&tt);
}

void process(const MapperQuery &query)
{
    ChannelMap pre{ChannelMap::mapNAP()};

    QString path = "/home/egor/build-adcmmodifier-Desktop-Debug/adcm.dat.mod";
    QString inputPath{query.input}, outputPath{query.output};
    Decoder *decoder = new Decoder(inputPath.toStdString(), outputPath.toStdString(), pre);
    decoder->process();
}
