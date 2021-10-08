import sys
import os
from os import listdir
from os.path import isfile, join
import subprocess

network_dir = sys.argv[1]
c_tech = sys.argv[2]
c_size = sys.argv[3]

###### PURPOSE OF SCRIPT ######################################
# Run ./ext on all connection files in a certain dir          #
###############################################################

if __name__ == "__main__":

    networks = [f.split('.')[0] for f in listdir(network_dir) if ".connection_info.txt" in f]
    #net_to_skip = ["VGG16", "LeNet", "denseNet8", "ResNet"]
    net_to_skip = []

    for network in networks:
        if network in net_to_skip:
            continue
        conn_file = join(network_dir, network) + ".connection_info.txt"
        spike_file = join(network_dir, network) + ".spike_info.txt"

        #if c_tech == "min-clusters":
        #    c_tech_print = "esl"
        #else:
        c_tech_print = c_tech

        ir_file = os.getcwd() + "/out/" + network + "." + c_tech_print + "." + c_size + ".ir.txt"
        stat_file = os.getcwd() + "/out/" + network + "." + c_tech_print + "." + c_size + ".cluster_info.txt"
        conn_stat_file = os.getcwd() + "/out/" + network + "." + c_tech_print + "." + c_size + ".cluster_conn_stat.txt"

        # print(ir_file)

        print (network)
        subprocess.call(["./ext", "--conn-file", conn_file, "--spike-file", spike_file,
                         "--unroll-fanin", "2", "--cluster-crossbar-size", c_size, 
                         "--clustering-algo", c_tech, "--cluster-stats", stat_file, 
                         "--cluster-ir-out", ir_file, "--cluster-conn-stats",  conn_stat_file
                         ])
        print(" ".join(["./ext", "--conn-file", conn_file, "--spike-file", spike_file,
                         "--unroll-fanin", "2", "--cluster-crossbar-size", c_size, 
                         "--clustering-algo", c_tech, "--cluster-stats", stat_file, 
                         "--cluster-ir-out", ir_file, "--cluster-conn-stats",  conn_stat_file
                         ]))
        print("")
