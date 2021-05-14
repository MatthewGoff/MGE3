import os

def obj_path(src_path):
    return src_path.replace("src\\", "build\\obj\\").replace(".cpp", ".obj")

def source_files():
    
    search = []
    search += ["src"]

    source_files = []
    def add_file(file):
        nonlocal source_files
        if (file[-4:] != ".cpp"): return
        if file not in source_files: source_files += [file]

    for location in search:
        if "." in location:
            add_file(location)
        else:
            for root, dirs, files in os.walk(location):
                for file in files:
                    add_file(os.path.join(root, file))
    
    return source_files

def compile_command():
    command = "cl"

    ## General ##
    command += " -c" # Do not link
    command += " -nologo" # No logo
    
    #output += "-std:c++17" # Use c++ 17 standard

    ## Debug ##
    if (True):
        command += " -Fdbuild\\" # Location of (intermediate) .pdb ("vc140.pdb")
        command += " -Zi" # Generates debug info used by Visual Studio

    ## Dissable warnings ##
    #output += "-wd4533" # 4533 initialization of <var> is skipped by <goto>

    ## Header locations ##
    command += " -I src\\"
    command += " -I C:\\GLM\\glm\\"
    command += " -I C:\\VulkanSDK\\1.2.162.1\\Include\\vulkan"

    ## Always include ##
    command += " -FITypes.h"
    command += " -FIConfig.h"
    command += " -FIMacros.h"
    command += " -FIStandard\\Standard.h"
    command += " -FIMemory.h"
    command += " -FImalloc.h"
    command += " -FImath.h"

    return command

def link_command():    
    command = "link"

    ## General ##
    command += " -OUT:build\\main.exe" # Location of executable (.exe)
    command += " -nologo" # No logo

    ## Debug ##
    if (True):
        command += " -DEBUG"
        command += " -PDB:build\\main.pdb" # Location of .pdb

    ## Libraries ##
    command += " user32.lib" # Multiple OS services from <window.h>
    command += " gdi32.lib" # StretchDIBits() from <windows.h>
    command += " winmm.lib" # timeBeginPeriod() from <windows.h>
    command += " C:\\VulkanSDK\\1.2.162.1\\Lib\\vulkan-1.lib"

    ## obj files ##
    for root, dirs, files in os.walk("src"):
        for file in files:
            if (file[-4:] != ".cpp"): continue
            src_path = os.path.join(root, file)
            command += " " + obj_path(src_path)

    return command

def create_batch():
    output = "@echo off"

    output += "" + "\n"
    output += "if not exist build mkdir build" + "\n"
    output += "if not exist build\\AppData mkdir build\\AppData" + "\n"
    output += "if not exist build\obj mkdir build\\obj" + "\n"
    
    for root, dirs, files in os.walk("src"):
        for dir in dirs:
            dir_path = obj_path(os.path.join(root, dir))
            output += "if not exist {0} mkdir {0}\n".format(dir_path)
    
    for source_file in source_files():
        
        output += compile_command()
        output += " -Fo" + obj_path(source_file)
        output += " " + source_file
        output += "\n"
    
    output += link_command() + "\n"
    
    return output

def main():
    file = open("build\\_build.bat", "w")
    file.write(create_batch())
    file.close()

if __name__ == "__main__":
    main()