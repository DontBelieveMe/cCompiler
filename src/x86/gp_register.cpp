#include <cc/x86/gp_register.h>

using namespace cc::x86;

const GeneralPurposeRegister GeneralPurposeRegister::eax(0x0);
const GeneralPurposeRegister GeneralPurposeRegister::ecx(0x1);
const GeneralPurposeRegister GeneralPurposeRegister::edx(0x2);
const GeneralPurposeRegister GeneralPurposeRegister::ebx(0x3);
const GeneralPurposeRegister GeneralPurposeRegister::esp(0x4);
const GeneralPurposeRegister GeneralPurposeRegister::ebp(0x5);
const GeneralPurposeRegister GeneralPurposeRegister::esi(0x6);
const GeneralPurposeRegister GeneralPurposeRegister::edi(0x7);

GeneralPurposeRegister::GeneralPurposeRegister(cc::u8 encoding)
	: m_encoding(encoding) {
}

bool GeneralPurposeRegister::operator==(const GeneralPurposeRegister& right) const {
	return m_encoding == right.m_encoding;
}

bool GeneralPurposeRegister::operator!=(const GeneralPurposeRegister& right) const {
	return m_encoding != right.m_encoding;
}