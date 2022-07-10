#include <catch2/catch.hpp>

#include "importconfig.hpp"


TEST_CASE( "ImportConfig Default Constructor", "[core]" )
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

TEST_CASE( "ImportConfig Setter and Getter", "[core]" )
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

TEST_CASE( "ImportConfig READ from JSON", "[core]" )
{

}

TEST_CASE( "ImportConfig WRITE to JSON", "[core]" )
{

}