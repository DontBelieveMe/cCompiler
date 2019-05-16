#include <stdio.h>

#include <cc/file_io.h>
#include <cc/logging.h>
#include <cc/os.h>
#include <cc/coff/coff.h>

#include <cxxopts/cxxopts.hpp>

#include <cc/x86_data.h>
#include <cc/x86_encoder.h>

int main(int argc, char* argv[])
{
	cc::Logger::Startup();

	cxxopts::Options options("cCompiler", "Assembler for x86");
	options.add_options()
		("s,source", "Input Source File (.asm)", cxxopts::value<std::string>())
		("o,output", "Output Object File", cxxopts::value<std::string>());

	const auto result = options.parse(argc, argv);
	
	if(result.count("source") != 1) {
		CFATAL("--source (-s) argument either not provided or specified multiple times. aborting.");
		return 1;
	}

	const std::string input_file = result["source"].as<std::string>();

	const bool has_specified_output = result.count("output") == 1;
	const std::string output_file = has_specified_output ? result["output"].as<std::string>() : input_file + ".obj";
	
	cc::CoffObjectFile object_file;
	object_file.ReadFromFile(input_file.c_str());

	auto sections = object_file.Sections();

	for(auto section : sections)
	{
		auto name = section.Name();
		CINFO("Read Sectio Name: {0}", section.Name().ResolveString());
	}

	cc::TryKeepConsoleOpen();
	return 0;
}
