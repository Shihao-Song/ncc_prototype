#include "model.h"

int main(int argc, char *argv[])
{
    std::string arch_file(argv[1]);
    std::string weight_file(argv[2]);
    NCC::NCC_FrontEnd::Model model(arch_file, weight_file);
    model.connector();
    // std::cout << "\n";
    model.printLayers();
    // std::cout << "\n";
    //std::string out_root = arch_file.substr(0, arch_file.find("."));
    //model.printConns(out_root);
}

