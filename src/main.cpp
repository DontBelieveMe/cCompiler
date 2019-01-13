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

	/*func.add_instructions(assembly_file.get_instructions().get_array());	
	
	u32 characteristics = coff::kImageScnAlign16Bytes | 
		coff::kImageScnMemExecute | 
		coff::kImageScnCntCode | 
		coff::kImageScnMemRead;

	shared_ptr<coff::section> section = make_shared<coff::section>();
	section->set_name(".text$mn")
		.set_characteristics(characteristics)
		.set_raw_data(assembly_file.get_instructions().combine());
	
	coff::object_file obj;
	obj.set_machine(coff::kMachinei386)
		.set_datetime((u32) time(0));
	
	obj.add_section(section);
	
	std::string drectve_str = "/ENTRY:main ";

	shared_ptr<coff::section> drectve = make_shared<coff::section>();
	drectve->set_name(".drectve")
		.set_characteristics(kImageScnLnkRemove | kImageScnLnkInfo)
		.set_raw_data(cc::array<cc::u8>(drectve_str.begin(), drectve_str.end()));

	obj.add_section(drectve);
	
	cc::shared_ptr<symbol_table> sym_table = cc::make_shared<symbol_table>();

	symbol sym;
	sym.is_aux = false;
	sym.number_of_aux_symbols = 0;
	sym.name = symbol_name("_main");
	sym.section_number = 1;
	sym.storage_clss = kImageSymClassExternal;
	sym.value = 0;

	sym_table->add_symbol(sym);

	obj.set_symbol_table(sym_table);
	obj.write_to_file("data/obj2.obj");
	*/	
	return 0;
}
