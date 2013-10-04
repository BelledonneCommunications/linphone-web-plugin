#!/usr/bin/python

import shutil
import sys

def main(argv=None):
  if argv is None:
      argv = sys.argv
  if len(argv) >= 3:
    dest = argv[1]
    srcs = argv[2:]
    fout = open(dest, "w")
    for src in srcs:
        fin = open(src, "r")
        text = fin.read()
        fout.write(text)
        fout.write("\n")

if __name__ == "__main__":
    sys.exit(main())
