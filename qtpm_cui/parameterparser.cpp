#include "parameterparser.h"
#include <QDebug>
#include <iostream>
#include <QRegExp>


static const char* defaultDescription = \
        "\nQt library management tool. It provides features like:\n\n"
        "* Getting library and install to your work folder\n"
        "* Initialize library project\n\nand so on.";

static const char* buildDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm build <options>\n";


static const char* installDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm install\n"
        "    qtpm install <pkg>\n"
        "    qtpm install <pkg>@<version>\n"
        "    qtpm install <folder>\n"
        "    qtpm install <tarball file/url>\n"
        "    qtpm install <git:// url>\n"
        "    qtpm install <git:// url>#<tag/branch>\n"
        "    qtpm install <git:// url>@<version>\n"
        "    qtpm install <github url>/<github username>/<github project>\n"
        "    qtpm install <github url>/<github username>/<github project>#<tag/branch>\n"
        "    qtpm install <github url>/<github username>/<github project>@<version>\n";

static const char* uninstallDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm uninstall <pkg>\n";

static const char* updateDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm update\n"
        "    qtpm update <pkg>\n";

static const char* refreshDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm refresh : refresh alias information.\n";

static const char* searchDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm search <word>         : search word from alias name, url, description.\n"
        "    qtpm search --title <word> : search alias names that have near name with word.\n";

static const char* configDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm config                  : show existing all config values.\n"
        "    qtpm config <config>         : show specified config value.\n"
        "    qtpm config <config> <value> : set config value.\n"
        "\n"
        "Available Configs:\n"
        "    qtdir\n";

static const char* cleanDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm clean : remove files/directories made by qtpm command.\n";

static const char* initDescription = \
        "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014 \n"
        "\n"
        "    qtpm init\n"
        "\n"
        "Available Licenses:\n"
        "    Apache\n"
        "    Artistic\n"
        "    BSD-2\n"
        "    BSD-3\n"
        "    EPL\n"
        "    AGPL\n"
        "    FDL-v1.3\n"
        "    GPL-v1.0\n"
        "    GPL-v2.0\n"
        "    GPL-v3.0\n"
        "    LGPL-v2.1\n"
        "    LGPL-v3.0\n"
        "    MIT\n"
        "    MPL\n"
        "    Unlicense\n";

ParameterParser::ParameterParser()
    : _commandHelpRequested(false)
{
}

