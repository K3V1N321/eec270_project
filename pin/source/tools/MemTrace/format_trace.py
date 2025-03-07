import numpy as np
BATCH_SIZE = 10000000


if __name__ == "__main__":
    inputFileName = "graph.trace"
    outputFileName = "graph_bfs2.trace"
    
    lines = []
    with open(inputFileName, "r") as inputFile:
        with open(outputFileName, "w") as outputFile:
            batch = 0
            batchIndex = 0
            lineNumber = 0
            reads = 0
            writes = 0
            readLatencies = []
            writeLatencies = []
            averageReadLatency = None
            averageWriteLatency = None
            start = None
            originalStart = None
            prevLineType = None
            prevLineCycle = None
            for line in inputFile:
                elements = line.strip().split(" ")
                if elements[1] == "R":
                    elements[1] = "READ"
                elif elements[1] == "W":
                    elements[1] = "WRITE"
                    
                if batchIndex == 0:
                    originalStart = int(elements[2])
                    if batch == 0:
                        elements[2] = "0"
                        start = 0
                    else:
                        if prevLineType == "WRITE":
                            start = int(prevLineCycle + averageWriteLatency)
                        elif prevLineType == "READ":
                            start = int(prevLineCycle + averageReadLatency)
                        elements[2] = str(start)
                    
                    readLatencies.clear()
                    writeLatencies.clear()
                    averageReadLatency = None
                    averageWriteLatency = None
                else:
                    curLineCycle = (int(elements[2]) - originalStart) + start
                    latency = curLineCycle - prevLineCycle
                    if prevLineType == "WRITE":
                        writeLatencies.append(latency)
                    elif prevLineType == "READ":
                        readLatencies.append(latency)
                    elements[2] = str(curLineCycle)

                
                prevLineType = elements[1]
                prevLineCycle = int(elements[2])

                lines.append(" ".join(elements) + "\n")
                lineNumber += 1
                batchIndex += 1
                
                if batchIndex == BATCH_SIZE:
                    print("(Line, Batch)","("+ str(lineNumber) + "," + str(batchIndex) + "):", lines[-1], end = "")
                    averageReadLatency = np.round(np.mean(readLatencies))
                    print("READ latency:", averageReadLatency)
                    averageWriteLatency = np.round(np.mean(writeLatencies))
                    print("WRITE latency:", averageWriteLatency)
                    print()
                    
                    outputFile.writelines(lines)
                    lines.clear()
                    readLatencies.clear()
                    writeLatencies.clear()
                    batchIndex = 0
                    batch += 1
            
            if len(lines) > 0:
                print("Final Writes:", len(lines), batchIndex, lineNumber)
                outputFile.writelines(lines)
                print("Last Element:", lineNumber, lines[-1])
            else:
                print("Last Element:", lineNumber, prevLineType, prevLineCycle)

    # inputFileName = "graph_bfs_old.trace"
    # with open(inputFileName, "r") as inputFile:
    #     lineNumber = 0
    #     reads = 0
    #     writes = 0
    #     readLatencies = []
    #     writeLatencies = []
    #     averageReadLatency = None
    #     averageWriteLatency = None
    #     start = None
    #     prevLineType = None
    #     prevLineCycle = None
    #     for line in inputFile:
    #         elements = line.strip().split(" ")
    #         curLineCycle = int(elements[2])
    #         if lineNumber > 0:
    #             latency = curLineCycle - prevLineCycle
    #             if prevLineType == "WRITE":
    #                 writeLatencies.append(latency)
    #             elif prevLineType == "READ":
    #                 readLatencies.append(latency)
            
    #         prevLineType = elements[1]
    #         prevLineCycle = curLineCycle

    #         lineNumber += 1
            
    #         if lineNumber == 100000:
    #             print(lineNumber)
    #             print(elements)
    #             print("READ:")
    #             averageReadLatency = np.round(np.mean(readLatencies))
    #             print(averageReadLatency)
    #             print()
                
    #             print("WRITE:")
    #             averageWriteLatency = np.round(np.mean(writeLatencies))
    #             print(averageWriteLatency)
                
    #             break