
func_base = "func"
var_base = "var"

numlines = 200
numfuncs = 500

def genFunc():
    stress_test = open("stress_test.nro",'w')
    for i in range(numfuncs):
        stress_test.write('fn '+func_base+str(i)+'() : int {\n')
        for j in range(numlines):
            stress_test.write(var_base+str(j)+' : int = (5*4+3-2)+1; \n')
        stress_test.write("return 0;\n")
        stress_test.write('}')
        stress_test.write("\n\n")

    stress_test.close()


genFunc()
