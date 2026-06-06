#include "application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Application app;

    if (!app.init(argc, argv)) {
        std::cerr << "[App] Initialisation failed.\n";
        return 1;
    }

    app.run();
    app.shutdown();
    return 0;
}
