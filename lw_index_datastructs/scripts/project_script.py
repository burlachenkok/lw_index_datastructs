#!/usr/bin/env python3
import subprocess, sys, time, os, shutil

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "./../"))

import config_build

def revision():
    return subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip()

def branch():
    return subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"]).strip()

def dateOfLastRevision():
    return subprocess.check_output(["git", "log", "-n1", "--date=short", "--pretty=format:%cd"]).replace("-", "_").strip()

def gitSalt():
    return dateOfLastRevision() + "_REV" + revision() + "_" + branch() + "_TS" + str(int(time.time()))

def safeCall(cmdline):
    print("Run command: ", cmdline)
    ret = subprocess.call(cmdline, shell=True)
    print("RETURN CODE: ", ret, " (FROM LAUCNHING ", cmdline, ")")
    if ret !=0: sys.exit(ret)

def runCmake(binary_tree, source_tree, cmake_cmdline):
    binary_tree = os.path.abspath(binary_tree)
    source_tree = os.path.abspath(source_tree)
    if not os.path.isdir(binary_tree):
        os.makedirs(binary_tree)
    currDir = os.getcwd()
    os.chdir(binary_tree)
    toRun = "%s %s %s" % ('cmake', cmake_cmdline, source_tree)
    print("Run command: ", toRun)
    ret = subprocess.call(toRun, shell = True)
    print("Return code from launching 'cmake':", ret)
    os.chdir(currDir)

def removeFolder(folder):
    if os.path.isdir(folder):
        shutil.rmtree(folder)
        print("Folder '", folder, "' cleaning completed...")
    else:
        print("Folder '", folder, "' does not exist...")

if __name__ == "__main__":
    print("**********************************************************")
    print("Path to python interpretator:", sys.executable)
    print("Version:", sys.version)
    print("Platform name:", sys.platform)
    print("**********************************************************")

    showHelp       = False
    generate       = False
    buildDebug     = False
    buildRelease   = False
    cleanUp        = False
    launchTests    = False

    for i in range(0, len(sys.argv)):
        if sys.argv[i] == "-help" or sys.argv[i] == "--help" or sys.argv[i] == "-h" or len(sys.argv) == 1:
            print("Python version: ", sys.version)
            print('''------------------------------------------------------------------------------------------------------------------
This is advanced create project which give you a way for various task w.r.t. t1sdk

Use cases:                                                                                                        |
1. advanced_create_projects.py -help          | -h       -- to print this information text into console           |
2. advanced_create_projects.py -generate      | -g       -- to generate project files (setup X86_BUILD if needed) |
3. advanced_create_projects.py -build_debug   | -d       -- build debug version                                   |
4. advanced_create_projects.py -build_release | -r       -- build debug version                                   |
5. advanced_create_projects.py -clean         | -c       -- cleanup folder with build                             |
6. advanced_create_projects.py -tests         | -t       -- launch unittests for library                          |
------------------------------------------------------------------------------------------------------------------
''')
            sys.exit(0)
        elif sys.argv[i] == "-generate" or sys.argv[i] == "-g":
            generate = True
            print("+GENERATE")
        elif sys.argv[i] == "-build_debug" or sys.argv[i] == "-d":
            buildDebug = True
            print("+BUILD_DEBUG")
        elif sys.argv[i] == "-build_release" or sys.argv[i] == "-r":
            buildRelease   = True
            print("+BUILD_RELEASE")
        elif sys.argv[i] == "-clean" or sys.argv[i] == "-c":
            cleanUp        = True
            print("+CLEAN")
        elif sys.argv[i] == "-tests" or sys.argv[i] == "-t":
            launchTests   = True
            print("+LAUNCH_UNIT_TESTS")

    print("----------------------------------------------------------------------")

    t0 = time.time()
    if os.name == 'nt' or sys.platform == 'win32':
        print("Current OS is: ", "Windows")
        os.environ["PATH"] += os.pathsep + config_build.cmake_folder_windows + os.pathsep
        folder_with_binary = config_build.output_folder_windows
        project_generator  = config_build.project_gen_windows
        os.environ["LW_PROJECT_NAME"] = config_build.project_name_windows
    elif os.name == 'posix' or sys.platform.startswith('linux'):
        print("Current OS is: ", "Linux")
        os.environ["PATH"] += os.pathsep + config_build.cmake_folder_linux + os.pathsep
        folder_with_binary = config_build.output_folder_linux
        project_generator  = config_build.project_gen_linux
        os.environ["LW_PROJECT_NAME"] = config_build.project_name_linux
    else:
        print("UNDEFINED OS")
        sys.exit(-1)

    if 'extra_include_folders' in dir(config_build):   os.environ["LW_EXTRA_INCLUDE_FOLDERS"] = config_build.extra_include_folders
    if 'include_unit_tests' in dir(config_build):      os.environ["LW_INCLUDE_UNITTESTS"] = config_build.include_unit_tests
    if 'include_tools' in dir(config_build):           os.environ["LW_INCLUDE_TOOLS"] = config_build.include_tools

    os.environ["LW_PROJECT_SCRIPT_IS_USED"] = "1"

    if os.name == 'nt' or sys.platform == 'win32':
        extra_build_args = ""
    else:
        extra_build_args = " -- -j`nproc`"

    extra_cmake_arguments = ""

    if cleanUp:
        removeFolder(folder_with_binary)
    if generate:
        runCmake(folder_with_binary, "./../", '-G ' + '"' + project_generator + '"' + extra_cmake_arguments)
    if buildRelease:
        buildCmd = 'cmake --build "%s" --clean-first --config Release' % folder_with_binary + extra_build_args
        print(safeCall(buildCmd))
        print("Output release binaries are available in: ", folder_with_binary)
    if buildDebug:
        buildCmd = 'cmake --build "%s" --clean-first --config Debug' % folder_with_binary + extra_build_args
        print(safeCall(buildCmd))
        print("Output debug binaries are available in: ", folder_with_binary)
    if launchTests:
        if os.name == 'nt' or sys.platform == 'win32':
            launchCmd = '"' + config_build.output_folder_windows + "/tests/Release/" + os.environ["LW_PROJECT_NAME"] + "_unittest" + '"'
        else:
            launchCmd = '"' + config_build.output_folder_linux + "/tests/" + os.environ["LW_PROJECT_NAME"] + "_unittest" + '"'
        print(safeCall(launchCmd))

    print("Completed in: ", str(time.time() - t0), " seconds")
    sys.exit(0)
