#ifndef PARAMETERPARSER_H
#define PARAMETERPARSER_H

#include <QMap>
#include <QStringList>
#include <QCommandLineParser>

struct People {
    QString name;
    QString email;
    QString url;
    bool valid;

    People(const QString& src);

    QString dump() const {
        QStringList result;
        result << this->name;
        if (!this->email.isEmpty()) {
            result << QString("<%1>").arg(this->email);
        }
        if (!this->url.isEmpty()) {
            result << QString("(%1)").arg(this->url);
        }
        return result.join(" ");
    }
};

class QCommandLineParser;

class ParameterParser
{
public:
    enum SubCommand {
        CommandListAction,
        GlobalAction,
        InvalidAction,
        InstallAction,
        UninstallAction,
        UpdateAction,
        InitAppAction,
        InitLibAction,
        BuildAction,
        RebuildAction,
        RefreshAction,
        SearchAction,
        ConfigAction
    };

    ParameterParser();
    SubCommand parse(const QStringList &params, const QCoreApplication* app=nullptr);

    bool flag(const QString& flagName) const;
    const QMap<QString, bool>& flags() const;
    const QMap<QString, QString> &params() const;
    QString param(const QString& paramName) const;
    const QStringList &args() const;
    QList<People> people(const QString& category) const;
    QStringList dumpPeoples(const QString& category) const;

    bool commandHelpRequested() const;
    void showCommandHelp() const;

private:
    bool _commandHelpRequested;
    QStringList _args;
    QMap<QString, bool> _flags;
    QMap<QString, QString> _params;
    QMap<QString, QList<People> > _people;

    void _addModuleOptions(QCommandLineParser& parser) const;
    void _getModuleOptions(QCommandLineParser& parser);
};

#endif // PARAMETERPARSER_H
