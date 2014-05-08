#include <iostream>
#include <string>

#include <boost/program_options.hpp>

struct config {
    std::string input_filename;
    std::string output_filename;
};

/**
 * Parses the command line options passed to the program.
 *
 * @return 0 to continue, 1 to exit with success, 2 to exit with failure.
 */
int parse_options(int argc, char *argv[], struct config &cfg)
{
    namespace po = boost::program_options;

    po::options_description desc("Available options");
    desc.add_options()
        ("help,h", "print this help message")
        ("input-file,i", po::value<std::string>(&cfg.input_filename), "specify the input file")
        ("output-file,o", po::value<std::string>(&cfg.output_filename), "specify the output file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct config cfg;
    int ret;

    ret = parse_options(argc, argv, cfg);

    if (ret != 0) {
        return ret - 1;
    }

    std::cout << cfg.input_filename << std::endl;
    std::cout << cfg.output_filename << std::endl;

    return 0;
}
