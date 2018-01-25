import os
import sys
import binascii
import array
import re

walk_dir = sys.argv[1]
dest_dir = sys.argv[2]

def underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def file_arrayname(filepath):
    return 'file_' + re.sub('[,.:/@+-\\\\*\(\)\[\]!#=]', '', underscore(filepath))

def file2carray(filepath, outfile):

    filebytes = open(filepath, "rb").read()

    fout = open(outfile + '.c', "wb")
    fout.write("// file generated\n")
    fout.write("#include <stdint.h>\n\n")
    fout.write("const uint8_t " + file_arrayname(filepath) + "["+ str(len(filebytes)) +"] = {\n    ")

    ctr = 0
    for byte in filebytes:
        fout.write('0x' + binascii.hexlify(byte) + ', ')
        ctr = ctr + 1
        if ctr >= 16:
            fout.write('\n    ')
            ctr = 0

    fout.write("\n};\n")
    fout.close()

def dir2carray(dirname, subdirs, files, outfile):
    print("=>", dirname, subdirs, files, outfile)

    outfile = os.path.join(outfile, dirname)

    if outfile == dest_dir + '/':
        outfile = os.path.join(outfile, "root")

    fout = open(outfile + '.c', "wb")
    fout.write("// file generated\n")
    fout.write("#include <stdint.h>\n")
    fout.write('#include "romfs_types.h"\n\n')
    fout.close()


# print('walk_dir (absolute) = ' + os.path.abspath(walk_dir))

for root, subdirs, files in os.walk(walk_dir):
    print('--\nroot = ' + root)

    dest_path_root = os.path.join(dest_dir, os.path.basename(root))

    try:
        os.mkdir(dest_path_root)
    except:
        pass

    dir2carray(os.path.basename(root), subdirs, files, dest_path_root)

    for subdir in subdirs:
            print('\t- subdirectory ' + subdir)

    for filename in files:
            file_path = os.path.join(root, filename)
            dest_path = os.path.join(dest_path_root, filename)

            print('\t- file %s (full path: %s)' % (filename, file_path))

            file2carray(file_path, dest_path)

