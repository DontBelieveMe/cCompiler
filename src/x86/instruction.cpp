#include <cc/x86/instruction.h>

#include <cstring>
#include <numeric>

using namespace cc::x86;

/*
	#todo (bwilks) - fix

	There should be a better way of storing opcode data.
	- Investigate how other compilers/assemblers do this.
*/
opcode instruction::m_opcodes[NUM_OPCODES] = {
	opcode(0xB8, true, false, false, -1), //kMov_r32imm32
	opcode(0x81, false, false, true, 0),  //kAdd_rm32imm32
	opcode(0x8B, false, true, false, -1), //kMov_r32rm32
	opcode(0xC3, false, false, false, -1), //kRet
	opcode(0x03, false, true, false, -1), //kAdd_r32rm32
	opcode(0x81, false, false, true, 5), //kSub_rm32imm32
	opcode(0x2B, false, true, false, -1) //kSub_r32rm32
};

opcode::opcode(cc::u8 op, bool encodes_reg_in_opcode, bool uses_rm_and_reg, bool reg_is_extension, cc::u8 extension)
	: m_opcode(op), 
	m_encodes_reg_in_opcode(encodes_reg_in_opcode),
	m_uses_rm_and_reg(uses_rm_and_reg), 
	m_reg_is_extension(reg_is_extension), 
	m_extension(extension) {}

instruction instruction::make_reg_imm_op(mnemonic op, const cc::x86::gp_register& reg, cc::u32 data) {
	using namespace cc;

	const opcode& kOpcodeData = m_opcodes[static_cast<size_t>(op)];
	const size_t kSizeofImmediateData = sizeof(u32);

	bool has_mod_reg_rm_byte = false;
	u8 mrrm = 0;
	u8 opcode_byte_data = 0;

	if (kOpcodeData.encodes_reg_in_opcode()) {
		opcode_byte_data = kOpcodeData.primary_opcode() + reg.encoding();
	}

	if (kOpcodeData.reg_is_extension()) {
		opcode_byte_data = kOpcodeData.primary_opcode();

		// #todo(bwilks). This shouldn't be hardcoded - as this only allows for direct addressing
		const u8 mrrm_mod = 0xC0;
		const u8 mrrm_reg = kOpcodeData.extension();
		const u8 mrrm_rm = reg.encoding();

		mrrm |= mrrm_mod | (mrrm_reg << 3) | mrrm_rm;
		
		has_mod_reg_rm_byte = true;
	}

	// Only support 1 byte opcodes for now.
	// #todo (bwilks). fix this.
	const size_t kSizeofOpcode = 1;

	instruction final_instruction;
	final_instruction.m_size = kSizeofOpcode + kSizeofImmediateData;
	final_instruction.m_data[0] = opcode_byte_data;

	// Immediate data directly follows the the opcode.
	size_t immediate_offset = kSizeofOpcode;
	if (has_mod_reg_rm_byte) {
		final_instruction.m_data[1] = mrrm;

		immediate_offset++;
		final_instruction.m_size++;
	}

	std::memcpy(&final_instruction.m_data[immediate_offset], (u8*)&data, kSizeofImmediateData);

	return final_instruction;
}

instruction instruction::make_2reg_op(mnemonic op, const cc::x86::gp_register& reg0, const cc::x86::gp_register& reg1) {
	const opcode& kOpcodeData = m_opcodes[static_cast<cc::size_t>(op)];
	
	cc::u8 mod_reg_rm_byte = 0;
	{
		// #todo (bwilks) fix.
		// Only supports direct addressing mode currently. this needs to put into an option.
		const cc::u8 kMod = 0xC0;
		const cc::u8 kReg = reg0.encoding();
		const cc::u8 kRm = reg1.encoding();

		mod_reg_rm_byte |= kMod | (kReg << 3) | kRm;
	}

	instruction final_ins;
	final_ins.m_data[0] = kOpcodeData.primary_opcode();
	final_ins.m_data[1] = mod_reg_rm_byte;

	// #todo(bwilks) - again fix hardcoded 1-byte opcode size.
	const cc::size_t kOpcodeSize = 1;
	const cc::size_t kSizeofModRegRm = 1;
	const cc::size_t kFinalInstructionSize = kOpcodeSize + kSizeofModRegRm;

	final_ins.m_size = kFinalInstructionSize;

	return final_ins;
}

instruction instruction::make_op(mnemonic op) {
	const opcode& kOpcodeData = m_opcodes[static_cast<cc::size_t>(op)];
	
	// #todo (bwilks) - opcode size is fixed to one byte again.
	const cc::size_t kSizeofOpcode = 1;

	instruction final_ins;

	// Instructions with no operands only have the opcode and nothing else
	// hence the sizeof the instruction is the same as the size of the instruction
	final_ins.m_size = kSizeofOpcode;
	
	final_ins.m_data[0] = kOpcodeData.primary_opcode();
	
	return final_ins;
}

instructions_collection::instructions_collection(const std::initializer_list<cc::x86::instruction>& instructions) {
	m_instructions = std::vector<instruction>(instructions.begin(), instructions.end());
}

instructions_collection::instructions_collection() {}

cc::array<cc::u8> instructions_collection::combine() {
	const cc::size_t kTotalInstructionsSize = std::accumulate(m_instructions.begin(), m_instructions.end(), 0,
		[](cc::size_t acu, const instruction& ins) {
			return acu + ins.size();
		}
	);

	cc::array<u8> instructions_buffer;
	instructions_buffer.reserve(kTotalInstructionsSize);

	for (instruction& ins : m_instructions) {
		const instruction::instruction_buffer& dat = ins.data();

		// Only iterate till ins.size() and not over the whole buffer
		// this is because the instruction may not (and probably won't) use the whole 15 bytes
		// available to it.
		// TL;DR: ins.size() may not equal the size of the the instruction buffer (15 bytes)
		for (cc::size_t i = 0; i < ins.size(); ++i) {
			instructions_buffer.push_back(dat[i]);
		}
 	}

	return instructions_buffer;
}

void instructions_collection::add(const instruction& ins) {
	m_instructions.push_back(ins);
}
