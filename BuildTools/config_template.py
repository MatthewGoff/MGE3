def vulkan_sdk(): return "C:\\VulkanSDK\\1.2.162.1"

def debug(): return True

'''
A list of directories and files which should be compiled. Recursive; Only considers (.cpp); Duplicates discarded.
'''
def source():

    source = []
    source += ["src"]
    #source += ["src\\WindowsOS\\Rendering"]
    
    return source

def ignore_warnings():
    warnings = []

    #warnings += ["4533"] # 4533 initialization of <var> is skipped by <goto>
    
    return warnings
    
def global_include():
    includes = []
    
    includes += ["Types.h"]
    includes += ["Config.h"]
    includes += ["Macros.h"]
    includes += ["Standard\\Standard.h"]
    includes += ["Memory.h"]
    includes += ["malloc.h"]
    includes += ["math.h"]
    
    return includes