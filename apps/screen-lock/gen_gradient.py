#! /usr/bin/env python

import sys
import os

def handle_line(line, transparent):
    line = line.replace('#', '')
    L = line.split()
    if (len(L) < 2): return None
    pos = int(L[0])
    #color = int(L[1], 16)
    color = L[1]
    return "\t{ %0.2ff, %s%s, },\n" % ((100-pos)/100.0, transparent, color)
    #return "\t{ %0.2ff, %s%s, },\n" % ((64-(pos-36))/64.0, transparent, color)

def gen_gradient(fin, fout, arrayname):
    L = []

    transparent = 'ff'
    name = arrayname

    def gen_array():
        text = '''
static const GradientData %s[] = { 
%s};
''' % (name, ''.join(L))
        fout.write(text)

    for line in fin:
        if line.startswith('#'):
            continue
        if line.startswith('name:'):
            if (len(L)>0):
                gen_array()
                del L[:]
            name = line.replace('name:', '')
            name = name.strip()
        elif line.startswith('transparence:'):
            transparent = line.replace('transparence:', '')
            transparent = transparent.replace('%', '')
            transparent = transparent.strip()
            transparent = int(int(transparent) / 100.0 * 255)
            transparent = hex(transparent)
        else:
            entry = handle_line(line, transparent)
            if entry is not None:
                L.append(entry)

    gen_array()

if __name__ == '__main__':

    if len(sys.argv) <= 1:
        print "Usage: gen_gradient.py filename"
        exit(1)

    infile = sys.argv[1]

    fin = open(infile)
    fout = sys.stdout
    if len(sys.argv) > 2:
        fout = open(sys.argv[2], 'w')
    arrayname = os.path.splitext(os.path.basename(infile))[0]
    gen_gradient(fin, fout, arrayname)
    fout.close()
    fin.close()
