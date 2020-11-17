import sys
from os import listdir
from os.path import isfile, join
import subprocess

files_dir = sys.argv[1]
new_dir = sys.argv[2]
old_pattern = sys.argv[3]

if __name__ == "__main__":

    if len(sys.argv) == 4:
        new_pattern = sys.argv[3]
    else:
        new_pattern = sys.argv[4]

    files = [f for f in listdir(files_dir)]

    for f in files:
        if old_pattern in f and new_pattern != old_pattern:
            old_name = join(files_dir, f)
            new_name = join(new_dir, f.replace(old_pattern, new_pattern))
        elif old_pattern in f and new_pattern == old_pattern:
            old_name = join(files_dir, f)
            new_name = join(new_dir, f.replace(old_pattern, ''))
        else:
            old_name = join(files_dir, f)
            new_name = join(new_dir, f)

        subprocess.call(["cp", old_name, new_name])
