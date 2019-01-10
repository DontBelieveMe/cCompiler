#pragma once

#include <cc/stdlib.h>
#include <cc/x86/gp_register.h>

#include <unordered_map>

// Instructions:
//
//   1-2		 0-1       0-1       0-4           0-4
// [OpCode] [Mod-Reg-R/M] [SIB] [Displacement] [Immediate]

namespace cc {
	namespace x86 {
		enum instruction_modifier {
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
		
		enum mnemonic {
			kMov_r32imm32,
			kAdd_rm32imm32,
			kMov_r32rm32,
			kRet,
			kAdd_r32rm32,
			kNop,
			kSub_rm32imm32,
			kSub_r32rm32
		};
		
		enum instruction_operands_def {
			kInsOps_rm32imm32 = 0,
			kInsOps_r32rm32 = 1,
			kInsOps_none = 2
		};

		class instruction_def {
		public:
			instruction_def(cc::u8 op, cc::size_t mod, const char* name, instruction_operands_def operands_def);
			instruction_def() {}
			
			cc::u8 get_opcode() const { return m_op; }
			cc::size_t get_modifiers() const { return m_mods; }
			
			bool uses_extension_modifier() const;
			bool encodes_reg_in_opcode() const;
			
			cc::u8 get_extension_digit() const;
			const char* get_name() const;
			instruction_operands_def get_operands_def() const { return m_operands_def; }
		private:
			cc::u8 m_op;
			cc::size_t m_mods;
			const char* m_name;
			instruction_operands_def m_operands_def;
		};

		extern std::unordered_map<mnemonic, instruction_def> instructions_map;

		class instruction {
		public:
			typedef cc::fixed_array<cc::u8, 15> instruction_buffer;

			static instruction make_reg_imm_op(mnemonic op, const cc::x86::gp_register& reg, cc::u32 data);
			static instruction make_2reg_op(mnemonic op, const cc::x86::gp_register& reg0, const cc::x86::gp_register& reg1);
			static instruction make_op(mnemonic op);

			cc::size_t size() const { return m_size; }
			instruction_buffer& data() { return m_data; }

		private:
			instruction_buffer m_data;
			cc::size_t m_size;
		};

		class instructions_collection {
		public:
			instructions_collection(const std::initializer_list<cc::x86::instruction>& instructions);
			instructions_collection();

			cc::array<cc::u8> combine();
			void add(const instruction& ins);
			void add(const cc::array<instruction>& instructions);	
			typedef cc::array<instruction> arr_type;
			arr_type& get_array() { return m_instructions; }
			arr_type::iterator begin() { return m_instructions.begin(); }
			arr_type::const_iterator cbegin() const { return m_instructions.cbegin(); }

			arr_type::iterator end() { return m_instructions.end(); }
			arr_type::const_iterator cend() const { return m_instructions.cend(); }

		private:
			cc::array<instruction> m_instructions;
		};
	}
}
