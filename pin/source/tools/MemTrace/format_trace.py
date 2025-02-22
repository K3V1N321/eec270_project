fileName = "graph2.trace"

lines = []
with open(fileName, "r") as file:
    lineNumber = 0
    startCycle = None
    for line in file:
        elements = line.strip().split(" ")
        if lineNumber == 0:
            startCycle = int(elements[2])
            elements[2] = "0"
        else:
            elements[2] = str(int(elements[2]) - startCycle)

        lines.append(" ".join(elements))
        lineNumber += 1
    
outputFileName = "graph2_formatted.trace"
with open(outputFileName, "w") as file:
    for line in lines:
        file.write(line + "\n")

