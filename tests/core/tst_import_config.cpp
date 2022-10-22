#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <catch2/catch.hpp>

#include "importconfig.hpp"


TEST_CASE("ImportConfig Default Constructor", "[core]")
{
    ImportConfig config;
    CHECK(!config.hasTime());
    CHECK(config.getSeparatorChar() == ';');
    CHECK(config.getDecimalChar() == ',');
    CHECK(config.getNbLinesToSkipStart() == 0);
    CHECK(config.getNbLinesToSkipEnd() == 0);
    CHECK(config.getDateFormat() == "dd/MM/yy");
    CHECK(config.getTimeFormat() == "HH.mm.ss.zzz");
    CHECK(config.getName().isEmpty());
}

TEST_CASE("ImportConfig Setter and Getter", "[core]")
{
    ImportConfig config;
    config.setHasTime(true);
    CHECK(config.hasTime());

    config.setSeparatorChar('#');
    CHECK(config.getSeparatorChar() == '#');

    config.setDecimalChar('.');
    CHECK(config.getDecimalChar() == '.');

    config.setNbLinesToSkipStart(5);
    CHECK(config.getNbLinesToSkipStart() == 5);

    config.setNbLinesToSkipEnd(1);
    CHECK(config.getNbLinesToSkipEnd() == 1);

    config.setDateFormat("dd-MM-yyyy");
    CHECK(config.getDateFormat() == "dd-MM-yyyy");

    config.setTimeFormat("HH:mm:ss");
    CHECK(config.getTimeFormat() == "HH:mm:ss");

    config.setName("CaisseEpargne");
    CHECK(config.getName() == "CaisseEpargne");
}

TEST_CASE("ImportConfig WRITE to JSON", "[core]")
{
    // create an Object to save in JSON format
    ImportConfig config;
    config.setUid(QUuid("67C8770B-44F1-410A-AB9A-F8B5446F13FF"));
    config.setHasTime(true);
    config.setSeparatorChar('#');
    config.setDecimalChar('.');
    config.setNbLinesToSkipStart(5);
    config.setNbLinesToSkipEnd(1);
    config.setDateFormat("dd-MM-yyyy");
    config.setTimeFormat("HH:mm:ss");
    config.setName("CaisseEpargne_CSV1");
    QMap<QString, int> columns;
    columns.insert("C1", 1);
    columns.insert("C2", 2);
    columns.insert("C3", 3);
    QMapIterator<QString, int> mapIt(columns);
    while (mapIt.hasNext()) {
        mapIt.next();
        config.addColumn(mapIt.key(), mapIt.value());
    }

    QJsonObject jsonData;
    config.write(jsonData);

    CHECK((jsonData.contains("uid") && !jsonData["uid"].isNull()));
    CHECK((jsonData.contains("hasTime") && jsonData["hasTime"].toBool() == true));
    CHECK((jsonData.contains("separatorChar") && jsonData["separatorChar"].toString() == "#"));
    CHECK((jsonData.contains("decimalChar") && jsonData["decimalChar"].toString() == "."));
    CHECK(jsonData.contains("nbLinesToSkipStart"));
    CHECK(jsonData["nbLinesToSkipStart"].toInt() == 5);
    CHECK(jsonData.contains("nbLinesToSkipEnd"));
    CHECK(jsonData["nbLinesToSkipEnd"].toInt() == 1);
    CHECK(jsonData.contains("dateFormat"));
    CHECK(jsonData["dateFormat"].toString() == "dd-MM-yyyy");
    CHECK(jsonData.contains("timeFormat"));
    CHECK(jsonData["timeFormat"].toString() == "HH:mm:ss");
    CHECK(jsonData.contains("name"));
    CHECK(jsonData["name"].toString() == "CaisseEpargne_CSV1");

    CHECK((jsonData.contains("columns") && jsonData["columns"].isObject()));
    QJsonObject mapObject = jsonData["columns"].toObject();
    CHECK(mapObject.keys()[0] == "C1");
    CHECK(mapObject.keys()[1] == "C2");
    CHECK(mapObject.keys()[2] == "C3");
    CHECK(mapObject["C1"].toInt() == 1);
    CHECK(mapObject["C2"].toInt() == 2);
    CHECK(mapObject["C3"].toInt() == 3);
}

TEST_CASE("ImportConfig READ from JSON", "[core]")
{
    // create an Object to save in JSON format
    ImportConfig config;
    config.setUid(QUuid("67C8770B-44F1-410A-AB9A-F8B5446F13FF"));
    config.setHasTime(true);
    config.setSeparatorChar('#');
    config.setDecimalChar('.');
    config.setNbLinesToSkipStart(5);
    config.setNbLinesToSkipEnd(1);
    config.setDateFormat("dd-MM-yyyy");
    config.setTimeFormat("HH:mm:ss");
    config.setName("CaisseEpargne_CSV1");
    QMap<QString, int> columns;
    columns.insert("C1", 1);
    columns.insert("C2", 2);
    columns.insert("C3", 3);
    QMapIterator<QString, int> mapIt(columns);
    while (mapIt.hasNext()) {
        mapIt.next();
        config.addColumn(mapIt.key(), mapIt.value());
    }
    QJsonObject jsonData;
    config.write(jsonData);

    // read data from Json to a new object
    ImportConfig config2;
    config2.read(jsonData);

    // check that values are correctly read
    CHECK(config2.getUid().toString().toUpper() == "{67C8770B-44F1-410A-AB9A-F8B5446F13FF}");
    CHECK(config2.hasTime());
    CHECK(config2.getSeparatorChar() == '#');
    CHECK(config2.getDecimalChar() == '.');
    CHECK(config2.getNbLinesToSkipStart() == 5);
    CHECK(config2.getNbLinesToSkipEnd() == 1);
    CHECK(config2.getDateFormat() == "dd-MM-yyyy");
    CHECK(config2.getTimeFormat() == "HH:mm:ss");
    CHECK(config2.getName() == "CaisseEpargne_CSV1");
    CHECK(config2.getColumnPosition("C1") == 1);
    CHECK(config2.getColumnPosition("C2") == 2);
    CHECK(config2.getColumnPosition("C3") == 3);
}