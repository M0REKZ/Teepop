#!/usr/bin/python

#
# todo:
# 		FIXED! settings passing from commandline
# settings thru a config file
# cleaner syntax in the config files
# fix robustness of ProcessVars
# fix so compilers och linkers can check if nessesary tools exist
# 		FIXED! every tool should be able to set default settings into the globalsettings registry
# some sort of scoping for settings
# 		FIXED! recursive search for subprojects
# selection of exporter
# 		FIXED! compiler should save the settings that were used to build the object files.
# same goes for the linkers
# the pickled stuff should perhaps be stored in a global object that is loaded at start and saved later on?
# 		FIXED! bundling on OSX
# 		FIXED! linker output should create path
#

import xml.sax
import os
import string
import sys
import stat # premissions
import shutil # needed for filecopying
import cPickle # object serialization

## setup basic vars
thisscript = os.path.abspath(sys.argv[0])
currentdir = os.getcwd();
globalsettings = {}
doneprojects = {}

class CWorkspaceNode:
	class CNode:
		def __init__(self):
			self.name = ""
			self.nodes = []
		def Print(self,indent):
			print " "*indent+self.name
			for node in self.nodes:
				node.Print(indent+4)
		def Sort(self):
			nodes = {}
			for node in self.nodes:
				nodes[node.name] = node

			sortedkeys = nodes.keys()
			sortedkeys.sort()

			self.nodes = []
			for nodename in sortedkeys:
				if len(nodes[nodename].nodes):
					self.nodes.append(nodes[nodename])
			for nodename in sortedkeys:
				if not len(nodes[nodename].nodes):
					self.nodes.append(nodes[nodename])

			for node in self.nodes:
				node.Sort()

	def __init__(self):
		self.filename = ""
		self.name = ""
		self.root = CWorkspaceNode.CNode()
		self.root.name = "#ROOT#"




##
## platform tools
##
def GetCPU(s):
	if string.find(s, "i386") != -1: return "x86"
	elif string.find(s, "i486") != -1: return "x86"
	elif string.find(s, "i586") != -1: return "x86"
	elif string.find(s, "i686") != -1: return "x86"
	elif string.find(s, "powerpc") != -1: return "ppc"
	elif string.find(s, "ppc") != -1: return "ppc"
	elif string.find(s, "mingw32") != -1: return "x86"
	return "unknown"

def GetPlatform(s):
	if string.find(s, "darwin") != -1: return "darwin"
	elif string.find(s, "win32") != -1: return "win32"
	elif string.find(s, "mingw32") != -1: return "win32"
	elif string.find(s, "linux") != -1: return "linux"
	elif string.find(s, "freebsd") != -1: return "freebsd"
	elif string.find(s, "openbsd3") != -1: return "openbsd"
	return "unknown"

def GetTarget(s):
	return GetCPU(s)+"-"+GetPlatform(s)

##
##
##########################
def GetSettingList(name):
	list = []
	for key in globalsettings:
		value = globalsettings[key]
		if len(key) < len(name):
			continue
		if key[:len(name)] == name and value > 0:
			list.append(key[len(name)+1:])
	return list
##
##
##########################
def GetSettingListValue(name):
	list = []
	for key in globalsettings:
		value = globalsettings[key]
		if len(key) < len(name):
			continue
		if key[:len(name)] == name and value > 0:
			list.append(value)
	return list
	
##
## replaces {var} will the corrisponding value in globalsettings
## this should be change to something more robust
##
##############
def ProcessVars(str):
	for var in globalsettings:
		str = string.replace(str, "{"+var+"}", globalsettings[var])
	return str

##
##
##########################
def CleanList(list):
	newlist = []
	for elm in list:
		if not elm in newlist:
			newlist.append(elm)
	return newlist
##
##
##########################
def PrintFile(f):
	line = f.readline()
	while line:
		print line,
		line = f.readline()
##
##
##
def DeleteFile(filename):
	try:
		os.stat(filename)
		print "deleting " + os.path.basename(filename)
		os.remove(filename)
	except: pass
##
##
##########################
def CreatePath(path):
	try: os.makedirs(path)
	except: pass

##
##
##########################
class CXMLHandler:
	def xmlStart(self, name, attrs): pass
	def xmlChar(self, ch): pass
	def xmlEnd(self, name): pass

##
##
##########################
Action_Build = 0
Action_Clean = 1
Action_Rebuild = 2
Action_Export = 3	# when it's time to export to an IDE
Action_Nothing = 4
Action_Dump = 5

objects = {}
class CObject:
	xmlinputchar = ""
	def __init__(self, parent):
		self.name = "unknown_object"
		self.tag = "unknown_object"
		self.handler = "no"
		self.parent = parent
		self.objects = []
		self.parseheader = 0

	def Init(self):
		pass

	def Dump(self, indent=0):
		print "  "*indent+self.tag+" - "+self.name
		for obj in self.objects:
			obj.Dump(indent+1)

	##
	##
	##
	def Perform(self, action): ## returns a list of result files and the latest timestamp
		return [], 0

	##
	## returns the project
	##
	def GetProject(self):
		if self.tag == "project":
			return self
		return self.parent.GetProject()

	##
	##
	##############
	def ProcessPath(self, str):
		if str[0] == "{":
			return ProcessVars(str)
		path = os.path.abspath(os.path.join(self.GetProject().path, ProcessVars(str)))
		if os.name == "nt":
			path = path.lower()
		return path
		
