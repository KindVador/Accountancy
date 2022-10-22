// Demonstrate how to create a Catch main() for testing Qt GUI code

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);                 // We can pass as argument "-platform offscreen" if needed
    int result = Catch::Session().run(argc, argv);// We can pass as argument "--break" if needed
    return result;
}