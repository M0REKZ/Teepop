import os, sys

buildscript = os.path.abspath(sys.argv[0])

currentid = 1000
def export(workspace):
	def GetID():
		global currentid
		currentid += 1
		return currentid
	def WriteNode(proj, thisnode):
		if len(thisnode.nodes): # group node
			id = GetID()
			idlist = []
			for node in thisnode.nodes:
				idlist += WriteNode(proj, node)
			
			proj.write("\t\t"+str(id)+' = {'+"\n")
			proj.write("\t\t\t"+'children = ('+"\n")
			for cid in idlist:
				proj.write("\t\t\t\t"+str(cid)+",\n")
			
			proj.write("\t\t\t"+');'+"\n")
			proj.write("\t\t\t"+'isa = PBXGroup;'+"\n")
			if thisnode.name != '#ROOT#':
				proj.write("\t\t\t"+'name = '+thisnode.name+';'+"\n")
			proj.write("\t\t\t"+'refType = 4;'+"\n")
			proj.write("\t\t\t"+'sourceTree = "<group>";'+"\n")
			proj.write("\t\t"+'};'+"\n")
			return [id]
		else:
			id = GetID()
			proj.write("\t\t"+str(id)+' = {'+"\n")
			proj.write("\t\t\t"+'expectedFileType = sourcecode.c.h;'+"\n")
			proj.write("\t\t\t"+'fileEncoding = 30;'+"\n")
			proj.write("\t\t\t"+'isa = PBXFileReference;'+"\n")
			proj.write("\t\t\t"+'name = '+os.path.basename(thisnode.name)+';'+"\n")
			proj.write("\t\t\t"+'path = '+thisnode.name+';'+"\n")
			proj.write("\t\t\t"+'refType = 0;'+"\n")
			proj.write("\t\t\t"+'sourceTree = "<absolute>";'+"\n")
			proj.write("\t\t"+'};'+"\n")
			return [id]
	def WriteTarget(proj, command, name):
		id = GetID()
		proj.write("\t\t"+str(id)+' = {' + "\n")
		proj.write("\t\t\t"+'buildArgumentsString = "'+command+' $(ACTION) conf.name='+name+'";' + "\n")
		proj.write("\t\t\t"+'buildPhases = (' + "\n")
		proj.write("\t\t\t"+');' + "\n")
		proj.write("\t\t\t"+'buildSettings = {' + "\n")
		proj.write("\t\t\t"+'};' + "\n")
		proj.write("\t\t\t"+'buildToolPath = /usr/bin/python;' + "\n")
		proj.write("\t\t\t"+'buildWorkingDirectory = '+os.path.abspath('.')+';' + "\n")
		proj.write("\t\t\t"+'dependencies = (' + "\n")
		proj.write("\t\t\t"+');' + "\n")
		proj.write("\t\t\t"+'isa = PBXLegacyTarget;' + "\n")
		proj.write("\t\t\t"+'name = '+name+';' + "\n")
		proj.write("\t\t\t"+'passBuildSettingsInEnvironment = 1;' + "\n")
		proj.write("\t\t\t"+'productName = '+name+';' + "\n")
		proj.write("\t\t\t"+'settingsToExpand = 6;' + "\n")
		proj.write("\t\t\t"+'settingsToPassInEnvironment = 287;' + "\n")
		proj.write("\t\t\t"+'settingsToPassOnCommandLine = 280;' + "\n")
		proj.write("\t\t"+'};' + "\n")
		return id

	try: os.makedirs("project.xcode")
	except: pass
	
	proj = open("project.xcode/project.pbxproj", "w")
	proj.write('// !$*UTF8*$!'+"\n")
	proj.write('{'+"\n")
	proj.write("\t"+'archiveVersion = 1;'+"\n")
	proj.write("\t"+'classes = {'+"\n")
	proj.write("\t"+'};'+"\n")
	proj.write("\t"+'objectVersion = 39;'+"\n")
	proj.write("\t"+'objects = {'+"\n")
	
	# file objects
	maingroup = WriteNode(proj, workspace.root)

	# write targets
	start = buildscript+' '+workspace.filename+' '
	targetids = []
	targetids.append(WriteTarget(proj, start, "debug"))
	targetids.append(WriteTarget(proj, start, "release"))

	# root object
	rootid = GetID()
	proj.write("\t\t"+str(rootid) + ' = {' + "\n")
	proj.write("\t\t\t"+'buildSettings = {' + "\n")
	proj.write("\t\t\t"+'};' + "\n")
	proj.write("\t\t\t"+'buildStyles = (' + "\n")
	proj.write("\t\t\t"+');' + "\n")
	proj.write("\t\t\t"+'hasScannedForEncodings = 1;' + "\n")
	proj.write("\t\t\t"+'isa = PBXProject;' + "\n")
	proj.write("\t\t\t"+'mainGroup = '+str(maingroup[0])+';' + "\n")
	proj.write("\t\t\t"+'projectDirPath = "";' + "\n")
	proj.write("\t\t\t"+'targets = (' + "\n")
	for id in targetids:
		proj.write("\t\t\t\t" + str(id) + ',' + "\n")
	proj.write("\t\t\t"+');' + "\n")
	proj.write("\t\t"+'};' + "\n")


	proj.write("\t"+'};'+"\n")
	
	proj.write("\t"+'rootObject = '+str(rootid)+';'+"\n") # fix me
	
	proj.write('}'+"\n")
	proj.close()
