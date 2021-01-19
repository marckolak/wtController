#include "settings.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

QString Settings::host = "192.168.0.200";

int Settings::port = 4321;


Settings::Settings()
{

}


bool Settings::loadSettings()
{
    // get absolute settings filepath
    QString path = QCoreApplication::applicationDirPath();
    QFile loadFile(path+"/settings.json");

    qDebug() << path+"/settings.json";

    // if not opened correctly return
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open settings file - loading default values");
        return false;
    }

    // read file contents into byte array
    QByteArray settingsData = loadFile.readAll();

    // load data as json document
    QJsonDocument loadDoc(QJsonDocument::fromJson(settingsData));

    // convert json to object
    QJsonObject json = loadDoc.object();

    // get properties fron json object
    if (json.contains("host") && json["host"].isString())
        host = json["host"].toString();

    if (json.contains("port") && json["port"].isDouble())
        port = json["port"].toInt();

    return true;

}
