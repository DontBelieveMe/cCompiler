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
		/*this.isa.instructions.forEach((inst) => {
			if (inst.prefix === "") {
				let pp = inst.pp;
				let mm = inst.mm; 

				let opcode = `${pp} ${mm} ${inst.opcodeHex}`;
				let line = `"${inst.name}": ${opcode}, `;
				inst.operands.forEach((op) => {
					line += `${op.data},`;
				});
				cdef.appendLine(line);
			}
		});*/
		let ins = [];
		this.isa.instructions.forEach((inst) => {
			if (inst.arch === "ANY" && inst.prefix === "") {
				ins.push(inst);
			}
		});

		cdef.appendLine("std::unordered_map<std::string, InstructionData> instructions = {");
		ins.forEach((inst, index) => {
			cdef.append(`\t{\"${inst.name}\", { 0x${inst.opcodeHex} } }`);
			if (index < ins.length - 1) {
				cdef.appendLine(",");
			} else {
				cdef.appendLine("");
			}
		});
		cdef.appendLine("};");

		return cdef;//JSON.stringify(ins, null, 2);
		//console.dir(this.isa);
		//return cdef.toString();

	}
}

let cpp = new Cpp();


console.log(cpp.buildCtorCall("GPRegister", ["eax", "0x32"]));
console.log(cpp.buildCArray("registers", "int", [100, 200, 300, 400, 500]));

let generator = new x86CDefGenerator();
let cdef = generator.build();
//console.log(cdef);

FileUtils.writeFile("x86.cdef", cdef, () => { });
