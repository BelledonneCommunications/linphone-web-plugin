import re, sys

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "on", "1")

def replace(src, dest, variable, value):
  infile = open(src, "r")
  intext = infile.read()
  infile.close()
  match = r"\${IF " + variable + r"}(.*?)\${ENDIF}"
  if str2bool(value):
    replace = r"\1"
  else:
    replace = r""
  re_match = re.compile(match, re.MULTILINE|re.DOTALL)
  outtext = re.sub(re_match, replace, intext)
  outfile = open(dest, "w")
  outfile.write(outtext)
  outfile.close() 

if __name__ == '__main__':
  if len(sys.argv) != 5:
    sys.exit(5)
  replace(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    
