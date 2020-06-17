#include "model.h"

int main()
{
    std::string arch_file = "model.json";
    std::string weight_file = "model.h5";
    NCC::NCC_FrontEnd::Model model(arch_file, weight_file);
    model.connector();
    std::cout << "\n";
    model.printLayers();
    std::cout << "\n";
    //model.printConns();
}

