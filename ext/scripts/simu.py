import sys
from os import listdir
from os.path import isfile, join
import subprocess

network_dir = sys.argv[1]
c_tech = sys.argv[2]
c_size = sys.argv[3]

if __name__ == "__main__":

    networks = [f.split('.')[0] for f in listdir(network_dir) if ".connection_info.txt" in f]

    for network in networks:
        conn_file = join(network_dir, network) + ".connection_info.txt"
        spike_file = join(network_dir, network) + ".spike_info.txt"

        ir_file = "out/" + network + "." + c_tech + ".ir.txt"
        stat_file = "out/" + network + "." + c_tech + ".stats.txt"

        subprocess.call(["./ext", "--conn-file", conn_file, "--spike-file", spike_file, "--unroll-fanin", "2", "--cluster-crossbar-size", c_size, "--clustering-algo", c_tech, "--cluster-stats", stat_file, "--cluster-ir-out", ir_file])
