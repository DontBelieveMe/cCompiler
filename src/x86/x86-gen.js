const fs = require("fs");
const process = require("process");

const asmdb = require("../../tools/asmdb");

class FileUtils {
	static writeFile(fileName, contents, callback) {
		let cwd = process.cwd();

		process.chdir(__dirname);
		fs.writeFile(fileName, contents, callback);
		process.chdir(cwd);
	}
}

class StringBuilder {
	constructor() {
		this._data = "";
	}

	appendLine(line) {
		this._data += line + "\n";
	}

	append(str) {
		this._data += str;
	}

	toString() {
		return this._data;
	}
}

class Convert {
	static toHex(x) {
		return x.toString(16);
	}
}

class Cpp {
	constructor() {
		this.tab = '  ';
	}

	buildCArray(arrayName, cpptype, items) {
		let sb = new StringBuilder();
		sb.appendLine(`${cpptype} ${arrayName}[${items.length}] = {`);

		items.forEach((item, index) => {
			sb.append(`${this.tab}${item}`);
			if (index < items.length - 1) {
				sb.append(',');
			}
			sb.append('\n');
		});

		sb.appendLine('};');

		return sb.toString();
	}

	buildInclude(includePath) {
		return `#include "${includePath}"`;
	}

	buildCtorCall(className, argsAsArray, semicolon) {
		if (semicolon === undefined) {
			semicolon = false;
		}

		let sb = new StringBuilder();
		sb.append(`${className}(`);
		argsAsArray.forEach((item, index) => {
			sb.append(item);
			if (index < argsAsArray.length - 1) {
				sb.append(', ');
			}
		});
		sb.append(')');

		if (semicolon) {
			sb.append(';');
		}

		return sb.toString();
	}
}

class x86CDefGenerator {
	constructor() {
		this.isa = new asmdb.x86.ISA();
	}

	build() {
		let cdef = new StringBuilder();
		let ins = [];

		this.isa.instructions.forEach((inst) => {
			if (inst.arch === "ANY" && inst.prefix === "") {
				ins.push(inst);
			}
		});

		cdef.appendLine("std::unordered_map<const char*, cc::Instruction> cc::InstructionSet::m_instructions = {");

		for (let i = 0; i < ins.length; i++) {
			let inst = ins[i];
			let pp = inst.pp !== '' ? inst.pp : "0";
			let mm = inst.mm !== '' ? inst.mm : "0";
			let dontparse = false;

			// Don't bother w/ more than 1 byte instructions.
			if (pp !== "0" || mm !== "0")
				dontparse = true;
			/*
			if (pp.length > 2 || mm.length > 2) {
				dontparse = true;
			}

			*/

			let mod = new StringBuilder();
			mod._data = "";
			let any = true;
			if (inst.rm === "r") {
				mod.append("kInsMod_r");
			} else if (/^[0-7]$/.test(inst.rm)) {
				mod.append("kInsMod_Ext" + inst.rm);
			}

			if (/i[bwdo]/.test(inst.opcodeString)) {
				if (mod._data.length > 0) {
					mod.append(" | ");
				}
				mod.append("kInsMod_" + inst.opcodeString.match(/i[bwdo]/)[0]);
			}

			if (/c[bwdpot]/.test(inst.opcodeString)) {
				if (mod._data.length > 0) {
					mod.append(" | ");
				}

				mod.append("kInsMod_" + inst.opcodeString.match(/c[bwdpot]/)[0]);
			}

			if (/\+r/.test(inst.opcodeString)) {
				if (mod._data.length > 0) {
					mod.append(" | ");
				}

				mod.append("kInsMod_" + inst.opcodeString.match(/\+r/)[0].replace("+", "p").replace("r", "R") + "d");
			}

			let operands = "kInsOps_none";
			if (inst.operands.length === 2) {
				if (inst.operands[0].type === "reg/mem" && inst.operands[1].imm === 32) {
					operands = "kInsOps_rm32imm32";
				}

				if (inst.operands[0].type === "reg" && inst.operands[1].type === "reg/mem") {
					operands = "kInsOps_r32rm32";
				}
			} else if (inst.operands.length === 1) {
				if (inst.operands[0].imm === 32) {
					operands = "kInsOpsimm32";
				}
			} else if (inst.operands.length === 0) {
				operands = "kInsOps_none";
			} else {
				dontparse = true;
			}

			if (mod._data === "") mod._data = "kInsMod_None";

			mod = mod.toString();
			if (!dontparse) {
				cdef.append(`\t{\"${inst.name}\", { 0x${inst.opcodeHex}, 0x${pp}, 0x${mm}, ${mod}, ${operands} } }`);

				if (i < ins.length - 3) {
					cdef.appendLine(",");
				} else {
					cdef.appendLine("");
				}
			}
		}

		cdef.appendLine("};");

		return cdef;
	}
}

let cpp = new Cpp();


let generator = new x86CDefGenerator();
let cdef = generator.build();

FileUtils.writeFile("x86.cdef", cdef, () => { });
