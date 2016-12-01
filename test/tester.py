from os import listdir, remove
import subprocess
import filecmp
import sys

conffiles = [f for f in listdir(".") if ".conf" in f]

neuroc_path = "../Neuro"

count = len(conffiles)
success = 0

for fname in conffiles:
    f = open(fname);
    src = ""
    res = ""
    for l in f:
        if "src=" in l:
            src = l[4:].rstrip('\n')
        elif "result=" in l:
            res = l[7:].rstrip('\n')
    
    if src == "" or res == "":
        continue

    command = [neuroc_path,src]
    p = subprocess.run(command,stdout=subprocess.PIPE)
    if p.returncode != 0:
        print("Error compiling "+src)
        continue

    outfile = src+".ll"
    try:
        same = filecmp.cmp(outfile, res, False)

        if same:
            success += 1
            remove(outfile)
        else:
            print("Test failed for "+src)
    except:
        print(sys.exc_info()[0])

resultheader = "##########"
resultbody = "Test results: "+str(success)+"/"+str(count)+" tests passed"

print(resultheader)
print(resultbody)
print(resultheader)
