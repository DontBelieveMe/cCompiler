#include <cc/x86/instruction.h>

#include <cstring>
#include <numeric>

#include <cc/assert.h>

using namespace cc::x86;

instruction_def::instruction_def(cc::u8 op, cc::size_t mod) 
	: m_op(op), m_mods(mod) {
}

std::unordered_map<cc::x86::mnemonic, cc::x86::instruction_def> cc::x86::instructions_map = {
	{ kMov_r32imm32, instruction_def(0xB8, kInsMod_pRd | kInsMod_id) },
	{ kMov_r32rm32,  instruction_def(0x8B, kInsMod_r) },

	{ kAdd_rm32imm32, instruction_def(0x81, kInsMod_Ext0 | kInsMod_id) },
	{ kAdd_r32rm32,   instruction_def(0x03, kInsMod_r) },

	{ kRet, instruction_def(0xC3, kInsMod_None) },

	{ kSub_rm32imm32, instruction_def(0x81, kInsMod_Ext5 | kInsMod_id) },
	{ kSub_r32rm32,   instruction_def(0x2B, kInsMod_r) }
};

bool instruction_def::uses_extension_modifier() const {
	return (m_mods & kInsMod_Ext0) || (m_mods & kInsMod_Ext1) ||
		(m_mods & kInsMod_Ext2) || (m_mods & kInsMod_Ext3) ||
		(m_mods & kInsMod_Ext4) || (m_mods & kInsMod_Ext5) ||
		(m_mods & kInsMod_Ext6) || (m_mods & kInsMod_Ext7);
}

bool instruction_def::encodes_reg_in_opcode() const {
	return (m_mods & kInsMod_pRb) || (m_mods & kInsMod_pRw) ||
		(m_mods & kInsMod_pRd) || (m_mods & kInsMod_pRo);
}

cc::u8 instruction_def::get_extension_digit() const {
	ASSERT(uses_extension_modifier(), "Cannot get extension code for a instruction that does not use an extension");

	if(m_mods & kInsMod_Ext0) return 0;
	if(m_mods & kInsMod_Ext1) return 1;
	if(m_mods & kInsMod_Ext2) return 2;
	if(m_mods & kInsMod_Ext3) return 3;
	if(m_mods & kInsMod_Ext4) return 4;
	if(m_mods & kInsMod_Ext5) return 5;
	if(m_mods & kInsMod_Ext6) return 6;
	if(m_mods & kInsMod_Ext7) return 7;

	// SHOULD NOT HAPPEN. Should be caught by the assertions above.
	ASSERT(false, "Invalid instruction modifier state, cannot get extension digit that should exist");
	return 0xff;
}

instruction instruction::make_reg_imm_op(mnemonic op, const cc::x86::gp_register& reg, cc::u32 data) {
	using namespace cc;

	const instruction_def& kInstructionDefData = instructions_map[op];
	const size_t kSizeofImmediateData = sizeof(u32);

	bool has_mod_reg_rm_byte = false;
	u8 mrrm = 0;
	u8 opcode_byte_data = 0;

	if (kInstructionDefData.encodes_reg_in_opcode()) {
		opcode_byte_data = kInstructionDefData.get_opcode() + reg.encoding();
	}

	if (kInstructionDefData.uses_extension_modifier()) {
		opcode_byte_data = kInstructionDefData.get_opcode();

		// #todo(bwilks). This shouldn't be hardcoded - as this only allows for direct addressing
		const u8 mrrm_mod = 0xC0;
		const u8 mrrm_reg = kInstructionDefData.get_extension_digit();
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
	const instruction_def& kOpcodeData = instructions_map[op];
	
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
	final_ins.m_data[0] = kOpcodeData.get_opcode();
	final_ins.m_data[1] = mod_reg_rm_byte;

	// #todo(bwilks) - again fix hardcoded 1-byte opcode size.
	const cc::size_t kOpcodeSize = 1;
	const cc::size_t kSizeofModRegRm = 1;
	const cc::size_t kFinalInstructionSize = kOpcodeSize + kSizeofModRegRm;

	final_ins.m_size = kFinalInstructionSize;

	return final_ins;
}

instruction instruction::make_op(mnemonic op) {
	const instruction_def& kOpcodeData = instructions_map[op];
	
	// #todo (bwilks) - opcode size is fixed to one byte again.
	const cc::size_t kSizeofOpcode = 1;

	instruction final_ins;

	// Instructions with no operands only have the opcode and nothing else
	// hence the sizeof the instruction is the same as the size of the instruction
	final_ins.m_size = kSizeofOpcode;
	
	final_ins.m_data[0] = kOpcodeData.get_opcode();
	
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