#		if string.find(str, ";"):
#			list = string.split(str, ";")
#			if list[0] == "cur":
#				return os.path.abspath(os.path.join(currentdir, list[1]))
#			elif list[0] == "none":
#				return list[1]
#		return os.path.abspath(os.path.join(self.GetProject().path, str))

	##
	##
	##
	##############
	def FindObject(self, name):
		for object in self.objects:
			if object.name == name:
				return object
		if self.parent:
			return self.parent.FindObject(name)
		return None

	##
	##
	##############
	def FindObjectsByTag(self, tag, recursive=0):
		objects = []
		for object in self.objects:
			if object.tag == tag:
				objects.append(object)
			elif recursive:
				objects += object.FindObjectsByTag(tag, recursive)
		return objects
	##
	##
	#########
	def BuildWorkspace(self, parentnode):
		files = self.FindObjectsByTag(CFile(None).tag)
		groups = self.FindObjectsByTag(CGroup(None).tag)
		everything = files+groups
		for obj in everything:
			obj.BuildWorkspace(parentnode)

	##
	##
	#########
	def xmlStart(self, name, attrs):
		if self.parseheader == 1:
			self.name = attrs.get('name', 'unnamed_object')
		elif name in objects:
			o = objects[name](self)
			self.objects.append(o)
			o.parseheader = 1
			o.xmlStart(name, attrs)
			o.parseheader = 0
			if o.handler == "yes":
				return o
	def xmlChar(self, content):
		CObject.xmlinputchar += content

	def xmlEnd(self, name):
		if name == self.tag:
			return 10

##
## Use this to register objectgs
##
def RegisterObject(o):
	objects[o(None).tag] = o;

