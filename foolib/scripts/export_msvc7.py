import os, sys

buildscript = os.path.abspath(sys.argv[0])

def export(workspace):
	def WriteConfiguration(vcproj, name, build, clean, rebuild, output):
		vcproj.write("\t\t"+ '<Configuration' + "\n")
		vcproj.write("\t\t\t"+ 'Name="'+name+'|Win32"' + "\n")
		vcproj.write("\t\t\t"+ 'OutputDirectory="output/'+name+'"' + "\n")				# needed?
		vcproj.write("\t\t\t"+ 'IntermediateDirectory="intermediate/'+name+'"' + "\n")	# needed?
		vcproj.write("\t\t\t"+ 'ConfigurationType="0">' + "\n")
		vcproj.write("\t\t\t"+ '<Tool' + "\n")
		vcproj.write("\t\t\t\t"+ 'Name="VCNMakeTool"' + "\n")
		vcproj.write("\t\t\t\t"+ 'BuildCommandLine="'+build+' conf.name='+name+'"' + "\n")
		vcproj.write("\t\t\t\t"+ 'ReBuildCommandLine="'+rebuild+' conf.name='+name+'"' + "\n")
		vcproj.write("\t\t\t\t"+ 'CleanCommandLine="'+clean+' conf.name='+name+'"' + "\n")
		vcproj.write("\t\t\t\t"+ 'Output="'+output+'"/>' + "\n")
		vcproj.write("\t\t"+ '</Configuration>' + "\n")
	def WriteNode(vcproj, indent, thisnode):
		if len(thisnode.nodes): # group node
			if thisnode.name != "#ROOT#":
				vcproj.write("\t"*indent+'<Filter Name=\"'+thisnode.name+'">'+"\n")

			for node in thisnode.nodes:
				WriteNode(vcproj, indent+1, node)

			if thisnode.name != "#ROOT#":
				vcproj.write("\t"*indent+"</Filter>\n")
		else:
			vcproj.write("\t"*indent+'<File RelativePath="'+thisnode.name+'"/>'+"\n")

	vcproj = open("project.vcproj", "w")

	# xml tag
	vcproj.write('<?xml version="1.0" encoding="Windows-1252"?>' + "\n")

	# project tag
	vcproj.write('<VisualStudioProject' + "\n")
	vcproj.write("\t"+ 'ProjectType="Visual C++"' + "\n")
	vcproj.write("\t"+ 'Version="7.10"' + "\n")
	vcproj.write("\t"+ 'Name="'+workspace.name+'"' + "\n")
	vcproj.write("\t"+ 'ProjectGUID="{F382AEE6-1264-4ACA-B2FA-781A4C309545}"' + "\n")
	vcproj.write("\t"+ 'Keyword="MakeFileProj">' + "\n")

	# platform tag
	vcproj.write("\t"+ '<Platforms> <Platform Name="Win32"/> </Platforms>' + "\n")

	# configurations
	vcproj.write("\t"+ '<Configurations>' + "\n")
	start = buildscript+' '+workspace.filename+' '
	WriteConfiguration(vcproj, "debug", start+'build', start+'clean', start+'rebuild', "")
	WriteConfiguration(vcproj, "release", start+'build', start+'clean', start+'rebuild', "")
	vcproj.write("\t"+ '</Configurations>' + "\n")

	# references
	vcproj.write("\t"+ '<References>' + "\n")
	vcproj.write("\t"+ '</References>' + "\n")

	# files
	vcproj.write("\t"+ '<Files>' + "\n")
	WriteNode(vcproj, 1, workspace.root)
	vcproj.write("\t"+ '</Files>' + "\n")

	# globals
	vcproj.write("\t"+ '<Globals>' + "\n")
	vcproj.write("\t"+ '</Globals>' + "\n")

	#
	vcproj.write('</VisualStudioProject>' + "\n")
	vcproj.close()

	# config
	try:
		open("config.xml")
	except:
		config = open("config.xml", "w")
		config.write("<project>\n")
		config.write("</project>\n")

	# config
	try:
		batchfile = open("redo.bat", "w")
		batchfile.write("@echo off\n")
		batchfile.write("attrib +r config.xml\n")
		batchfile.write("attrib +r redo.bat\n")
		batchfile.write("del /A H /S /Q *\n")
		batchfile.write("for /D %%f in (*) do rd /s /q %%f\n")
		batchfile.write("python " + buildscript + " " + workspace.filename + " export\n")
		batchfile.write("attrib -r config.xml\n")
		batchfile.write("attrib -r redo.bat\n")
		confibatchfileg.write("pause\n")
	except:
		pass

