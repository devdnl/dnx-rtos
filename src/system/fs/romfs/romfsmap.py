import os
import sys
import binascii
import array
import re
import hashlib

walk_dir = sys.argv[1]
dest_dir = sys.argv[2]

file_dict = {}

def underscore(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def file_arrayname(filepath):
    return 'file_' + re.sub('[,.:/@+-\\\\*\(\)\[\]!#=]', '', underscore(filepath))


def dir_arrayname(filepath):
    return 'dir_' + re.sub('[,.:/@+-\\\\*\(\)\[\]!#=]', '', underscore(filepath))


def file2carray(src_path, pointdir, filename):

    global file_dict

    filecontent = open(src_path, "rb").read()

    hash_object = hashlib.sha1(filecontent)
    hash_name   = hash_object.hexdigest()

    outfile = os.path.join(dest_dir, hash_name) + '.c'

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("// source file: " + src_path + '\n')
    fout.write("#include <stddef.h>\n")
    fout.write("#include <stdint.h>\n\n")

    fout.write("const uint8_t file_" + hash_name + "["+ str(len(filecontent)) +"] = {\n    ")

    ctr = 0
    for byte in filecontent:
        fout.write('0x' + binascii.hexlify(byte) + ', ')
        ctr = ctr + 1
        if ctr >= 16:
            fout.write('\n    ')
            ctr = 0

    fout.write("\n};\n\n")

    fout.write("const size_t file_" + hash_name + "_size = " + str(len(filecontent)) + ";\n")

    fout.close()

    with open(os.path.join(dest_dir, "Makefile.in"), "a") as mk:
        mk.write("CSRC_CORE += fs/romfs/" + outfile + '\n')

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

    # create file
    outfile = os.path.join(dest_dir, file_dict[dirname]) + '.c'

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("// source dir: " + walk_dir + dirname + '\n')
    fout.write("#include <stdint.h>\n")
    fout.write("#include <stddef.h>\n")
    fout.write('#include "romfs_types.h"\n\n')

    for subdir in subdirs:
        fout.write("extern const romfs_dir_t dir_" + file_dict[dirname + '/' + subdir] + ";\n")

    for file in files:
        name = file_dict[walk_dir + dirname + '/' + file]
        fout.write("extern const uint8_t file_" + name + "[];\n")
        fout.write("extern const size_t file_" + name + "_size;\n")

    fout.write("\n")
    fout.write("const romfs_dir_t dir_" + file_dict[dirname] + " = {\n")
    fout.write("    .items = " + str(len(subdirs) + len(files)) + ",\n")

    entry = 0

    for subdir in subdirs:
        name = file_dict[dirname + '/' + subdir]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__DIR, NULL, &dir_" + name + ', "' + subdir + '"},\n')
        entry = entry + 1

    for file in files:
        name = file_dict[walk_dir + dirname + '/' + file]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__FILE, &file_" + name + "_size, &file_" + name + ', "' + file + '"},\n')
        entry = entry + 1

    fout.write("};\n")
    fout.close()

    with open(os.path.join(dest_dir, "Makefile.in"), "a") as mk:
        mk.write("CSRC_CORE += fs/romfs/" + outfile + '\n')


def root2c(subdirs, files):

    global file_dict
    global walk_dir

    dirname = "root"

    # create file
    outfile = os.path.join(dest_dir, dirname) + '.c'

    fout = open(outfile, "wb")
    fout.write("// file generated\n")
    fout.write("// source dir: " + walk_dir + dirname + '\n')
    fout.write("#include <stdint.h>\n")
    fout.write("#include <stddef.h>\n")
    fout.write('#include "romfs_types.h"\n\n')

    for subdir in subdirs:
        fout.write("extern const romfs_dir_t dir_" + file_dict['/' + subdir] + ";\n")

    for file in files:
        name = file_dict[walk_dir + '/' + file]
        fout.write("extern const uint8_t file_" + name + "[];\n")
        fout.write("extern const size_t file_" + name + "_size;\n")

    fout.write("\n")
    fout.write("const romfs_dir_t dir_" + dirname + " = {\n")
    fout.write("    .items = " + str(len(subdirs) + len(files)) + ",\n")

    entry = 0

    for subdir in subdirs:
        name = file_dict['/' + subdir]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__DIR, NULL, &dir_" + name + ', "' + subdir + '"},\n')
        entry = entry + 1

    for file in files:
        name = file_dict[walk_dir + '/' + file]
        fout.write("    .entry[" + str(entry) + "] = {ROMFS_FILE_TYPE__FILE, &file_" + name + "_size, &file_" + name + ', "' + file + '"},\n')
        entry = entry + 1

    fout.write("};\n")
    fout.close()

    with open(os.path.join(dest_dir, "Makefile.in"), "a") as mk:
        mk.write("CSRC_CORE += fs/romfs/" + outfile + '\n')


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
    else:
        file_dict['root'] = "/"


# create file tree
for root, subdirs, files in os.walk(walk_dir):
    pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

    if pointdir:
        dir2c(pointdir, subdirs, files)


# create root dir
for root, subdirs, files in os.walk(walk_dir):
    pointdir = os.path.abspath(root).replace(os.path.abspath(walk_dir), "")

    if not pointdir:
        print("X")
        root2c(subdirs, files)
    else:
        break


# print hashes FIXME
for key, val in file_dict.iteritems():
    print(key, val)