##
## A set of stuff, could be files
##
class CSet(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "set"
		self.handler = "yes"
	def Perform(self, action):
		list = []
		latest = 0
		for obj in self.objects:
			output, timestamp = obj.Perform(action)
			if latest < timestamp: latest = timestamp
			list += output
		return list, latest
RegisterObject(CSet)

##
## A set of stuff, could be files
##
##########################
class CSubProject(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "subproject"
		self.handler = "yes"
		self.filename = ""
	##
	###########
	def xmlStart(self, name, attrs):
		if name == self.tag:
			self.filename = self.ProcessPath(attrs.get('file', ''))
			#print self.GetProject().path, "SubProj:", self.filename
		CObject.xmlStart(self, name, attrs)

	def GetFilename(self):
		return os.path.join(self.GetProject().path, self.filename)
	##
	###########
	def Perform(self, action):
		return DoProject(self.GetFilename(), action)

RegisterObject(CSubProject)

##
##
##
##########################
class CSetting(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "setting"
	def xmlStart(self, name, attrs):
		name = attrs.get('name', '.')
		value = attrs.get('value', None)


		if value == None:
			pathval = attrs.get('path', None)
			if pathval != None:
				value = self.ProcessPath(pathval)
				#print "got path", value

		if value == None:
			value = "1"
		#process = attrs.get('process', "none")

		#if process == "path":
		#print value, ProcessVars(value)
		#print name
		value = ProcessVars(value) #self.ProcessPath(value)
		globalsettings[name] = value
RegisterObject(CSetting)

##
##
##
##########################
class CBasicTool(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "BASICTOOL"
		self.inputs = []		## dict {'name', 'obj', 'ignoreoutput'}
		self.rawinputs = []
		self.handler = "yes"
		self.ignoreoutput = 0
	def xmlStart(self, name, attrs):
		if name == "input":
			self.ignoreoutput = attrs.get('ignoreoutput', 0)
		CObject.xmlStart(self, name, attrs)
	def xmlEnd(self, name):
		if name == "input":
			self.inputs.append({'name':CObject.xmlinputchar, 'ignoreoutput':self.ignoreoutput})
		if name == "rawinput":
			self.rawinputs.append(CObject.xmlinputchar)
		return CObject.xmlEnd(self, name)
	def ResolveInputs(self):
		for input in self.inputs:
			input['obj'] = self.FindObject(input['name'])
			if not input['obj']:
				print "WARNING("+self.GetProject().name+"): Object not found, "+input['name']
	def PerformInputs(self, action):
		output = []
		latest = 0
		for input in self.inputs:
			if input['obj']:
				performedinput, time = input['obj'].Perform(action)
				if not input['ignoreoutput']:
					output += performedinput

				if latest < time: latest = time
		output += self.rawinputs
		return output, latest

	def Perform(self, action):
		return [], 0

##
##
##########################
class CChain(CBasicTool):
	def __init__(self, parent):
		CBasicTool.__init__(self, parent);
		self.tag = "chain"
		self.rawoutput = []
	def xmlEnd(self, name):
		if name == "rawoutput":
			self.rawoutput.append(CObject.xmlinputchar)
		return CBasicTool.xmlEnd(self, name)
	def Perform(self, action):
		#print "chain doing stuff"
		#print self.inputnames
		self.ResolveInputs()
		inputs, time = self.PerformInputs(action)
		return inputs+self.rawoutput, time
RegisterObject(CChain)

##
##
##########################
class CCopy(CBasicTool):
	def __init__(self, parent):
		CBasicTool.__init__(self, parent);
		self.tag = "copy"
		self.output = ""
		self.timecheck = 0
	def xmlStart(self, name, attrs):
		if name == self.tag:
			self.timecheck = attrs.get('timecheck', 1)
		return CBasicTool.xmlStart(self, name, attrs)
	def xmlEnd(self, name):
		if name == "output":
			self.output = self.ProcessPath(CObject.xmlinputchar)
		return CBasicTool.xmlEnd(self, name)
	def Perform(self, action):
		self.ResolveInputs()
		inputs, latest = self.PerformInputs(action)

		outputs = []
		for input in inputs:
			#print self.output
			#print input #os.path.basename(input)
			output = os.path.join(self.output, os.path.basename(input))
			outputs += [output]

			if action == Action_Build:
				inputtime = os.stat(input).st_mtime
				
				if inputtime < latest: latest = inputtime

				if self.timecheck:
					try:
						outputtime = os.stat(output).st_mtime
						if inputtime <= outputtime:
							continue
					except:
						CreatePath(os.path.dirname(output))
						pass

				shutil.copyfile(input, output)
			elif action == Action_Clean:
				DeleteFile(output)
		return outputs, latest
RegisterObject(CCopy)

##
##
##########################
class CFile(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent);
		self.tag = "file"
	def BuildWorkspace(self, parentnode):
		thisnode = CWorkspaceNode.CNode()
		thisnode.name = self.ProcessPath(self.name)
		parentnode.nodes.append(thisnode)
		CObject.BuildWorkspace(self, thisnode)
	def Perform(self, action):
		filename = self.ProcessPath(self.name)
		try:
			return [filename], os.stat(filename).st_mtime
		except:
			pass
		return [filename], 0
RegisterObject(CFile)

##
##
##########################
class CString(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent);
		self.tag = "string"
	def Perform(self, action):
		return [self.name], 0
RegisterObject(CString)

##
##
##
class CCPPCompiler(CBasicTool):
	compilers = []

	class CCompiler:
		def Init(self, tool):
			pass
		def Dump(self, tool):
			pass

	##
	## Microsoft Visual C++ 7.1 (.NET 2003)
	##
	class CCompiler_msvc7(CCompiler):
		name = "msvc7"
		supports = []
		def Init(self, tool):
			try:
				mstdin, mstdout, mstderr = os.popen3("cl")
				row = mstdout.readline()
				mstdin.close(); mstdout.close(); mstderr.close()
				if string.find(row, "usage: cl") != -1:
					tool.includepaths += string.split(os.getenv('INCLUDE'), ";")
					self.supports += ["x86-win32"]
			except:
				pass
		def compile(self, config):
			defines = ""
			for define in config['defines']:
				defines += " -D"+define

			includes = ""
			for include in config['includes']:
				includes += " -I"+include

			## /Yd = debuginfo in every object file
			## /Zi = debuginfo
			## /ZI = debuginfo, Edit&Continue
			## /MT = link against LIBCMT.LIB
			## /EHs = synchronous exception handling model
			execstring = "cl /nologo /MT /c /EHs "
			if config['debuginfo'] == 'true':
				execstring += "/Zi /Yd /MTd "
			if config['optimize'] == 'true':
				execstring += "/Ox /O2 /Og "
			execstring += config['input'] + " " + includes + " " + defines + " /Fo" + config['output'] + " "
			if int(globalsettings['verbose']) > 0:
				print execstring

			compile = os.popen(execstring)
			compile.readline()
			PrintFile(compile)
			compile.close()
	compilers.append(CCompiler_msvc7)

	##
	## GCC, needs to be fixed
	##
	class CCompiler_gcc(CCompiler):
		name = "gcc"
		supports = []
		def Init(self, tool):
			try:
				gpp = os.popen("g++ -dumpmachine")
				target = gpp.readline()
				target = target[0:len(target)-1]
				gpp.close()
				target = GetTarget(target)
				if string.find(target, "unknown") == -1: # only allow specific targets
					self.supports.append(target)
			except:
				pass
		def compile(self, config):
			defines = ""
			for define in config['defines']:
				defines += " -D"+define

			includes = ""
			for include in config['includes']:
				includes += " -I"+include

			#execstring = "g++ -pedantic -ansi "
			execstring = "g++ -ansi "
			if config['debuginfo'] == 'true':
				execstring += "-g "
			if config['optimize'] == 'true':
				execstring += "-O3 "
			if globalsettings['system.platform'] == 'darwin':
				execstring += "-fmessage-length=0 " ## forces so there will be no line wrapping
			execstring += "-c " + config['input'] + " " + includes+" "+defines + " -o" + config['output'] + " "

			if int(globalsettings['verbose']) > 0:
				print execstring
			os.system(execstring)
	compilers.append(CCompiler_gcc)

	##
	##
	##
	def __init__(self, parent):
		CBasicTool.__init__(self, parent);
		self.tag = "cxx"
		self.outputdir = ""
		self.defines = []
		self.includepaths = []
		
	def Init(self):
		globalsettings['cxx.optimize'] = 'false'
		globalsettings['cxx.debuginfo'] = 'true'
		## check for compilers
		for comp in self.compilers:
			i = comp()
			i.Init(self)
			if len(comp.supports) > 0:
				globalsettings['cxx.compiler'] = comp.name


	def Dump(self,indent=0):
		for comp in self.compilers:
			i = comp()
			print "  "+i.name
			print "    Supports:", comp.supports
			i.Dump(self)
			

	def xmlEnd(self, name):
		if name == "outputdir":
			self.outputdir = self.ProcessPath(CObject.xmlinputchar)
		elif name == "define":
			self.defines.append(CObject.xmlinputchar)
		else:
			return CBasicTool.xmlEnd(self, name)
			
	##
	## Goes thru the include list inorder to find the include file
	##
	#####################
	def FindFile(self, filename):
		for inc in self.includepaths:
			try:
				final = os.path.join(inc, filename)
				os.stat(final)
				return final
			except: pass
		#print "Couldn't find", filename
		return filename

	##
	## This function walks thru all headers and see if something need to be updates.
	## A large optimization chould be done if this just built a dependency tree once.
	##
	########################
	deps = []

	def GetDependencies(self, filename):
		if filename in CCPPCompiler.deps:
			return
		CCPPCompiler.deps.append(filename)

		try:
			dir = os.path.dirname(filename)
			f = open(filename)

			line = f.readline()
			while line:
				line = string.lstrip(line)
				if string.find(line, '#include') != -1:
					if string.find(line, '<') != -1:
						line = string.replace(line, '<', '"')
						line = string.replace(line, '>', '"')
						list = string.split(line, '"')
						dep = self.FindFile(list[1])
					else:
						list = string.split(line, '"')
						dep = os.path.join(dir, list[1])

					self.GetDependencies(dep)

				line = f.readline()
		except:
			pass
		return

	##
	## This function walks thru all headers and see if something need to be updates.
	## Optimized from the version above. Only check one file one time. Keeps record
	## of all timestamps of the files
	##
	########################
	timestamps = {}
	def GetTimestamp(self, filename):
		absname = os.path.abspath(filename)
		if absname in CCPPCompiler.timestamps:
			return CCPPCompiler.timestamps[absname]

		latest = 0
		try:
			latest = os.stat(filename).st_mtime
		except: pass

		CCPPCompiler.timestamps[absname] = latest ## prevents circular depencenices

		try:
			dir = os.path.dirname(absname)
			f = open(absname)

			line = f.readline()
			while line:
				line = string.lstrip(line)
				if len(line) > 0 and line[0] == '#': ## opt
					if line[:len('#include')] == '#include':
						if string.find(line, '<') != -1:
							line = string.replace(line, '<', '"')
							line = string.replace(line, '>', '"')
							list = string.split(line, '"')
							dep = self.FindFile(list[1])
						else:
							list = string.split(line, '"')
							dep = os.path.join(dir, list[1])

						time = self.GetTimestamp(dep)
						if time > latest: latest = time

				line = f.readline()
		except:
			pass

		CCPPCompiler.timestamps[absname] = latest
		return latest

	##
	##
	###############################
	def Perform(self, action):
		## do all dependencies first
		self.ResolveInputs()
		inputs, latest = self.PerformInputs(action)

		## find suitable compiler
		compiler = None
		compilername = ""
		
		if action == Action_Build:
			for c in CCPPCompiler.compilers:
				if globalsettings['cxx.target'] in c.supports:
					compiler = c()
					compilername = c.name
					break
	
			if compiler == None:
				print "Couldn't find a c++ compiler that supports " + globalsettings['cxx.target']
				sys.exit()

		## add search paths
		self.includepaths += GetSettingListValue('cxx.include')
		
		outputs = []

		## config for compiler
		config = {}
		config['optimize'] = globalsettings['cxx.optimize']
		config['debuginfo'] = globalsettings['cxx.debuginfo']
		config['includes'] = GetSettingListValue('cxx.include')
		config['defines'] = GetSettingList('cxx.define')
		config['defines'] += ["FOO_COMPILER_"+string.upper(compilername)]

		## SPEEDHACK!!!
		if int(globalsettings['cxx.speedhack']) >= 1:
			output = os.path.join(self.outputdir, os.path.basename(inputs[0])+".speedhack.o")
			speedhackcpp = os.path.join(self.outputdir, os.path.basename(inputs[0])+".speedhack.cpp")
			cfile = file(speedhackcpp, "w")
			inputfiles = ""
			for inputfile in inputs:
				inputfiles += os.path.basename(inputfile) + " "
				cfile.write('#include "'+inputfile+'"'+"\n")
			cfile.flush()
			cfile.close()

			##
			config['input'] = speedhackcpp
			config['output'] = output
				
			print "speedcxx("+compiler.name+") " + inputfiles
			sys.stdout.flush()
			compiler.compile(config)
			try:
				latest = os.stat(output).st_mtime
			except: pass
			outputs += [output]

			return outputs, latest



		## process all inputs
		for inputfile in inputs:

			## only compile .cpp files
			validfiles = ["c++","cxx","cpp", "c"]
			list = string.split(inputfile, ".")
			if not list[len(list)-1] in validfiles:
				continue
			
			## fix output name
			output = inputfile + ".o." + globalsettings['cxx.target']
			if len(self.outputdir):
				output = os.path.join(self.outputdir, os.path.basename(inputfile)+".o")
				
			conf = output + ".conf"

			if action == Action_Build:
				## check if the output exist
				rebuild = 0
				try:
					outputtime = os.stat(output)
				except:
					CreatePath(os.path.dirname(output))
					rebuild = 1
					if int(globalsettings['verbose']) >= 1:
						print "rebuild because output doesn't exist"

				## walk thru all files an see if they have a newer timestamp than the object file
				if rebuild == 0:
					if globalsettings['cxx.target'] == "win32-x86": ## ugly hack, but makes things alot faster
						CCPPCompiler.timestamps[self.FindFile("windows.h")] = 0

					timestamp = self.GetTimestamp(inputfile)
					if timestamp > outputtime.st_mtime:
						if int(globalsettings['verbose']) >= 1:
							print "rebuild because of timestamp", outputtime.st_mtime, timestamp
						rebuild = 1

				##
				config['input'] = inputfile
				config['output'] = output
				
				## check if the configuration have changed
				if rebuild == 0:
					gotconf = 0
					try:
						storedconf = cPickle.load(open(conf, "rb"))
						gotconf = 1
					except:
						if int(globalsettings['verbose']) >= 1:
								print "rebuild because no stored settings where found"
						rebuild = 1
					if gotconf:
						if storedconf != config:
							if int(globalsettings['verbose']) >= 1:
								print "rebuild because of settings", config, storedconf
								for key in config:
									if key in storedconf:
										if config[key] != storedconf[key]:
											print "  ", config[key], " != ", storedconf[key]
									else:
										print "  ", key, "is new"
								for key in storedconf:
									if not key in config:
										print "  ", key, "is old"
							rebuild = 1

				## build if nessasary
				if rebuild > 0:
					#try:
						cPickle.dump(config, open(conf, "wb"))
						print "cxx("+compiler.name+") " + os.path.basename(inputfile)
						sys.stdout.flush()
						compiler.compile(config)
						try:
							latest = os.stat(output).st_mtime
						except: pass
					#except:
					#	print "compiler error"
			elif action == Action_Clean:
				DeleteFile(output)
				DeleteFile(conf)
			outputs += [output]

		return outputs, latest
RegisterObject(CCPPCompiler)

##
##
##########################
class CLinker(CBasicTool):
	linkers = []

	class CLinker_ar:
		name = "ar"
		supports = []
		def Init(self, linker):
			try:
				mstdin, mstdout, mstderr = os.popen3("ar")
				usage = mstderr.readline()
				mstdin.close(); mstdout.close(); mstderr.close()
				if string.find(usage, "sage: ") != -1: # we have ar
					self.supports.append(globalsettings['system.cpu'] + "-" + globalsettings['system.platform']+"-lib")
			except:
				pass
		def Dump(self, linker):
			pass
		def getoutputname(self, linker):
			return linker.output+".a"
		def link(self, linker, inputs):
			if linker.type == "library":
				DeleteFile(linker.output)
				execline = "ar -rcs " + linker.output + " " + string.join(inputs, " ")
				if int(globalsettings['verbose']) > 0:
					print execline
				mstdin, mstdout, mstderr = os.popen3(execline)
				line = mstderr.readline()
				while line:
					if string.find(line, 'has no symbols') != 0:
						print line,
					line = mstderr.readline()
	linkers.append(CLinker_ar)

	def CreateBundle(self, name):
	
		# create package structure
		CreatePath(name+".app/Contents/MacOS")
		
		# write pkginfo file
		Pkginfo = open(name+".app/Contents/Pkginfo", "wb")
		Pkginfo.write("APPL\x3f\x3f\x3f\x3f")
		Pkginfo.close()
		
		# write info plist
		Infofile = open(name+".app/Contents/Info.plist", "w")
		Infofile.write('<?xml version="1.0" encoding="UTF-8"?>' + "\n");
		Infofile.write('<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' + "\n");
		Infofile.write('<plist version="1.0">' + "\n");
		Infofile.write('<dict>' + "\n");
		Infofile.write('	<key>CFBundleDevelopmentRegion</key>' + "\n");
		Infofile.write('	<string>English</string>' + "\n");
		Infofile.write('	<key>CFBundleExecutable</key>' + "\n");
		Infofile.write('	<string>'+os.path.basename(name)+'</string>' + "\n");
		Infofile.write('	<key>CFBundleIconFile</key>' + "\n");
		Infofile.write('	<string></string>' + "\n");
		Infofile.write('        <key>CFBundleIdentifier</key>' + "\n");
		Infofile.write('	<string>com.apple.myCarbonNibApp</string>' + "\n");
		Infofile.write('	<key>CFBundleInfoDictionaryVersion</key>' + "\n");
		Infofile.write('	<string>6.0</string>' + "\n");
		Infofile.write('	<key>CFBundlePackageType</key>' + "\n");
		Infofile.write('	<string>APPL</string>' + "\n");
		Infofile.write('	<key>CFBundleSignature</key>' + "\n");
		Infofile.write('	<string>????</string>' + "\n");
		Infofile.write('	<key>CFBundleVersion</key>' + "\n");
		Infofile.write('	<string>0.1</string>' + "\n");
		Infofile.write('	<key>CSResourcesFileMapped</key>' + "\n");
		Infofile.write('	<true/>' + "\n");
		Infofile.write('</dict>' + "\n");
		Infofile.write('</plist>' + "\n");
		Infofile.close()
		
		# copy exe into bundle
		finalexe = name+".app/Contents/MacOS/"+os.path.basename(name)
		shutil.copyfile(name, finalexe)
		prem = stat.S_IEXEC
		prem |= stat.S_IXOTH+stat.S_IROTH
		prem |= stat.S_IXGRP+stat.S_IRGRP
		prem |= stat.S_IXUSR+stat.S_IRUSR+stat.S_IWUSR
		os.chmod(finalexe,prem)

	class CLinker_gcc:
		name = "gcc"
		supports = []
		def Init(self, linker):
			try:
				gpp = os.popen("g++ -dumpmachine")
				target = gpp.readline()
				target = target[0:len(target)-1]
				gpp.close()
				target = GetTarget(target)
				if string.find(target, "unknown") == -1: # only allow specific targets
					self.supports.append(target+"-exe")
			except:
				pass
		def Dump(self, linker):
			pass
		def link(self, linker, inputs):
			if linker.type == "executable":
				inputs = CleanList(inputs)
				# --start-gorup is important!! gcc one-pass linker crapiness
				# I don't know what version this was introduced/removed
				# It doesn't exist on os x 10.3, gcc 3.3
				if globalsettings['system.platform'] == 'darwin':
					execstring = "g++ -all_load -o " + linker.output + " " + string.join(inputs, " ") 
				else:
					execstring = "g++ -all_load -o " + linker.output + " -Wl,--start-group " + string.join(inputs, " ") + " -Wl,--end-group" 
				#execstring = "g++ -all_load -o " + linker.output + " " + string.join(inputs, " ") 
				if int(globalsettings['verbose']) > 0:
					print execstring
				os.system(execstring)
				if globalsettings['system.platform'] == 'darwin':
					linker.CreateBundle(linker.output)
	linkers.append(CLinker_gcc)

	class CLinker_msvc7:
		name = "msvc7"
		supports = []
		def Init(self, linker):
			try:
				mstdin, mstdout, mstderr = os.popen3("cl")
				row = mstdout.readline()
				mstdin.close(); mstdout.close(); mstderr.close()
				if string.find(row, "usage: cl") != -1:
					self.supports.append("x86-win32-lib")
					self.supports.append("x86-win32-exe")
			except:
				pass
		def Dump(self, linker):
			pass
		def link(self, linker, inputs):
			if linker.type == "library":
				execstring = "lib /nologo /out:" + linker.output + " " + string.join(inputs, " ")
				if int(globalsettings['verbose']) > 0:
					print execstring
				link = os.popen(execstring)
				PrintFile(link)
			elif linker.type == "executable":
				execstring = "link /nologo /incremental:no /map:"+linker.output+".map"
				if globalsettings['linker.debuginfo'] == 'true':
					execstring += " /debug "
				else:
					execstring += " /release "

				execstring += " /stack:"+str(linker.stacksize)+" /pdb:"+os.path.basename(linker.output)+".pdb /subsystem:windows /out:" + linker.output + " " + string.join(inputs, " ")
				if int(globalsettings['verbose']) > 0:
					print execstring
				try:
					link = os.popen(execstring)
					PrintFile(link)
					shutil.copyfile(linker.output,  linker.output+".exe")
				except:
					pass
			else:
				print "HELP!"
	linkers.append(CLinker_msvc7)

	def __init__(self, parent):
		CBasicTool.__init__(self, parent);
		self.tag = "link"
		self.type = "exe"
		self.stacksize = 1024*1024
	def xmlEnd(self, name):
		if name == "type":
			self.type = CObject.xmlinputchar
		elif name == "stacksize":
			self.stacksize = int(CObject.xmlinputchar)
		elif name == "output":
			self.output = self.ProcessPath(CObject.xmlinputchar)
		else:
			return CBasicTool.xmlEnd(self, name)

	def Init(self):
		## check for linkers
		for linker in self.linkers:
			(linker()).Init(self)

	def Dump(self,indent=0):
		for linker in self.linkers:
			i = linker()
			print "  "+i.name
			print "    Supports:", linker.supports
			i.Dump(self)

	def Perform(self, action):
		self.ResolveInputs()
		inputs, latest = self.PerformInputs(action);
		output = str(self.output)

		if action == Action_Build:
			build = 1
			try:
				if os.stat(output).st_mtime > latest:
					build = 0
			except:
				pass

			if build:
				targetname = globalsettings['system.cpu'] + "-" + globalsettings['system.platform']
				if self.type == "library":
					targetname += "-lib"
				elif self.type == "executable":
					targetname += "-exe"

				linker = None
				for l in CLinker.linkers:
					if targetname in l.supports:
						linker = l()
						break

				if linker == None:
					print "Couldn't find a linker that supports "+targetname
					sys.exit()
	
#				try:
				CreatePath(os.path.dirname(output))
				
				print "link("+linker.name+") " + os.path.basename(output)
				try:
					sys.stdout.flush()
					linker.link(self,inputs)
				except:
					print "link failed"
					
				try:
					latest = os.stat(output).st_mtime
				except:
					pass
				if globalsettings['cxx.compiler'] == 'msvc7':
					print "\xff" # msvc removes single lines
				else:
					print ""
#				except:
#					print "linker error"
		elif action == Action_Clean:
			DeleteFile(output)

		return [self.output], latest
RegisterObject(CLinker)

##
##
##
##########################
class CWorkspace(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "workspace"
		self.base = ""
		self.handler = "yes"
	def xmlStart(self, name, attrs):
		if name == self.tag:
			self.base = attrs.get("base", "")
		return CObject.xmlStart(self, name, attrs)

RegisterObject(CWorkspace)

class CGroup(CObject):
	def __init__(self, parent):
		CObject.__init__(self, parent)
		self.tag = "group"
		self.handler = "yes"
	def BuildWorkspace(self, parentnode):
		print parentnode.name,"->", self.name
		thisnode = CWorkspaceNode.CNode()
		thisnode.name = self.name
		parentnode.nodes.append(thisnode)
		CObject.BuildWorkspace(self, thisnode)
RegisterObject(CGroup)

##
##
##########################
class CProject(CObject, CXMLHandler):
	def __init__(self):
		CObject.__init__(self, None)
		self.tag = "project"
		self.name = ""
		self.defaultaction = ""
		self.path = ""
		self.filename =""
		self.settings = {}
		
	def xmlStart(self, name, attrs):
		globalsettings['path.proj'] = self.path
		if name == "default":
			self.defaultaction = attrs.get("action", "")
		else:
			return CObject.xmlStart(self, name, attrs)

	def Print(self):
		print "Project '"+self.name+"'"
		print "\tDefault Action: '"+self.defaultaction+"'"
		print "\tObjects:"

		for object in self.objects:
			print "\t\t" + object.name + " ("+object.tag+")"
	def Perform(self, name, action):
		for object in self.objects:
			if object.name == name:
				return object.Perform(action)
	def BuildWorkspace(self, workspace):
		pass

##
##
##########################
class XMLHandler(xml.sax.handler.ContentHandler):
	def __init__(self):
		self.indent = 0
		self.handlerlist = []
		self.proj = CProject()
		self.ignore = 0

	def Handler(self):
		return self.handlerlist[len(self.handlerlist)-1]

	def characters(self, content):
		self.Handler().xmlChar(content)

	def startElement(self, name, attrs):
		newhandler = None
		self.ignore = 0
		CObject.xmlinputchar = ""
		if attrs.get('condition'):
			list = string.split(attrs.get('condition'), "=")
			if globalsettings[list[0]] != list[1]:
				self.ignore = 1
				return

		if name == "project":
			self.handlerlist.append(self.proj)
			self.proj.path = os.path.dirname(self.filename)
			self.proj.filename = os.path.abspath(self.filename)
			self.proj.xmlStart(name, attrs)
		else:
			newhandler = self.Handler().xmlStart(name,attrs)

		if newhandler:
			self.handlerlist.append(newhandler)
			#self.Handler().xmlStart(name,attrs)

	def endElement(self, name):
		if self.ignore:
			return
 		if self.Handler().xmlEnd(name) == 10:
			self.handlerlist = self.handlerlist[:len(self.handlerlist)-1]

##
## does a project
##
def LoadProject(filename):
	parser = xml.sax.make_parser()
	handler = XMLHandler()
	handler.filename = filename
	parser.setContentHandler(handler)
	#try:
	parser.parse(open(filename, "r"), )
	#except:
	#	print "Unable to open project:", filename
		#sys.exit(1)
	return handler.proj

def LoadProjects(filename):
	def Load(projects, proj):
		subprojects = proj.FindObjectsByTag(CSubProject(None).tag, 1)
		for subproj in subprojects:
			subprojfilename = subproj.GetFilename()
			if not subprojfilename in projects:
				p = LoadProject(subprojfilename)
				projects[p.filename] = p
				Load(projects, p)

	projects = {}
	proj = LoadProject(filename)
	projects[proj.filename] = proj
	Load(projects, proj)
	return projects

def DoConfig():
	try:
		f = open("config.xml")
		f.close
		print "Configurating"
		DoProject("config.xml", Action_Build)
	except:
		pass

def DoGlobal():
	try:
		globalproj = os.path.dirname(thisscript)+os.sep+"global.xml"
		#print globalproj
		f = open(globalproj)
		f.close
		DoProject(globalproj, Action_Build)
	except:
		pass

def DoProject(filename, action):
	filename = os.path.abspath(filename)
	if filename in doneprojects:
		return doneprojects[filename]

	proj = LoadProject(filename)

	result, time = proj.Perform(proj.defaultaction, action)
	doneprojects[filename] = [result, time]
	return [result, time]

def GetNode(current, parts):
	if len(parts) == 0:
		return current
	if len(parts[0]) == 0:
		return current
	for node in current.nodes:
		if node.name == parts[0]:
			return GetNode(node, parts[1:])

	newnode = CWorkspaceNode.CNode()
	newnode.name = parts[0]
	current.nodes.append(newnode)

	return GetNode(newnode, parts[1:])

def MakeProjectWorkspace(filename):
	workspace = CWorkspaceNode()
	projects = LoadProjects(filename)
	workspace.filename = os.path.abspath(filename)

	for key in projects:
		proj = projects[key]
		proj.Dump()
		if proj.filename.find(filename) != -1:
			workspace.name = proj.name
			break

	for key in projects:
		proj = projects[key]

		objs = proj.FindObjectsByTag(CWorkspace(None).tag)
		if len(objs):
			wsobj = objs[0]
			node = GetNode(workspace.root, wsobj.base.split('/'))
			wsobj.BuildWorkspace(node)
		else:
			pass # no workspace in this file
	workspace.root.Sort()
	return workspace


# Get host
host = "unknown"
if os.name == "nt":
	globalsettings['system.platform'] = 'win32'
	globalsettings['system.cpu'] = 'x86'
elif os.name == "posix":
	if sys.platform == 'linux2':
		globalsettings['system.platform'] = 'linux'
	elif sys.platform == 'freebsd5':
		globalsettings['system.platform'] = 'freebsd'
	elif sys.platform == 'openbsd3':
		globalsettings['system.platform'] = 'openbsd'
	else:
		globalsettings['system.platform'] = sys.platform

	uname = os.popen("uname -mp")
	cpu = uname.readline()
	cpu = cpu[0:len(cpu)-1]
	uname.close()
	
	globalsettings['system.cpu'] = GetCPU(cpu)

# set some inital settings
globalsettings['verbose'] = '0'
globalsettings['cxx.compiler'] = 'unknown'
globalsettings['cxx.speedhack'] = '0'
globalsettings['conf.name'] = 'debug'
globalsettings['linker.debuginfo'] = 'true'
globalsettings['cxx.target'] = globalsettings['system.cpu']+'-'+globalsettings['system.platform']

globalsettings['path.work'] = currentdir

## let all objects set there settings
for obj in objects:
	(objects[obj](None)).Init()

## get settings from the command line
action = Action_Build
for arg in sys.argv[2:]: # 0 =exe 1 = project, 2=
	list = string.split(arg, "=")
	if len(list) > 1:
		globalsettings[list[0]] = list[1]
	else:
		if arg == "clean":
			action = Action_Clean
		elif arg == "build":
			action = Action_Build
		elif arg == "rebuild":
			action = Action_Rebuild
		elif arg == "export":
			action = Action_Export
		elif arg == "nothing":
			action = Action_Nothing
		elif arg == "dump":
			action = Action_Dump
		else:
			print "Unknown action '"+arg+"'"
			sys.exit(0)

##
os.environ['PYTHONPATH'] = os.path.dirname(thisscript)

## print some important settings
print globalsettings['system.cpu']+"-"+globalsettings['system.platform']+",",
print globalsettings['conf.name']
print ""
import time, fpformat

#print ProcessVars(sys.argv[1])
#sys.exit(0)
DoGlobal()
DoConfig()

starttime = time.mktime(time.localtime())
if len(sys.argv) > 1:
	## make it happen
	if action == Action_Rebuild:
		DoProject(sys.argv[1], Action_Clean)
		doneprojects = {}
		DoProject(sys.argv[1], Action_Build)
	elif action == Action_Dump:
		DoProject(sys.argv[1], Action_Nothing)
	elif action == Action_Export:
		## Add some kind of option for thiss
		if globalsettings['system.platform'] == 'darwin':
			print "exporting to xcode"
			import export_xcode
			export_xcode.export(MakeProjectWorkspace(sys.argv[1]))
		elif globalsettings['system.platform'] == 'win32':
			print "exporting to msvc7.1"
			import export_msvc7
			export_msvc7.export(MakeProjectWorkspace(sys.argv[1]))
		else:
			print "exporter not found"
	elif action == Action_Nothing:
		print "Doing jack shit"
	else:
		DoProject(sys.argv[1], action)

if len(sys.argv) <= 1 or action == Action_Dump:
	# diag
	if len(sys.argv) <= 1:
		print "No project supplied, dumping info."
	print "----------------------------------"
	print "Global settings"
	print "----------------------------------"
	for key in globalsettings:
		print key+":", globalsettings[key] 
	
	print ""
	print "----------------------------------"
	print "Objects"
	print "----------------------------------"
	for obj in objects:
		i = objects[obj](None)
		print i.tag #, "-"*(30-len(i.tag))
		i.Dump()

buildtime = time.mktime(time.localtime()) - starttime

print "\nBuild Time: " + str(buildtime) + " s\n"
