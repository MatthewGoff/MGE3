import os

output = ""

## General ##
output += "-c" + "\n" # Do not link
output += "-Fobuild\\obj\\" + "\n" # Location of object files (.obj)
#output += "-std:c++17" + "\n" # Use c++ 17 standard

## Debug ##
if (True):
    output += "-Fdbuild\\" + "\n" # Location of (intermediate) .pdb
                                  # ("vc140.pdb")
    output += "-Zi" + "\n" # Generates debug info used by Visual Studio

## Warning suppression ##
#output += "-wd4533" + "\n" # 4533 initialization of <var> is skipped by <goto>

## Header locations ##
output += "-I src\\" + "\n"
output += "-I C:\\GLM\\glm\\" + "\n"
output += "-I C:\\VulkanSDK\\1.2.162.1\\Include\\vulkan" + "\n"

## Always include ##
output += "-FICommon.h" + "\n"
output += "-FIConfig.h" + "\n"
output += "-FIStandard\\Standard.h" + "\n"
output += "-FIMemory.h" + "\n"
output += "-FImalloc.h" + "\n"
output += "-FImath.h" + "\n"
output += "-FILogging.h" + "\n"

## Input files ##
for root, dirs, files in os.walk("src"):
    for file in files:
        if (file[-4:] != ".cpp"): continue
        output += os.path.join(root, file) + "\n"

file = open("BuildTools\\compile_command.txt", "w")
file.write(output)
file.close()