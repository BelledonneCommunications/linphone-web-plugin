#!/usr/bin/env python
# -*- mode: python coding: utf-8 -*-
"""
building google chrome extension crx with commandline
it is inspired by rubygem's crxmake
requires: M2Crypto module (and "openssl" commandline)
"""
import dircache
import hashlib
import io
import os
import struct
import subprocess
import sys
import zipfile
import M2Crypto.EVP
import M2Crypto.RSA
import M2Crypto.BIO

class PassFile:
       def __init__(self, file):
               self.file = file
       def password(self, v):
               file = open(self.file, 'r');
               line = file.read()
               file.close()
               return line

			   
CRX_ALGO = "30819F300D06092A864886F70D010101050003818D00"
CRX_TRANS = {"0":"a", 
"1":"b",
"2":"c",
"3":"d",
"4":"e",
"5":"f",
"6":"g",
"7":"h",
"8":"i",
"9":"j",
"a":"k",
"b":"l",
"c":"m",
"d":"n",
"e":"o",
"f":"p"}

def crxmake(dirname, pem_name, pass_file, crx_name):
    if dirname.endswith(os.path.sep): dirname = dirname[:-1]

    # make raw zip data
    zip_memory = io.BytesIO()
    zip = zipfile.ZipFile(zip_memory, "w", zipfile.ZIP_DEFLATED)
    def make_zip(z, path, parent):
        for ch in dircache.listdir(path):
            child = os.path.join(path, ch)
            name = "%s/%s" % (parent, ch)
            if os.path.isfile(child): z.write(child, name)
            if os.path.isdir(child): make_zip(z, child, name)
            pass
        pass
    make_zip(zip, dirname, "")
    zip.close()
    zip_data = zip_memory.getvalue()

    # load or make private key PEM
    key = M2Crypto.RSA.load_key(pem_name, callback=PassFile(pass_file).password)

    # make sign for zip_data with private key
    sign = key.sign(hashlib.sha1(zip_data).digest(), "sha1")

    # generate public key DER 
    if hasattr(key, "save_pub_key_der_bio"):
        mem_bio = M2Crypto.BIO.MemoryBuffer()
        key.save_pub_key_der_bio(mem_bio) # missing API on M2Crypto <= 0.20.2
        der_key = mem_bio.getvalue()
        pass
    else:
        der_key = subprocess.Popen(
            ["openssl", "rsa", "-pubout", "-outform", "DER",
             "-inform", "PEM", "-in", pem_name],
            stdout=subprocess.PIPE).stdout.read()
        pass

    # make crx
    magic = "Cr24"
    version = struct.pack("<I", 2)
    key_len = struct.pack("<I", len(der_key))
    sign_len = struct.pack("<I", len(sign))
    with open(crx_name, "w") as crx:
        crx.write(magic)
        crx.write(version)
        crx.write(key_len)
        crx.write(sign_len)
        crx.write(der_key)
        crx.write(sign)
        crx.write(zip_data)
        crx.flush()
        pass
    print("update package: %s" % crx_name)

if __name__ == '__main__':
  if len(sys.argv) < 5:
    print 'Usage: %s source_dir key_file pass_file output_file' % sys.argv[0]
    sys.exit(2)
  crxmake(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])