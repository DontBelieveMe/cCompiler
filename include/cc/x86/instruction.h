#pragma once

#include <cc/stdlib.h>
#include <cc/x86/gp_register.h>
#include <cc/cdef_api.h>

#include <unordered_map>

// Instructions:
//
//   1-2		 0-1       0-1       0-4           0-4
// [OpCode] [Mod-Reg-R/M] [SIB] [Displacement] [Immediate]

namespace cc {
	namespace x86 {
		enum EInstructionModifier {
			kInsMod_None = 0,

			// ModR/M byte uses only the R/M portion
			// and the reg portion contains an "extension number"
			kInsMod_Ext0 = 1,
			kInsMod_Ext1 = 1 << 1,
			kInsMod_Ext2 = 1 << 2,
			kInsMod_Ext3 = 1 << 3,
			kInsMod_Ext4 = 1 << 4,
			kInsMod_Ext5 = 1 << 5,
			kInsMod_Ext6 = 1 << 6,
			kInsMod_Ext7 = 1 << 7,
			
			// #todo (bwilks) fill in a description here
			// when you understand it
			kInsMod_RexW = 1 << 8,

			// ModR/M byte contains a register operand and a r/m operand
			kInsMod_r    = 1 << 9,
			
			// A value follows the opcode that may specify a code offset and
			// maybe a new value for the code segment register
			kInsMod_cb   = 1 << 10, 
			kInsMod_cw   = 1 << 11, 
			kInsMod_cd   = 1 << 12,
			kInsMod_cp   = 1 << 13, 
			kInsMod_co   = 1 << 14, 
			kInsMod_ct   = 1 << 15,
			
			// Has immediate operand that follows the opcode, ModR/M or
			// SIB bytes
			kInsMod_ib   = 1 << 16, 
			kInsMod_iw   = 1 << 17, 
			kInsMod_id   = 1 << 18,
			kInsMod_io   = 1 << 19,
			
			// Lower 3 bits of opcode byte is used to encode the register
			// operand without  a ModR/M byte 
			kInsMod_pRb  = 1 << 20, 
			kInsMod_pRw  = 1 << 21, 
			kInsMod_pRd  = 1 << 22,
			kInsMod_pRo  = 1 << 23,
			kInsMod_pI   = 1 << 24
		};
		
		enum EMnemonic {
			kMov_r32imm32,
			kAdd_rm32imm32,
			kMov_r32rm32,
			kRet,
			kAdd_r32rm32,
			kNop,
			kSub_rm32imm32,
			kSub_r32rm32,
			kCall
		};
		
		enum EInstructionOperandsDef {
			kInsOps_rm32imm32 = 0,
			kInsOps_r32rm32 = 1,
			kInsOps_none = 2,
			kInsOps_imm32 = 3
		};

		class InstructionDef {
		public:
			InstructionDef(cc::u8 op, cc::size_t mod, const char* name, EInstructionOperandsDef operands_def);
			InstructionDef() {}
			
			cc::u8 get_opcode() const { return m_op; }
			cc::size_t get_modifiers() const { return m_mods; }
			
			bool uses_extension_modifier() const;
			bool encodes_reg_in_opcode() const;
			
			bool has_immediate_code_offset() const;

			cc::u8 get_extension_digit() const;
			const char* get_name() const;
			EInstructionOperandsDef get_operands_def() const { return m_operands_def; }
		private:
			cc::u8 m_op;
			cc::size_t m_mods;
			const char* m_name;
			EInstructionOperandsDef m_operands_def;
		};

		extern std::unordered_map<EMnemonic, InstructionDef> instructions_map;

		class Instruction {
		public:
			typedef cc::FixedArray<cc::u8, 15> instruction_buffer;

			static Instruction make_reg_imm_op(EMnemonic op, const cc::x86::GeneralPurposeRegister& reg, cc::u32 data);
			static Instruction make_2reg_op(EMnemonic op, const cc::x86::GeneralPurposeRegister& reg0, const cc::x86::GeneralPurposeRegister& reg1);
			static Instruction make_op(EMnemonic op);
			static Instruction make_imm32_op(EMnemonic op, cc::u32 value);

			cc::size_t size() const { return m_size; }
			instruction_buffer& data() { return m_data; }

		private:
			instruction_buffer m_data;
			cc::size_t m_size;
		};

		class InstructionsCollection {
		public:
			typedef cc::Array<Instruction> arr_type;
			
			InstructionsCollection(const std::initializer_list<cc::x86::Instruction>& instructions);
			InstructionsCollection();

			cc::Array<cc::u8> combine();
			cc::size_t get_count() const { return m_instructions.size(); }	
			void add(const Instruction& ins);
			void add(const cc::Array<Instruction>& instructions);	
			
			cc::size_t offset_of_instruction(cc::size_t index);

			arr_type& get_array() { return m_instructions; }
			
			arr_type::iterator begin() { return m_instructions.begin(); }
			arr_type::iterator end() { return m_instructions.end(); }

			arr_type::const_iterator cbegin() const { return m_instructions.cbegin(); }
			arr_type::const_iterator cend() const { return m_instructions.cend(); }

		private:
			cc::Array<Instruction> m_instructions;
		};
	}
}
