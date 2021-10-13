import sys
import os
from os import listdir
from os.path import isfile, join
import subprocess

network_dir = sys.argv[1]

###### PURPOSE OF SCRIPT ######################################
# Run ./conn_depth on all connection files in a certain dir   #
###############################################################

if __name__ == "__main__":

    networks = [f.split('.')[0] for f in listdir(network_dir) if ".connection_info.txt" in f]
    #net_to_skip = ["VGG16", "LeNet", "denseNet8", "ResNet"] #heavy ones
    net_to_skip = ["VisualPursuit", "RecurrentDigitRecog", "HeartEstm"] # recurrent ones
    #cycles found in recurrent ones: [('2', '6'), ('6', '2')], [('400', '400')], [('7', '7')] (relatively)

    for network in networks:
        if network in net_to_skip:
            continue
        conn_file = join(network_dir, network) + ".connection_info.txt"

        ir_file = os.getcwd() + "/out/" + network + ".neudepth_ir.txt"
        conndepth_file = os.getcwd() + "/out/" + network + ".conndepth.txt"

        # print(ir_file)

        print (network)
        subprocess.call(["./conn_depth", "--conn-file", conn_file, 
                         "--neuron-depth-ir", ir_file, "--longterm-conn-stats",  conndepth_file
                         ])
        print(" ".join(["./conn_depth", "--conn-file", conn_file, 
                         "--neuron-depth-ir", ir_file, "--longterm-conn-stats",  conndepth_file
                         ]))
        print("")
