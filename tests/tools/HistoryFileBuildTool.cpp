#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <delorean/BasicTypes.hpp>
#include <delorean/interval/Int32Interval.hpp>
#include <delorean/interval/StringInterval.hpp>
#include <delorean/interval/StandardIntervalType.hpp>
#include <delorean/HistoryFileSink.hpp>
#include <delorean/ex/InvalidIntervalArguments.hpp>
#include <delorean/ex/IO.hpp>
#include <utils.hpp>

using namespace delo;

struct config
{
    std::string inputFilename;
    std::string outputFilename;
    bool verbose;
};

struct context
{
    struct config cfg;
    std::ifstream inputFile;
};

/**
 * Parses the command line options passed to the program.
 *
 * @return 0 to continue, 1 to exit with success, 2 to exit with failure.
 */
static int parse_options(int argc, char* argv[], struct config& cfg)
{
    namespace bpo = boost::program_options;

    bpo::options_description desc("Available options");
    desc.add_options()
        ("help,h", "print this help message")
        ("input-file,i", bpo::value<std::string>(&cfg.inputFilename), "specify the input file")
        ("output-file,o", bpo::value<std::string>(&cfg.outputFilename), "specify the output file")
        ("verbose,v", bpo::value<bool>(&cfg.verbose), "verbose mode")
    ;

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    return 0;
}



int main(int argc, char* argv[])
{
    struct context ctx;
    int ret;
    HistoryFileSink hfSink;
    std::string line;
    std::vector<AbstractInterval::UP> intervals;

    ret = parse_options(argc, argv, ctx.cfg);

    if (ret != 0) {
        return ret - 1;
    }

    if (ctx.cfg.inputFilename.length() == 0) {
        std::cerr << "Error: input file not specified." << std::endl;
        return 1;
    }

    if (ctx.cfg.outputFilename.length() == 0) {
        std::cerr << "Error: output file not specified." << std::endl;
        return 1;
    }



    try {
        getIntervalsFromTextFile(ctx.cfg.inputFilename, intervals);

        hfSink.open(ctx.cfg.outputFilename);

        for (auto& interval : intervals) {
            hfSink.addInterval(std::move(interval));

            if (ctx.cfg.verbose) {
                std::cout << *interval << std::endl;
            }
        }

        if (ctx.cfg.verbose) {
            std::cout << "History file created successfully." << std::endl;
        }
    } catch (ex::InvalidIntervalArguments e) {
        std::cerr << e.what() << ": " << e.getBegin() << " " << e.getEnd() << std::endl;
    } catch (ex::IO e) {
        std::cerr << "IO error: " << e.what() << std::endl;
    }

    return 0;
}
