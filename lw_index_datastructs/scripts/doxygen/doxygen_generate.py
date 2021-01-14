#!/usr/bin/env python
import subprocess, sys, os, shutil

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "./../../"))

import config_build

#================================================================================================================================
# Append path to hhc.exe
if os.name == 'nt' or sys.platform == 'win32': 
    print("Current OS is: ", "Windows")
    os.environ["PATH"] += os.pathsep + '''D:/bins/doxygen/bin'''
    os.environ["PATH"] += os.pathsep + '''C:/Program Files (x86)/HTML Help Workshop'''
    os.environ["HHC_APP"] = '''C:/Program Files (x86)/HTML Help Workshop/hhc.exe'''
    os.environ["DOT_APP_PATH"] = '''C:/Program Files (x86)/Graphviz2.38/bin'''
    os.environ["USE_DOT"] = 'YES' # Setup USE_DOT. If you want faster build don't use it
    os.environ["LW_PROJECT_NAME"] = config_build.project_name_windows
elif os.name == 'posix' or sys.platform.startswith('linux'):
    print("Current OS is: ", "Linux")
    os.environ["LW_PROJECT_NAME"] = config_build.project_name_linux
else:
    print("UNDEFINED OS")
    sys.exit(-1)

# Make directory where output documents will be stored
if not os.path.exists("./generated_docs/html"): 
    os.makedirs("./generated_docs/html")
#================================================================================================================================
os.environ["MY_INPUT_DIRECTORIES"] = "./../../headers_public ./../../headers_internal ./../../src"

ret = subprocess.call('doxygen library_documentation.conf', shell=True)   
print("Completed [OK]: Final generated single chm file is here: ", "./generated_docs/library_documentation.chm")
