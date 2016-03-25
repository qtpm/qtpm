#include <package2.h>
#include <iostream>

int main() {
    // Use package2 class
    Package2 package2;
    std::cout << package2.greeting().toStdString() << std::endl;
    // package2 uses package1
    std::cout << package2.relay().toStdString() << std::endl;
}
