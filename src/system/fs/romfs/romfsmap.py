import os
import sys
import binascii
import array
import re
import hashlib

try:
    walk_dir = sys.argv[1]
    dest_dir = sys.argv[2]
except:
    print("Usage: python romfsmap.py <source-dir> <output-dir>\n")
    exit(1)

file_dict  = {}
total_size = 0


def file2carray(src_path, pointdir, filename):

    global file_dict
    global total_size

    filecontent = open(src_path, "rb").read()
    total_size  = total_size + len(filecontent)

    hash_object = hashlib.sha1(filecontent)
    hash_name   = hash_object.hexdigest()

    outfile = os.path.join(dest_dir, hash_name) + '.c'

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("// source file: " + src_path + '\n')
    fout.write("#include <stddef.h>\n")
    fout.write("#include <stdint.h>\n\n")

    fout.write("const uint8_t romfsfile_" + hash_name + "["+ str(len(filecontent)) +"] = {\n    ")

    ctr = 0
    for byte in filecontent:
        fout.write('0x' + binascii.hexlify(byte) + ', ')
        ctr = ctr + 1
        if ctr >= 16:
            fout.write('\n    ')
            ctr = 0

    fout.write("\n};\n\n")

    fout.write("const size_t romfsfile_" + hash_name + "_size = " + str(len(filecontent)) + ";\n")

    fout.close()

    with open(os.path.join(dest_dir, "Makefile.in"), "ab") as mk:
        mk.write("               fs/romfs/" + outfile + '\\\n')

    file_dict[src_path] = hash_name


def hashdir(dirname, subdirs, files):

    global file_dict

    hash_object = hashlib.sha1(dirname)

    for subdir in subdirs:
        hash_object.update(subdir)

    for file in files:
        hash_object.update(file)

    hash_name = hash_object.hexdigest()

    file_dict[dirname] = hash_name


def dir2c(dirname, subdirs, files):

    global file_dict
    global walk_dir
    global total_size

    outfile = os.path.join(dest_dir, "root" if dirname == "/" else file_dict[dirname]) + '.c'

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("// source dir: " + walk_dir + dirname + '\n')
    fout.write("#include <stdint.h>\n")
    fout.write("#include <stddef.h>\n")
    fout.write('#include "romfs_types.h"\n\n')

    for subdir in subdirs:
        name = file_dict[(dirname if dirname != "/" else "") + '/' + subdir]
        fout.write("extern const romfs_dir_t romfsdir_" + name + ";\n")

    for file in files:
        name = file_dict[walk_dir + (dirname if dirname != "/" else "") + '/' + file]
        fout.write("extern const uint8_t romfsfile_" + name + "[];\n")
        fout.write("extern const size_t romfsfile_" + name + "_size;\n")

    fout.write("\n")

    name = "root" if dirname == "/" else file_dict[dirname]

    fout.write("const romfs_dir_t romfsdir_" + name + " = {\n")
    fout.write("    .items = " + str(len(subdirs) + len(files)) + ",\n")

    entry = 0

    for subdir in subdirs:
        name = file_dict[(dirname if dirname != "/" else "") + '/' + subdir]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__DIR, NULL, &romfsdir_"
                   + name + ', "' + subdir + '"},\n')
        entry = entry + 1

    for file in files:
        name = file_dict[walk_dir + (dirname if dirname != "/" else "") + '/' + file]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__FILE, &romfsfile_"
                   + name + "_size, &romfsfile_" + name + ', "' + file + '"},\n')
        entry = entry + 1

    fout.write("};\n\n")

    if dirname == "/":
        fout.write("const size_t romfs_total_size = " + str(total_size) + ';\n')
        fout.write("const size_t romfs_files = " + str(len(file_dict)) + ';\n')

    fout.close()

    with open(os.path.join(dest_dir, "Makefile.in"), "ab") as mk:
        mk.write("               fs/romfs/" + outfile + '\\\n')


def main():
    # prepare Makefile.in
    with open(os.path.join(dest_dir, "Makefile.in"), "wb") as mk:
        mk.write("CSRC_CORE += $(sort\\\n")


    # collect all files
    for root, subdirs, files in os.walk(walk_dir):
        pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

        for filename in files:
            src_path = os.path.join(root, filename)
            file2carray(src_path, pointdir, filename)


    # collect all dirs
    for root, subdirs, files in os.walk(walk_dir):
        pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

        if pointdir:
            hashdir(pointdir, subdirs, files)


    # create file tree
    for root, subdirs, files in os.walk(walk_dir):
        pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

        if pointdir:
            dir2c(pointdir, subdirs, files)


    # create root dir
    for root, subdirs, files in os.walk(walk_dir):
        dir2c("/", subdirs, files)
        break


    # prepare Makefile.in
    with open(os.path.join(dest_dir, "Makefile.in"), "ab") as mk:
        mk.write("              )\n")


    # DEBUG: print file hashes
    # for key, val in file_dict.iteritems(): print(key, val)

if __name__ == "__main__":
    main()