ParameterParser::SubCommand ParameterParser::parse(const QStringList &params, const QCoreApplication* app)
{
    QCommandLineParser parser;
    const auto helpOption = parser.addHelpOption();
    QCommandLineOption commandHelpOption(QStringList() << "command-help", "Displays command list.");
    parser.addOption(commandHelpOption);
    parser.addPositionalArgument("command", "The command to execute.");
    parser.parse(params);

    const QStringList initialArgs = parser.positionalArguments();
    const QString command = initialArgs.isEmpty() ? QString() : initialArgs.first();

    if (command == "install") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("install", "Install library.", "install [install_options] [install modules]");
        const QCommandLineOption qtdirOption("qtdir", "qt tool/library path to build library.", "qtdir");
        parser.addOption(qtdirOption);
        const QCommandLineOption bundleOption("bundle-only", "install only bundle libraries.");
        parser.addOption(bundleOption);
        const QCommandLineOption saveOption("save", "register the specified module to qtpackage.ini as a bundle module.");
        parser.addOption(saveOption);
        const QCommandLineOption saveDevOption("save-dev", "register the specified module to qtpackage.ini as a module for development.");
        parser.addOption(saveDevOption);
        const QCommandLineOption updateOption("update", "update existing modulues.");
        parser.addOption(updateOption);
        const QCommandLineOption verboseOption(QStringList() << "verbose" << "v", "show detail message.");
        parser.addOption(verboseOption);
        parser.setApplicationDescription(installDescription);

        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_args = parser.positionalArguments().mid(1);
        this->_params.insert("qtdir", parser.value(qtdirOption));
        this->_flags.insert("bundle-only", parser.isSet(bundleOption));
        this->_flags.insert("save", parser.isSet(saveOption));
        this->_flags.insert("save-dev", parser.isSet(saveDevOption));
        this->_flags.insert("update", parser.isSet(updateOption));
        this->_flags.insert("verbose", parser.isSet(verboseOption));
        return ParameterParser::InstallAction;
    } else if (command == "uninstall") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("uninstall", "Uninstall library.", "uninstall [uninstall_options] [uninstall modules]");
        const QCommandLineOption removeOption("remove", "remove the specified module from qtpackage.ini.");
        parser.addOption(removeOption);
        parser.setApplicationDescription(uninstallDescription);

        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_args = parser.positionalArguments().mid(1);
        this->_flags.insert("remove", parser.isSet(removeOption));
        return ParameterParser::UninstallAction;
    } else if (command == "update") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("update", "Update library.", "update [update_options] [install modules]");
        parser.setApplicationDescription(updateDescription);

        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_args = parser.positionalArguments().mid(1);
        return ParameterParser::UpdateAction;
    } else if (command == "build") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("build", "Build library.", "build [build_options]");
        parser.setApplicationDescription(buildDescription);
        const QCommandLineOption qtdirOption("qtdir", "qt tool/library path to build library.", "qtdir");
        parser.addOption(qtdirOption);
        const QCommandLineOption buildTypeOption(QStringList() << "build" << "b", "Build type: source, configure, scons.", "build");
        parser.addOption(buildTypeOption);
        const QCommandLineOption buildOptionOption(QStringList() << "build-option" << "o", "Build option like: --enable-static", "buildOption");
        parser.addOption(buildOptionOption);
        const QCommandLineOption saveOption(QStringList() << "save" << "s", "Save build param or/and build-option to setting file.");
        parser.addOption(saveOption);
        const QCommandLineOption verboseOption(QStringList() << "verbose" << "v", "show detail message.");
        parser.addOption(verboseOption);

        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_params.insert("qtdir", parser.value(qtdirOption));
        this->_params.insert("buildType", parser.value(buildTypeOption));
        this->_params.insert("buildOption", parser.value(buildOptionOption));
        this->_flags.insert("save", parser.isSet(saveOption));
        this->_flags.insert("verbose", parser.isSet(verboseOption));
        return ParameterParser::BuildAction;
    } else if (command == "init-lib") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("init-lib", "Initlize library project.", "init-lib [initlib_options]");
        const QCommandLineOption nameOption("name", "Package name. It will be identity of package", "name");
        parser.addOption(nameOption);
        const QCommandLineOption typeOption("type", "Library type: bundle, dev.", "type");
        parser.addOption(typeOption);
        const QCommandLineOption buildTypeOption(QStringList() << "build" << "b", "Build type: source, configure, cmake.", "build");
        parser.addOption(buildTypeOption);
        const QCommandLineOption buildOptionOption(QStringList() << "build-option" << "o", "Build option like: --enable-static", "buildOption");
        parser.addOption(buildOptionOption);
        this->_addModuleOptions(parser);
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_getModuleOptions(parser);
        this->_args = parser.positionalArguments().mid(1);
        this->_params.insert("name", parser.value(nameOption));
        this->_params.insert("type", parser.value(typeOption));
        this->_params.insert("buildType", parser.value(buildTypeOption));
        this->_params.insert("buildOption", parser.value(buildOptionOption));
        return ParameterParser::InitLibAction;
    } else if (command == "init") {
        parser.clearPositionalArguments();
        parser.setApplicationDescription(initDescription);
        parser.addPositionalArgument("init", "Initlize application project.", "init [initapp_options]");
        this->_addModuleOptions(parser);
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_getModuleOptions(parser);
        return ParameterParser::InitAppAction;
    } else if (command == "refresh") {
        parser.clearPositionalArguments();
        parser.setApplicationDescription(refreshDescription);
        parser.addPositionalArgument("refresh", "Initlize application project.", "refresh");
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        return ParameterParser::RefreshAction;
    } else if (command == "config") {
        parser.clearPositionalArguments();
        parser.setApplicationDescription(configDescription);
        parser.addPositionalArgument("config", "Get/set qtpm config.", "config");
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_args = parser.positionalArguments().mid(1);
        return ParameterParser::ConfigAction;
    } else if (command == "clean") {
        parser.clearPositionalArguments();
        parser.setApplicationDescription(cleanDescription);
        parser.addPositionalArgument("clean", "remove files/directories created by qtpm command", "config");
        const QCommandLineOption verboseOption(QStringList() << "verbose" << "v", "show detail message.");
        parser.addOption(verboseOption);
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_flags.insert("verbose", parser.isSet(verboseOption));
        return ParameterParser::CleanAction;
    } else if (command == "search") {
        parser.clearPositionalArguments();
        parser.setApplicationDescription(searchDescription);
        parser.addPositionalArgument("search", "Search module alias", "search [search_options] search_word");
        const QCommandLineOption nameOption("name", "Search near names from passed word.");
        parser.addOption(nameOption);
        const QCommandLineOption distanceOption(QStringList() << "distance" << "d", "Levenshtein distance for filtering the result.", "distance", "4");
        parser.addOption(distanceOption);
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        this->_args = parser.positionalArguments().mid(1);
        if (this->_args.length() != 1) {
            std::cerr << "search command can have only one search word." << std::endl;
            parser.showHelp();
            return ParameterParser::InvalidAction;
        } else {
            this->_flags.insert("name", parser.isSet(nameOption));
            this->_params.insert("distance", parser.value(distanceOption));
            return ParameterParser::SearchAction;
        }
    } else if (command == "command-help") {
        const QCommandLineOption commandHelpOption(QStringList() << "command-help", "Displays command list.");
        parser.addOption(commandHelpOption);
        if (app) {
            parser.process(*app);
        } else {
            parser.process(params);
        }
        //qDebug() << "positionArguments:" << parser.positionalArguments();
        //qDebug() << "optionNames:" << parser.optionNames();
        if (parser.isSet(commandHelpOption)) {
            return ParameterParser::CommandListAction;
        }
    } else if (command == "") {
        if (parser.isSet(helpOption)) {
            parser.setApplicationDescription(defaultDescription);
            parser.showHelp();
        }

        return ParameterParser::CommandListAction;
    }
    return ParameterParser::InvalidAction;
}

