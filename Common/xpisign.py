#!/usr/bin/env python
import os, sys, re, hashlib, zipfile, base64, M2Crypto
class PassFile:
       def __init__(self, file):
               self.file = file
       def password(self, v):
               file = open(self.file, 'r');
               line = file.readline().strip()
               file.close()
               return line

def signDir(source_dir, key_file, pass_file, output_file):
  source_dir = os.path.abspath(source_dir)
 
  # Build file list
  filelist = []
  for dirpath, dirs, files in os.walk(source_dir):
    for file in files:
      abspath = os.path.join(dirpath, file)
      relpath = os.path.relpath(abspath, source_dir).replace('\\', '/')
      handle = open(abspath, 'rb')
      filelist.append((abspath, relpath, handle.read()))
      handle.close()
 
  # Generate manifest.mf and zigbert.sf data
  manifest_sections = []
  signature_sections = []
  def digest(data):
    md5 = hashlib.md5()
    md5.update(data)
    sha1 = hashlib.sha1()
    sha1.update(data)
    return 'Digest-Algorithms: MD5 SHA1\nMD5-Digest: %s\nSHA1-Digest: %s\n' % \
           (base64.b64encode(md5.digest()), base64.b64encode(sha1.digest()))
  def section(manifest, signature):
    manifest_sections.append(manifest)
    signature_sections.append(signature + digest(manifest))
  section('Manifest-Version: 1.0\n', 'Signature-Version: 1.0\n')
  for filepath, relpath, data in filelist:
    section('Name: %s\n%s' % (relpath, digest(data)), 'Name: %s\n' % relpath)
  manifest = '\n'.join(manifest_sections)
  signature = '\n'.join(signature_sections)
 
  # Generate zigbert.rsa (detached zigbert.sf signature)
  handle = open(key_file, 'rb')
  key_data = handle.read()
  handle.close()
  certstack = M2Crypto.X509.X509_Stack()
  first = True
  certificates = re.finditer(r'-----BEGIN CERTIFICATE-----.*?-----END CERTIFICATE-----', key_data, re.S)
  # Ignore first certificate, we will sign with this one. Rest of them needs to
  # be added to the stack manually however.
  certificates.next()
  for match in certificates:
    certstack.push(M2Crypto.X509.load_cert_string(match.group(0)))
 
  mime = M2Crypto.SMIME.SMIME()
  if key_file is not None:
    mime.load_key(key_file, callback=PassFile(pass_file).password)
  else:
    key = M2Crypto.RSA.gen_key(2048, 65537)
    bio = M2Crypto.BIO.MemoryBuffer()
    key.save_key_bio(bio, None)
    mime.load_key_bio(bio)

  mime.set_x509_stack(certstack)
  pkcs7 = mime.sign(M2Crypto.BIO.MemoryBuffer(signature),
                    M2Crypto.SMIME.PKCS7_DETACHED | M2Crypto.SMIME.PKCS7_BINARY)
  pkcs7_buffer = M2Crypto.BIO.MemoryBuffer()
  pkcs7.write_der(pkcs7_buffer)
 
  # Write everything into a ZIP file, with zigbert.rsa as first file
  zip = zipfile.ZipFile(output_file, 'w', zipfile.ZIP_DEFLATED)
  zip.writestr('META-INF/zigbert.rsa', pkcs7_buffer.read())
  zip.writestr('META-INF/zigbert.sf', signature)
  zip.writestr('META-INF/manifest.mf', manifest)
  for filepath, relpath, data in filelist:
    zip.writestr(relpath, data)
 
if __name__ == '__main__':
  if len(sys.argv) != 5 and len(sys.argv) != 3:
    print 'Usage: %s source_dir key_file pass_file output_file' % sys.argv[0]
    sys.exit(2)
  if len(sys.argv) == 5:
    signDir(sys.argv[1], sys.argv[3], sys.argv[4], sys.argv[2])
  elif len(sys.argv) == 3:
    signDir(sys.argv[1], None, None, sys.argv[2])
