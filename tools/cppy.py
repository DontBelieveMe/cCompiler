import re, sys, types

def exit_with_error(reason):
	print(">> Templating error: " + reason, file=sys.stderr)
	sys.exit(1)

class CppTemplateFile:
	def __init__(self, templatefilename, outputfilename):
		self.templatefilename = templatefilename
		self.outputfilename = outputfilename

		self.env_vars = { 'self': self,
		'emit': self.emit,
		'emit_line': self.emit_line,
		'indent': self.indent}
		self.result = ''

	def register_var(self, variable_name, value):
		self.env_vars[variable_name] = value

	def emit(self, str_):
		self.result += str(str_)

	def emit_line(self, str_):
		self.result += str(str_) + '\n'

	def indent(self, times):
		self.result += times*'\t'

	def parse(self):
		template_file = open(self.templatefilename, 'r')
		template_text = template_file.read()

		code_block = ''
		parsing_code_block = False

		char_index = 0
		while char_index < len(template_text) - 1:
			char = template_text[char_index]

			if char == '$' and template_text[char_index + 1] == '(':
				parsing_code_block = True
				char_index += 1

			elif char == '$' and template_text[char_index + 1] == ')':
				parsing_code_block = False
				char_index += 1

				if '\n' in code_block:
					tmp = code_block
					code_block = code_block.replace('return', 'global ret_val; ret_val = ')
					expr_result = exec(code_block, self.env_vars)

					if code_block != tmp:
						global ret_val
						self.result += str(ret_val)

				else:
					self.result += str(eval(code_block, self.env_vars))

				code_block = ''
			else:
				if parsing_code_block:
					code_block += char
				else:
					self.result += char

			char_index += 1

		template_file.close()

		return self.result
