#!/usr/bin/python

import re
import shutil
import sys

def main(argv=None):
  if argv is None:
    argv = sys.argv
  if len(argv) == 2:
    filename = argv[1]
    fin = open(filename, "r")
    text = fin.read()
    pattern = re.compile(r'(var linphone.*?$)|(\s*?/\*.*?\*/\s*?$)', re.S | re.MULTILINE)
    text = re.sub(pattern, '', text)
    fout = open(filename, "w")
    fout.write("/*globals linphone*/")
    fout.write(text)

if __name__ == "__main__":
    sys.exit(main())
