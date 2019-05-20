class CppTemplateFile:
    def __init__(self, templatefilename, outputfilename):
        self.templatefilename = templatefilename
        self.outputfilename   = outputfilename
        
        # Exposing self as an environment variable is a little
        # hack so that the template scripts can call the member
        # functions (of CppTemplateFile) that are also exposed.
        self.env_vars = {
            'self'     : self,
	    'emit'     : self.emit,
	    'emit_line': self.emit_line,
	    'indent'   : self.indent
        }

        self.result = ''
        
    # Register a global variable that can be accessed by any python scripts inside
    # the template file.
    def register_var(self, variable_name, value):
        self.env_vars[variable_name] = value
    
    # Template API function (do not use outside of template file)
    # Append the given object as a string to the result/output string
    def emit(self, str_):
            self.result += str(str_)
    
    # Template API function (do not use outside of template file)
    # Append the given object (as a string) to the result/output string
    # followed by a newline
    def emit_line(self, str_):
            self.result += str(str_) + '\n'
    
    # Template API function (do not use outside of template file)
    # Append a tab character 'times' number of times to the result/output
    # string
    def indent(self, times):
            self.result += times*'\t'

    def parse(self):
        template_file      = open(self.templatefilename, 'r')
        template_text      = template_file.read()

        code_block         = ''
        parsing_code_block = False

        char_index         = 0

        while char_index < len(template_text) - 1:
            char = template_text[char_index]
            
            # Check if this character starts a python code block - using $(
            if char == '$' and template_text[char_index + 1] == '(':
                parsing_code_block = True
                char_index += 1
            
            # Check if this character ends a python code block - using $)
            elif char == '$' and template_text[char_index + 1] == ')':
                parsing_code_block = False
                char_index += 1
                
                # So many hacks here...

                # 1) checking if '\n' is present is not a good method
                # of determining if code should be executed as a expression
                # or a mini/full on a program
                if '\n' in code_block:

                    # 2) tmp is used to store code_block before any replace
                    # and then is used to check if anything has been replaced
                    # (aka if code_block = tmp then nothing has changed)
                    # this is a _bad way of doing this.
                    tmp = code_block

                    # 3) HUGE hack. Programs don't return a value in normal python, but I
                    # want to be able to return values from multiline scripts in template files
                    # So this hack means you can type "return ..." in the script and it will be
                    # replaced with some code that will take the `...` and store it in a global
                    # variable, that is then retrievd and appended to the final string, IF
                    # the code block had the return in the first place.
                    # ... sigh ...
                    code_block = code_block.replace('return', 'global ret_val; ret_val = ')
                    expr_result = exec(code_block, self.env_vars)

                    if code_block != tmp:
                        global ret_val
                        self.result += str(ret_val)

                else:
                    self.result += str(eval(code_block, self.env_vars))

                code_block = ''
            else:
                # Either add this char to the code_block or the final string,
                # depending on whether we are in a code block or not (aka inside a
                # $(/$) pair.
                if parsing_code_block:
                    code_block += char
                else:
                    self.result += char

            char_index += 1

        template_file.close()

        return self.result
