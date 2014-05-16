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

/**
 * Creates an interval from a read line.
 *
 * @param line Line from the interval text file.
 *
 * @return Created interval.
 */
static AbstractInterval::SP intervalFromLine(const std::string& line)
{
    std::vector<std::string> fields;
    boost::split(fields, line, boost::is_any_of(" "));
    timestamp_t begin, end;
    std::string val;
    interval_key_t key;

    begin = std::stoll(fields[0]);
    end = std::stoll(fields[1]);
    key = std::stoi(fields[2]);
    val = fields[3];

    StringInterval::SP interval {new StringInterval(begin, end, key)};
    interval->setValue(val);

    std::cout << "Creating interval with value " << val << std::endl;

    return interval;
}

/**
 * Gets the next line of the file that is not empty or that is not a
 * comment (begins with '#').
 *
 * @param inputFile Input file stream
 *
 * @return The next line, or an empty string is there is nothing left
 *         to read.
 */
std::string getNextLine(std::ifstream& inputFile)
{
    std::string str;
    bool done = false;

    while (!done) {
        getline(inputFile, str);

        if (inputFile.eof()) {
            str = std::string();
            done = true;
        }

        if (str.length() > 0 && str[0] != '#') {
            done = true;
        }
    }

    return str;
}

/**
 * Reads the next interval from the input file.
 *
 * @param inputFile Input file stream.
 *
 * @return Built interval, or NULL if there is no more intervals to read.
 */
AbstractInterval::SP getNextInterval(std::ifstream& inputFile)
{
    std::string line = getNextLine(inputFile);
    if (line.length() == 0) {
        return AbstractInterval::SP();
    }

    return intervalFromLine(line);
}

int main(int argc, char* argv[])
{
    struct context ctx;
    int ret;
    HistoryFileSink hfSink;
    std::string line;

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

    ctx.inputFile.open(ctx.cfg.inputFilename);
    if (!ctx.inputFile.good())
    {
        std::cerr << "Error: could not open input file." << std::endl;
        return 1;
    }

    try {
        hfSink.open(ctx.cfg.outputFilename);

        while (true) {
            AbstractInterval::SP interval = getNextInterval(ctx.inputFile);
            if (interval == NULL) {
                break;
            }

            hfSink.addInterval(interval);

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
