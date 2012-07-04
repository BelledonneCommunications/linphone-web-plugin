#!/usr/bin/python

import shutil
import sys

def main(argv=None):
  if argv is None:
      argv = sys.argv
  if len(argv) == 3:
    src = argv[1]
    dest = argv[2]
    shutil.copytree(src,dest)

if __name__ == "__main__":
    sys.exit(main())
