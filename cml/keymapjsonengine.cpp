#include "keymapjsonengine.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

KeyMapJsonEngine::KeyMapJsonEngine()
{

}

QString KeyMapJsonEngine::keyMapToJson(QMap<int, KeyMap> keyMap)
{
    if(keyMap.count()<1){
#ifdef DEBUG_QML_SHOW_INFO
    qDebug() <<"KeyMapJsonEngine::keyMapToJson: keyMap.count=0";
#endif
        return QString();
    }

    QJsonDocument keyMapJsonDoc;
    QJsonObject keyMapJsonObj;
    QJsonObject keyMapDataJsonObj;
    keyMapJsonObj.insert("time", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    keyMapJsonObj.insert("keys_count", keyMap.count());
    keyMapJsonObj.insert("jsonengine_version", KEYMAP_JSON_ENGINE_VERSION);
    keyMapJsonObj.insert("keycontainer_version", ABOUT_VERSION);
    keyMapJsonObj.insert("keycontainer_platform", ABOUT_PLANTFORM);
    keyMapJsonObj.insert("keycontainer_compiler_type", ABOUT_BASE_COMPILER_TYPE);
#ifdef ABOUT_BASE_COMPILER_STRING
    keyMapJsonObj.insert("keycontainer_compiler_version", ABOUT_BASE_COMPILER);
#else
    keyMapJsonObj.insert("keycontainer_compiler_version", QString::number(ABOUT_BASE_COMPILER));
#endif

    QMap<int, KeyMap>::const_iterator iter = keyMap.constBegin();
    while(iter != keyMap.constEnd()){
        QJsonObject keyJsonObj;
        keyJsonObj.insert("id", iter.key());
        keyJsonObj.insert("disc", iter.value().disc.getVal());
        keyJsonObj.insert("account", iter.value().account.getVal());
        keyJsonObj.insert("password", iter.value().password.getVal());
        keyMapDataJsonObj.insert(QString::number(iter.key()), keyJsonObj);

        iter++;
    }
    keyMapJsonObj.insert("keys_data", keyMapDataJsonObj);
    keyMapJsonDoc.setObject(keyMapJsonObj);
    QString result;
    result = keyMapJsonDoc.toJson();
    return result;
}

QMap<int, KeyMap> KeyMapJsonEngine::jsonToKeyMap(QString json)
{
    QMap<int, KeyMap> keyMap;
    QJsonDocument keyMapJsonDoc;
    QJsonObject keyMapJsonObj;
    QJsonObject keyMapDataJsonObj;
    QJsonParseError keyMapJsonErr;
    keyMapJsonDoc = QJsonDocument::fromJson(json.toUtf8().data(), &keyMapJsonErr);
    if(keyMapJsonDoc.isNull()){
        qDebug() << "keyMapJsonDoc invalid:" << keyMapJsonErr.errorString();
        return keyMap;
    }
    keyMapJsonObj = keyMapJsonDoc.object();
    keyMapDataJsonObj = keyMapJsonObj.value("keys_data").toObject();
    QStringList keyNumberList = keyMapDataJsonObj.keys();
    foreach(QString keyNumber, keyNumberList){
        QJsonObject keyJsonObj = keyMapDataJsonObj.value(keyNumber).toObject();
        keyMap.insert(keyNumber.toInt(),KeyMap(static_cast<quint32>(keyJsonObj.value("id").toInt()),
                                            Estring(keyJsonObj.value("disc").toString()),
                                            Estring(keyJsonObj.value("account").toString()),
                                            Estring(keyJsonObj.value("password").toString())));
    }
    return keyMap;
}
