#include "mapperparser.h"

MapperParser::MapperParser(QCommandLineParser &parser, MapperQuery &query) : parser_(parser), query_(query)
{
}

bool MapperParser::parseResult()
{
    using Status = CommandLineParseResult::Status;

    auto r{false};

    auto parseCommandLineResult{parseCommandLine()};
    switch (parseCommandLineResult.statusCode) {
        case Status::Ok:
        {
            r = true;
            break;
        }
        case Status::Error:
        {
            std::fputs(qPrintable(parseCommandLineResult.errorString.value_or("Unknown error occurred")),
                       stderr);
            std::fputs("\n\n", stderr);
            std::fputs(qPrintable(parser_.helpText()), stderr);
            break;
        }
        case Status::VersionRequested:
            parser_.showVersion();
        case Status::HelpRequested:
            parser_.showHelp();
    }

    return r;
}

MapperParser::CommandLineParseResult MapperParser::parseCommandLine()
{
    using Status = CommandLineParseResult::Status;

    parser_.setApplicationDescription("Adcm data mapping program.");
    parser_.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption aOption("a", "any (unused)", "a");

    parser_.addOption(aOption);

    parser_.addPositionalArgument("input", "Input file name.");
    parser_.addPositionalArgument("output", "Output file name.");
    const QCommandLineOption helpOption = parser_.addHelpOption();
    const QCommandLineOption versionOption = parser_.addVersionOption();

    if (!parser_.parse(QCoreApplication::arguments()))
        return { Status::Error, parser_.errorText() };

    if (parser_.isSet(versionOption))
        return { Status::VersionRequested };

    if (parser_.isSet(helpOption))
        return { Status::HelpRequested };

//    if (parser_.isSet(aOption)) {
//        bool ok;
//        query_.a = parser_.value(aOption).toUInt(&ok);
//        if (query_.a < 0 || !ok)
//        {
//            return { Status::Error, QString("Incorrect a number: %1").arg(query_.a) };
//        }
//    }

    const QStringList positionalArguments = parser_.positionalArguments();
    if (positionalArguments.isEmpty() || positionalArguments.size() < 1)
    {
        return { Status::Error, "Argument 'input' required." };
    }
    if (positionalArguments.size() > 3)
    {
        return { Status::Error, "Several 'input' or 'output' arguments specified." };
    }
    query_.input = positionalArguments.first();
    query_.output = positionalArguments.first() + ".map";
    if (positionalArguments.size() == 2)
    {
        query_.output = positionalArguments.last();
    }


    return { Status::Ok };
}
