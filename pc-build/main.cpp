#include "loop.h"
#include "gui/app.h"

#include <chrono>
#include <thread>

#include <QApplication>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

App *g_app = nullptr;

int main(int argc, char *argv[]) {
    using namespace std::this_thread;
    using namespace std::chrono;


    QApplication app(argc, argv);

    g_app = new App(argc, argv);
    g_app->show();

    return QApplication::exec();
}

#pragma clang diagnostic pop

/*
#include <iostream>

#define function int
#define for while(
#define do ) {
#define downto -->
#define end }
#define if (
#define then ) {
#define main main(
#define eq =
#define print(x) std::cout << x << std::endl;

#define integer int

function main do
    integer i eq 5;
    for i downto 0 do
        print(i)
    end
end
*/
