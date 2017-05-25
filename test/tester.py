from os import listdir, remove
import subprocess
import sys
import os
import imp
import difflib

neuroc_path = "../Neuro"

def diffOutput(expected, actual):
    diff = difflib.unified_diff(expected.splitlines(1), actual.splitlines(1))
    print(''.join(diff))

def loadConfFiles():
    ret = []
    for root, dirs, files in os.walk('.'):
        for f in  files:
            if len(f) < 5:
                continue
            if '.conf' in f[-5:]:
                importString = str(root).replace('/','.')
                testConf = imp.load_source(f,root+'/'+f)
                ret.append((str(root+'/'+f),testConf.conf))
    return ret

def runTests(tests):
    success = 0
    for tup in tests:
        t = tup[1]
        print("Test " + tup[0])
        srcs = ""
        compiler = None
        expected = None
        options = ""
        timeout = -1
        if 'src' in t:
            srcs = t['src']
        else:
            print('  Fail: Error, no source files provided')
            continue
        if 'compiler' in t:
            compiler = t['compiler']
        if 'expected' in t:
            expected = t['expected']
        if 'options' in t:
            options = t['options']
        if 'timeout' in t:
            timeout = int(t['timeout'])

        if compiler == None and expected == None:
            print('  Fail: Error, no results files provided')
            continue
        if compiler == '' and expected == '':
            print('  Fail: Error, no results files provided')
            continue


        #Compile program
        command = []
        if options != "":
            command = [neuroc_path,options,srcs]
        else:
            command = [neuroc_path,srcs]
        p = subprocess.run(command,stdout=subprocess.PIPE)
        if p.returncode != 0:
            print("  Fail: Error compiling "+srcs)
            continue
        #outs, errs = p.communicate(timeout=3)
        outs = p.stdout
        output = outs.decode('utf-8')
        matchedCompilerOutput = True
        matchedExpectedOutput = True

        if compiler != None:
            with open(compiler, 'r') as compilerOutFile:
                expectedCompilerOutput = compilerOutFile.read()
                if expectedCompilerOutput != output:
                    print("  Fail: mismatched compiler output")
                    matchedCompilerOutput = False
                    diffOutput(expectedCompilerOutput.splitlines(1), output.splitlines(1))

        #Run application
        if expected != None:
            exe = srcs.replace('.nro','.exe')
            command = ['./'+exe]

            #If application doesn't exist, exit
            if not os.path.exists('./'+exe):
                print("  Fail: Compilation failed")
                continue

            p2 = subprocess.run(command,stdout=subprocess.PIPE)
            if p2.returncode != 0:
                print("  Fail: Error running program")

            programOut = p2.stdout.decode('utf-8')
            with open(expected, 'r') as expectedOutFile:
                expectedOut = expectedOutFile.read()
                if programOut != expectedOut:
                    print("  Fail: mismatched program output")
                    matchedExpectedOutput = False
                    try:
                        diffOutput(expectedOut.splitlines(1), programOut.splitlines(1))
                    except:
                        pass
                else:
                    objFile = srcs+'.o'
                    os.remove('./'+objFile)
                    os.remove('./'+exe)

        #Check for success
        if matchedCompilerOutput and matchedExpectedOutput:
            success += 1
            print('  Pass')
    return success

# Run tests
tests = loadConfFiles()
success = runTests(tests)
total = len(tests)

resultheader = "##########"
resultbody = "Test results: "+str(success)+"/"+str(total)+" tests passed"

print('')
print(resultheader)
print(resultbody)
print(resultheader)
