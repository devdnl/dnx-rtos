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

def dir_arrayname(filepath):
    return 'dir_' + re.sub('[,.:/@+-\\\\*\(\)\[\]!#=]', '', underscore(filepath))

def file2carray(src_path, pointdir, filename):

    print("file2carray() =>", src_path, pointdir, filename)

    outfile = os.path.join(dest_dir, pointdir, filename) + '.c'

    filebytes = open(src_path, "rb").read()

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("#include <stdint.h>\n\n")

    name = file_arrayname(outfile)
    fout.write("const uint8_t " + name + "["+ str(len(filebytes)) +"] = {\n    ")

    ctr = 0
    for byte in filebytes:
        fout.write('0x' + binascii.hexlify(byte) + ', ')
        ctr = ctr + 1
        if ctr >= 16:
            fout.write('\n    ')
            ctr = 0

    fout.write("\n};\n")
    fout.close()

def dir2carray(dirname, subdirs, files):
    print("dir2carray() =>", dirname, subdirs, files)

    path = os.path.join(dest_dir, dirname)

    os.mkdir(path)

    outfile = os.path.join(path, os.path.basename(dirname) + ".c")

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("#include <stdint.h>\n")
    fout.write('#include "romfs_types.h"\n\n')

    for subdir in subdirs:
        name = dir_arrayname(os.path.join(path, subdir + "/" + subdir + "c"))
        fout.write("extern const romfs_dir_t " + name + ";\n")

    for file in files:
        name = file_arrayname(os.path.join(path, file + "c"))
        fout.write("extern const uint8_t " + name + "[];\n")

    fout.write("\n")
    fout.write("const romfs_dir_t " + dir_arrayname(outfile) + " = {\n")
    fout.write("    .items = " + str(len(subdirs) + len(files)) + ",\n")

    entry = 0

    for subdir in subdirs:
        name = dir_arrayname(os.path.join(path, subdir + "/" + subdir + "c"))
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__DIR, 0, " + name + ', "' + subdir + '"},\n')
        entry = entry + 1

    for file in files:
        name = file_arrayname(os.path.join(path, file + "c"))
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__FILE, sizeof(" + name + "), " + name + ', "' + file + '"},\n')
        entry = entry + 1

    fout.write("};\n")
    fout.close()


# print('walk_dir (absolute) = ' + os.path.abspath(walk_dir))
for root, subdirs, files in os.walk(walk_dir):
    print('--\nroot = ' + str.replace(".", "", root))

    pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

    if not pointdir:
        pointdir = "root"
    else:
        pointdir = re.sub("^/", "root/", pointdir)

    print(pointdir)

    dir2carray(pointdir, subdirs, files)

    for subdir in subdirs:
            print('\t- subdirectory ' + subdir)

    for filename in files:
            src_path = os.path.join(root, filename)
            #dest_path = os.path.join(dest_path_root, filename)

            print('\t- file %s (full path: %s)' % (filename, src_path))

            file2carray(src_path, pointdir, filename)

