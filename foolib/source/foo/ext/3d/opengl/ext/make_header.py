import os, sys, string, re

tokens = []
functions = []

extname = sys.argv[1]

file = open(extname + ".txt")
output_h = open("../ext_" + extname + ".h", "w")
output_cpp = open("../ext_" + extname + ".cpp", "w")

line = file.readline()
mode = 0;

while line:
	line = re.sub("^", " ", line)
	line = re.sub("\(", "( ", line)
	line = re.sub(" ubyte", " GLubyte", line)
	line = re.sub(" byte", " GLbyte", line)
	line = re.sub(" uint", " GLuint", line)
	line = re.sub(" int", " GLint", line)
	line = re.sub(" ushort", " GLushort", line)
	line = re.sub(" short", " GLshort", line)

	line = re.sub(" enum", " GLenum", line)
	line = re.sub(" float", " GLfloat", line)
	line = re.sub(" double", " GLdouble", line)
	line = re.sub(" boolean", " GLboolean", line)

	line = string.lstrip(line)

	if line[0] == '#':
		mode = mode + 1
	else:
		if mode == 0:
			output_h.write("#define GL_" + line)
		else:
			functions.append(line)
		
	line = file.readline()


for func in functions:
	parts = string.split(func, " ")
	output_h.write("typedef " + parts[0] + " (GLFUNC *PFN" + string.upper(parts[1]) + "PROC)" + string.join(parts[2:]))

for func in functions:
	parts = string.split(func, " ")
	output_h.write("extern " + "PFN" + string.upper(parts[1]) + "PROC glext" + parts[1] + ";\n")

output_h.write("\n\nbool GLExtInit_"+extname+"();\n\n")

output_cpp.write("#include \"opengl_include.h\"\n\n")
output_cpp.write("#include \"ext_"+extname+".h\"\n\n")
output_cpp.write("extern void *GLExtInit_GetProcAddress(const char *_pName);\n\n")

for func in functions:
	parts = string.split(func, " ")
	output_cpp.write("PFN" + string.upper(parts[1]) + "PROC glext" + parts[1] + " = 0;\n")


output_cpp.write("\n\nbool GLExtInit_"+extname+"()\n{\n")

for func in functions:
	parts = string.split(func, " ")
	output_cpp.write("\tglext" + parts[1] + " = (PFN" + string.upper(parts[1]) + "PROC)GLExtInit_GetProcAddress(\"gl"+parts[1]+"\");\n")

output_cpp.write("\n\n\tif(\n")

for func in functions:
	parts = string.split(func, " ")
	output_cpp.write("\t\tglext" + parts[1] + " &&\n")

output_cpp.write("\t\t1\n\t)\n\t{\n\t\treturn true;\n\t}\n\n\treturn false;\n")

output_cpp.write("}\n")

