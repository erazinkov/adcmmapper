#ifndef MAPPERPARSER_H
#define MAPPERPARSER_H

#include <QString>
#include <QCommandLineParser>

#include "mapperquery.h"


class MapperParser
{

public:
    MapperParser(QCommandLineParser &, MapperQuery &);

    bool parseResult();

private:
    struct CommandLineParseResult {
        enum class Status {
            Ok,
            Error,
            VersionRequested,
            HelpRequested
        };
        Status statusCode{Status::Ok};
        std::optional<QString> errorString{std::nullopt};
    };
    QCommandLineParser &parser_;
    MapperQuery &query_;

    CommandLineParseResult parseCommandLine();
};

#endif // MAPPERPARSER_H
