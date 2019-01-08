#include <iostream>

#include <cc/file_io.h>
#include <cc/coff/object_file.h>
#include <cc/mem_utils.h>
#include <cc/x86/instruction.h>

#include <time.h>

#include <cc/coff/symbols.h>
#include <cc/logging.h>
#include <cc/assert.h>

#include <cc/parsing/asm_parser.h>

cc::x86::instruction make_instruction(cc::x86::mnemonic op, cc::x86::gp_register reg, cc::u32 imm) {
	return cc::x86::instruction::make_reg_imm_op(op, reg, imm);
}

cc::x86::instruction make_instruction(cc::x86::mnemonic op, cc::x86::gp_register reg1, cc::x86::gp_register reg2) {
	return cc::x86::instruction::make_2reg_op(op, reg1, reg2);
}

cc::x86::instruction make_instruction(cc::x86::mnemonic op) {
	return cc::x86::instruction::make_op(op);
}

int main() {
	using namespace cc::x86;
	using namespace cc;
	using namespace coff;
	
	cc::logger::startup();

	x86::instructions_collection instructions = {
		make_instruction(kMov_r32imm32, gp_register::ecx, 5),
		make_instruction(kAdd_rm32imm32, gp_register::ecx, 15),
		make_instruction(kMov_r32imm32, gp_register::ebx, 3),
		make_instruction(kAdd_r32rm32, gp_register::ebx, gp_register::ecx),
		make_instruction(kMov_r32rm32, gp_register::eax, gp_register::ebx),
		make_instruction(kSub_rm32imm32, gp_register::eax, 10),
		make_instruction(kMov_r32imm32, gp_register::ebx, 4),
		make_instruction(kSub_r32rm32, gp_register::eax, gp_register::ebx),
		make_instruction(kRet)
	};

	u32 characteristics = coff::kImageScnAlign16Bytes | 
		coff::kImageScnMemExecute | 
		coff::kImageScnCntCode | 
		coff::kImageScnMemRead;

	shared_ptr<coff::section> section = make_shared<coff::section>();
	section->set_name(".text$mn")
		.set_characteristics(characteristics)
		.set_raw_data(instructions.combine());
	
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
	
	using namespace cc::parsing;

	asm_file assembly_file = asm_file::from_file("data/test.asm");

	return 0;
}