bool ParameterParser::flag(const QString &flagName) const
{
    return this->_flags.value(flagName, false);
}

const QMap<QString, QString> &ParameterParser::params() const
{
    return this->_params;
}

QString ParameterParser::param(const QString &paramName) const
{
    return this->_params.value(paramName, QString());
}

const QStringList &ParameterParser::args() const
{
    return this->_args;
}

QList<People> ParameterParser::people(const QString &category) const
{
    return this->_people.value(category, QList<People>());
}

QStringList ParameterParser::dumpPeoples(const QString &category) const
{
    QList<People> peoples = this->people(category);
    QStringList output;
    foreach(const People& people, peoples) {
        output.append(people.dump());
    }
    return output;
}

bool ParameterParser::commandHelpRequested() const
{
    return this->_commandHelpRequested;
}

void ParameterParser::showCommandHelp() const
{
    std::cout << "qtpm: C++ Package Manager for Qt projects - Yoshiki Shibukawa, 2014" << std::endl
              << std::endl
              << "Usage: qtpm <command> <command option>" << std::endl
              << std::endl
              << "where <command> is one of:" << std::endl
              << "    build     : build module" << std::endl
              << "    init      : create setting file for your awesome application" << std::endl
              << "    init-lib  : create setting file for module" << std::endl
              << "    install   : install module" << std::endl
              << "    uninstall : uninstall module" << std::endl
              << "    refresh   : refresh module alias information" << std::endl
              << "    search    : search module alias" << std::endl
              << "    config    : set/get qtpm configuration" << std::endl
              << "    clean     : clean work folders and build result" << std::endl
              << std::endl
              << "qtpm <cmd> -h     quick help on <cmd>" << std::endl;
}

void ParameterParser::_addModuleOptions(QCommandLineParser &parser) const
{
    const QCommandLineOption authorOption("author", "author name. It can have extra information by using \"name <email> (url)\" format.", "author");
    parser.addOption(authorOption);
    const QCommandLineOption committerOption("committer", "committer name. It can have extra information by using \"name <email> (url)\" format.", "author");
    parser.addOption(committerOption);
    const QCommandLineOption contributorOption("contributor", "contributor name. It can have extra information by using \"name <email> (url)\" format.", "contributor");
    parser.addOption(contributorOption);
    const QCommandLineOption licenseOption("license", "license name. If you use the name in license list, this command generates LICENSE.rst too", "license");
    parser.addOption(licenseOption);
    const QCommandLineOption descriptionOption("description", "description of this software", "description");
    parser.addOption(descriptionOption);
    const QCommandLineOption homepageOption("homepage", "homepage URL", "homepage");
    parser.addOption(homepageOption);
    const QCommandLineOption repositoryOption("repository", "repository URL.", "repository");
    parser.addOption(repositoryOption);
    const QCommandLineOption bugsOption("bugs", "issue tracker URL", "bugs");
    parser.addOption(bugsOption);
    const QCommandLineOption githubOption("github", "github page. It is a persudo command to set homepage/repository/bugs at once.", "github");
    parser.addOption(githubOption);
}

void ParameterParser::_getModuleOptions(QCommandLineParser &parser)
{
    this->_params.insert("description", parser.value("description"));
    this->_params.insert("license", parser.value("license"));
    this->_params.insert("homepage", parser.value("homepage"));
    this->_params.insert("repository", parser.value("repository"));
    this->_params.insert("bugs", parser.value("bugs"));

    if (parser.isSet("github")) {
        QString githubUrl = parser.value("github");
        if (!githubUrl.startsWith("https://")) {
            githubUrl = QString("https://") + githubUrl;
        }
        this->_params.insert("homepage", githubUrl);
        this->_params.insert("repository", githubUrl + ".git");
        this->_params.insert("bugs", githubUrl + "/issues");
    }

    QList<People> authors;
    foreach(const QString& src, parser.values("author")) {
        People author(src);
        if (author.valid) {
            authors.append(author);
        }
    }
    this->_people.insert("authors", authors);

    QList<People> committers;
    foreach(const QString& src, parser.values("committer")) {
        People committer(src);
        if (committer.valid) {
            committers.append(committer);
        }
    }
    this->_people.insert("committers", committers);

    QList<People> contributors;
    foreach(const QString& src, parser.values("contributor")) {
        People contributor(src);
        if (contributor.valid) {
            contributors.append(contributor);
        }
    }
    this->_people.insert("contributors", contributors);
}


People::People(const QString &src)
{
    QRegExp person("([^<\\(]+)\\s*(<(.+)>)?\\s*(\\((.+)\\))?");
    if (person.indexIn(src) == -1) {
        this->valid = false;
    } else {
        this->valid = true;
        this->name = person.cap(1).trimmed();
        this->email = person.cap(3).trimmed();
        this->url = person.cap(5);
    }
}
