#include <cc/x86/gp_register.h>

using namespace cc::x86;

const gp_register gp_register::eax(0x0);
const gp_register gp_register::ecx(0x1);
const gp_register gp_register::edx(0x2);
const gp_register gp_register::ebx(0x3);
const gp_register gp_register::esp(0x4);
const gp_register gp_register::ebp(0x5);
const gp_register gp_register::esi(0x6);
const gp_register gp_register::edi(0x7);

gp_register::gp_register(cc::u8 encoding)
	: m_encoding(encoding) {
}

bool gp_register::operator==(const gp_register& right) const {
	return m_encoding == right.m_encoding;
}

bool gp_register::operator!=(const gp_register& right) const {
	return m_encoding != right.m_encoding;
}