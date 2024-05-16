import numpy as np

path = "C:\\Users\\Dew\\Desktop\\Dew\\CBA\\resource"

namelist = ["OpenCyc", "Mondial", "YAGO", "LinkedMDB", "Dbpedia", "LUBM-50K", "LUBM-500K", "LUBM-5M"]
namelist = ["Dbpedia"]
edgelist = ["UW", "IW"]
algolist = ["core", "CBA+", "CBAf"]

#namelist = ["OpenCyc"]
#edgelist = ["UW"]
#algolist = ["core"]

tra = {"core": "\\textsc{Core}", "CBA+": "\\textsc{PrunedCBA}", "CBAf": "\\textsc{PrunedCBA+}"}

def operator_data(file_path, name, edge, algo):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    data = []
    for line in lines:
        if line.strip() == "Aver:":
            break

        values = [float(val)/1000 for val in line.strip().split()]
        data.append(values)

    data_array = np.array(data)

    x_mean = np.mean(data_array[:, 0])
    y_mean = np.mean(data_array[:, 1])
    z_mean = np.mean(data_array[:, 2])

    x_std = np.std(data_array[:, 0])
    y_std = np.std(data_array[:, 1])
    z_std = np.std(data_array[:, 2])

    x_mean= format(x_mean, '.3f')
    y_mean= format(y_mean, '.3f')
    z_mean= format(z_mean, '.3f')

    x_std = format(x_std, '.3f')
    y_std = format(y_std, '.3f')
    z_std = format(z_std, '.3f')

    strl, strr= "\(", ")\)"
    if algo == "CBAf":
        strl += "\\textbf{"
        strr = "}" + strr
    if algo == "core":
        print("\multirow{3}{*}{\\textup{" + name + "}}", end=' ')
    
    print("& " + tra[algo], end = " & ")
    print(strl + str(x_mean) + "(" + str(x_std) + strr, end = " & ")
    print(strl + str(y_mean) + "(" + str(y_std) + strr, end = " & ")
    print(strl + str(z_mean) + "(" + str(z_std) + strr, end = " \\\\ \n")

for edge in edgelist:
    print(edge + ":")
    for name in namelist:
        #print(name + ":")
        for algo in algolist:
            #print(algo + ":")
            file = path + "\\" + name + "\\" + edge + "_" + algo  + "\\" + "time_result_only.txt"
            operator_data(file, name, edge, algo)
            
        print("\midrule\n")

