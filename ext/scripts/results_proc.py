import sys
from os import listdir
from os.path import isfile, join
from collections import deque
import math

# Driver Code
if __name__ == "__main__":
    stats_dir = sys.argv[1]
    fanin = sys.argv[2]

    workloads = ["LeNet", "AlexNet", "VGG16", "HeartClass", "DigitRecogMLP", "EdgeDet", "ImgSmooth", "HeartEstm", "VisualPursuit", "DigitRecogSTDP"]
    workloads_real = ["LeNet", "AlexNet", "VGG16", "HeartClass", "MLP-MNIST", "EdgeDet", "ImgSmooth", "HeartEstm", "VisualPursuit", "RecurrentDigitRecog"]
    techs = ["random", "min-clusters", "min-comm"]

    total_c_file = join(fanin,"total_clusters.csv")
    total_c_file_d = open(total_c_file, 'w')

    avg_spikes_file = join(fanin,"avg_inter_spikes.csv")
    avg_spikes_file_d = open(avg_spikes_file, 'w')

    avg_conns_file = join(fanin,"avg_connections.csv")
    avg_conns_file_d = open(avg_conns_file, 'w')

    avg_util_file = join(fanin,"avg_cluster_util.csv")
    avg_util_file_d = open(avg_util_file, 'w')

    for i in range(len(workloads)):
        total_c = [workloads[i]]
        avg_spikes = [workloads[i]]
        avg_conns = [workloads[i]]
        avg_util = [workloads[i]]
 
        for j in range(len(techs)):
            stat_file = "out/" + workloads_real[i] + "." + techs[j] + "." + fanin + ".stats.txt"

            num_clusters = 0
            num_conns = 0
            num_spikes = 0
            num_utils = 0
            with open(stat_file) as fp:
                line = fp.readline()
                while line:
                    tokens = line.strip().split()
                    num_clusters = num_clusters + 1
                    num_inputs = int(tokens[1])
                    num_outputs = int(tokens[2])
                    #if num_inputs == num_outputs and num_inputs == int(tokens[3]) and techs != "min-clusters":
                     #   num_utils = num_utils
                        #print(line)
                    #else:
                    num_utils = num_utils + int(tokens[3])
                    num_conns = num_conns + int(tokens[4])
                    num_spikes = num_spikes + int(tokens[5])

                    line = fp.readline()
            total_c.append(str(num_clusters))
            avg_spikes.append(str(math.ceil(num_spikes/num_clusters)))
            avg_conns.append(str(math.ceil(num_conns/num_clusters)))
            avg_util.append(str(math.ceil(num_utils/num_clusters)))

        total_c_file_d.write(",".join(total_c) + "\n")
        avg_spikes_file_d.write(",".join(avg_spikes) + "\n")
        avg_conns_file_d.write(",".join(avg_conns) + "\n")
        avg_util_file_d.write(",".join(avg_util) + "\n")

    total_c_file_d.close()
    avg_spikes_file_d.close()
    avg_conns_file_d.close()
    avg_util_file_d.close()
