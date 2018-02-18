#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <mmd/parser.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

void testParser(std::string modelPath) {
    PMXModel testModel(modelPath);
}

int main(int argc, char **argv) {
    po::options_description desc("MMD Parser Testing Program");
    desc.add_options()
        ("input-model,i", po::value<std::string>(), "input mmd model")
        ("help", "show help")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
    } else if (vm.count("input-model")) {
        testParser(vm["input-model"].as<std::string>());
    } else {
        std::cout << "model path was not set." << std::endl;
    }
    return 0;
}