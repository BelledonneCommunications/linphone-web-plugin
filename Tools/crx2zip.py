#!/usr/bin/env python
# -*- mode: python coding: utf-8 -*-
import sys
import struct

#    magic = "Cr24"
#    version = struct.pack("<I", 2)
#    key_len = struct.pack("<I", len(der_key))
#    sign_len = struct.pack("<I", len(sign))
#    with open(crx_name, "wb") as crx:
#        crx.write(magic)
#        crx.write(version)
#        crx.write(key_len)
#        crx.write(sign_len)
#        crx.write(der_key)
#        crx.write(sign)
#        crx.write(zip_data)
#        crx.flush()


def crx2zip(source, target):
    with open(source, "rb") as crx:
        print "Read crx %s" % source
        magic = crx.read(4)
	if magic <> "Cr24":
		raise Exception("Invalid")
	version = struct.unpack("<I", crx.read(4))[0]
	key_len = struct.unpack("<I", crx.read(4))[0]
        sign_len = struct.unpack("<I", crx.read(4))[0]
        print "Keylen %d" % key_len
        dummy_key = crx.read(key_len)
        dummy_sign = crx.read(sign_len)
        zip_data = crx.read()
        with open(target, "wb") as zip:
            print "Write zip %s" % target
            zip.write(zip_data)

if __name__ == '__main__':
  if len(sys.argv) != 3:
    print 'Usage: %s source_file target_file' % sys.argv[0]
    sys.exit(2)
  crx2zip(sys.argv[1], sys.argv[2])
