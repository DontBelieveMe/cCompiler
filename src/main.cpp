#include <cc/coff/object_file.h>
#include <cc/coff/symbols.h>

#include <cc/x86/function.h>

#include <cc/asm/asm_file.h>

#include <cc/logging.h>
#include <cc/timing.h>

#include <cxxopts/cxxopts.hpp>

int main(int argc, char** argv) {
	using namespace cc::x86;
	using namespace cc;
	using namespace coff;
	using namespace cc::assembly;

	cc::Logger::startup();
	
	cxxopts::Options options("cCompiler Assembler", "Retargetable assembler (currently supporting x86)");
	options.add_options()
		("i,input", "The input assembly (.asm) file", cxxopts::value<std::string>())
		("o,output", "The output object file (.obj)", cxxopts::value<std::string>()->default_value("a.obj"));

	auto result = options.parse(argc, argv);

	cc::TimeBlock program_perf([](float seconds){
		CINFO("Program completed in {0}s", seconds);
	});
	
	AsmFile source_file(result["input"].as<std::string>());

	if (source_file.compiled_successfully) {
		source_file.get_obj_file()->write_to_file(result["output"].as<std::string>());
	}
	else {
		CFATAL("Found errors - not writing object file");
	}
	return 0;
}
