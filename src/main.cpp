#include <cc/coff/object_file.h>
#include <cc/coff/symbols.h>

#include <cc/x86/function.h>

#include <cc/asm/asm_file.h>

#include <cc/logging.h>
#include <cc/timing.h>

int main(int argc, char** argv) {
	using namespace cc::x86;
	using namespace cc;
	using namespace coff;
	using namespace cc::assembly;

	cc::logger::startup();
	
	if(argc != 2) {
		CFATAL("Invalid numer of arguments!");
		return 1;
	}
	
	cc::time_block program_pref([](float seconds){
		CINFO("Program completed in {0}s", seconds);
	});
	
	asm_file source_file(argv[1]);

	if (source_file.compiled_successfully) {
		source_file.get_obj_file()->write_to_file("data/output.obj");
	}
	else {
		CFATAL("Found errors - not writing object file");
	}
	return 0;
}
