#include <iostream>
#include <fstream>
#include "Controller.h"
#include "Cache.h"
#include <time.h>
#include <format>
#include <stdlib.h>
#include "cxxopts.hpp"

/* Console Interface
* Cache:
* cellCount: uint (how many cells the cache has in total)
* blockSize: uint (how many bytes a cache cell contains)
* associativity: uint (how many cells are in a set)
* associativity: uint (how many cells are in a set)
* evictionPolicy: random|fifo|lru
* writeHitPolicy: writeThrough|writeBack
* writeMissPolicy allocate|noAllocate 
*/

int main(int argc, char *argv[]) {
    cxxopts::Options options("Cache Sim", "Simulates a cache with options to configure it");

    options
        .set_width(100)
        .add_options("cache")
        ("c,cellCount",     "Number of cache cells in cache     [uint]  ", cxxopts::value<unsigned int>()->default_value("1024"))
        ("b,blockSize",     "Number of bytes a cache cell holds [uint]  ", cxxopts::value<unsigned int>()->default_value("16"))
        ("a,associativity", "Cache associativity                [uint]  ", cxxopts::value<unsigned int>()->default_value("1"))
        ("e,evict",  "Evicton Policy    [LRU|fifo|random]        ",        cxxopts::value<std::string>()->default_value("LRU"))
        ("w,hit",    "Write hit Policy  [writeBack|writeThrough] ",        cxxopts::value<std::string>()->default_value("writeBack"))
        ("m,miss",   "Write miss Policy [allocate|noAllocate]    ",        cxxopts::value<std::string>()->default_value("allocate"));
    options.add_options("simulator")
		("h,help",  "Print help screen")
		("o,output","Path to output file [string]", cxxopts::value<std::string>()->default_value(""))
        ("t,trace", "Path to trace file  [string]", cxxopts::value<std::string>());

    // parse arguments
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch (const cxxopts::exceptions::exception &e) {
        std::cerr << "error: " << e.what() << '\n';
        std::cerr << "usage: see help -h/--help\n";
        return EXIT_FAILURE;
    }

    // help screen
    if (result.count("help")) {
		std::cout << options.help() << std::endl;
        return 0;
    }

    // fill variables
    unsigned int cellCount = 4;
    unsigned int blockSize = 2;
    unsigned int associativity = 4;
    EvictionPolicy evictionPolicy = LRU;
    WriteHitPolicy writeHitPolicy = writeBack;
    WriteMissPolicy writeMissPolicy = allocate;
    std::string trace = "";
    std::string output = "";

    std::string evict = "";
	std::string hit = "";
	std::string miss = "";
    try
    {
		cellCount = result["cellCount"].as<std::uint32_t>();
        blockSize = result["blockSize"].as<std::uint32_t>();
        associativity = result["associativity"].as<std::uint32_t>();
        evict = result["evict"].as<std::string>();
        hit = result["hit"].as<std::string>();
        miss = result["miss"].as<std::string>();
        trace = result["trace"].as<std::string>();
        output = result["output"].as<std::string>();

        if (evict == "LRU") {
            evictionPolicy = LRU;
        }
        else if (evict == "fifo") {
            evictionPolicy = fifo;
        }
        else if (evict == "random") {
            evictionPolicy = random;
        }
        else {
            std::string error = std::format("Argument 'evict' must be [LRU|fifo|random] and not '{}'", evict);
            cxxopts::throw_or_mimic<cxxopts::exceptions::exception>(error);
        }

        if (hit == "writeBack") {
            writeHitPolicy = writeBack;
        }
        else if (hit == "writeThrough") {
            writeHitPolicy = writeThrough;
        }
        else {
            std::string error = std::format("Argument 'hit' must be [writeBack|writeThrough] and not '{}'", hit);
            cxxopts::throw_or_mimic<cxxopts::exceptions::exception>(error);
        }

        if (miss == "allocate") {
            writeMissPolicy = allocate;
        }
        else if (miss == "noAllocate") {
            writeMissPolicy = noAllocate;
        }
        else {
            std::string error = std::format("Argument 'miss' must be [allocate|noAllocate] and not '{}'", miss);
            cxxopts::throw_or_mimic<cxxopts::exceptions::exception>(error);
        }
    }
    catch (const cxxopts::exceptions::exception &e)
    {
        std::cerr << "error: " << e.what() << '\n';
        std::cerr << "usage: see help -h/--help\n";
        return EXIT_FAILURE;
    }



    // main functionality
    try
    {
		Controller controller = Controller(cellCount, blockSize, associativity, evictionPolicy, writeHitPolicy, writeMissPolicy);
        std::cout << "Cache Sim started:\n";
        std::cout << std::format("  cellCount: {}\n  blockSize: {}\n  associativity: {}\n  evictionPolicy: {}\n  writeHitPolicy: {}\n  writeMissPolicy: {}\n\n", cellCount, blockSize, associativity, evict, hit, miss) << "\n";
        

		std::ifstream traceFile;
		traceFile.open(trace);
        

        if (!traceFile.is_open()) {
            std::string error = std::format("Couldn't open file '{}'. Check if it exists and the path to the file is correct.", trace);
            throw std::runtime_error(error);
        }

		while (traceFile) {
		std::string line;
			std::getline(traceFile, line);
			if (line.length() < 12) continue;
			std::string operation = line.substr(2, 1);
			unsigned int address = std::stoul(line.substr(4, 8), 0, 16);

			if (operation == "0") {
				controller.read(address);
			}
			if (operation == "1") {
				controller.write(address);
			}
		}
        traceFile.close();

		// output
		std::cout << controller.printResults();
        if (output == "") {
            return 0;
        }

		std::ofstream outputFile;
		outputFile.open(output);

		if (!outputFile.is_open()) {
			std::string error = std::format("Couldn't open file '{}'. Check if it exists and the path to the file is correct.", output);
			throw std::runtime_error(error);
		}
        outputFile << controller.printResults();
        outputFile.close();


    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
        std::cerr << "usage: see help -h/--help\n";
        return EXIT_FAILURE;
    }


    return 0;

}
