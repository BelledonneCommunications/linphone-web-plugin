#!/usr/bin/env python
# from https://github.com/nmaier/xpisign.py
# modified for add a new argument taking the path of a the file containing the password for the certificate
from __future__ import with_statement

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "xpisign/")

from optparse import OptionParser

from xpisign import xpisign, BytesIO, __version__

def main(args):
    global smime_pkcs7_der_sign

    optparse = OptionParser(usage="Usage: %prog [options] xpifile outfile")
    optparse.add_option("-k",
                        "--keyfile",
                        dest="keyfile",
                        default="sign.pem",
                        help="Key file to get the certificate from"
                        )
    optparse.add_option("-a",
                        "--passfile",
                        dest="passfile",
                        default=None,
                        help="File containing password"
                        )
    optparse.add_option("-f",
                        "--force",
                        dest="force",
                        action="store_true",
                        default=False,
                        help="Force signing, i.e. overwrite outfile if it already exists"
                        )
    optparse.add_option("-p",
                        "--plain",
                        dest="optimize",
                        action="store_false",
                        default=True,
                        help="Generate plain XPI without any optimizations."
                        )
    optparse.add_option("-s",
                        "--signer",
                        dest="signer",
                        default=None,
                        help="Force signing with a particular implementation (m2, openssl)"
                        )
    optparse.add_option("-v",
                        "--version",
                        dest="printversion",
                        default=False,
                        action="store_true",
                        help="Print version"
                        )
    options, args = optparse.parse_args(args)

    if options.printversion:
        print __version__
        return 0

    try:
        xpifile, outfile = args
    except ValueError:
        optparse.error("Need to specify xpifile and outfile!")

    if not os.path.isfile(xpifile):
        optparse.error("xpifile %s is not a file" % xpifile)

    if not options.force and os.path.exists(outfile):
        optparse.error("outfile %s already exists" % outfile)

    keyfile = options.keyfile
    if not os.path.exists(keyfile):
        optparse.error("keyfile %s cannot be found" % keyfile)
    passfile = options.passfile
    optimize = options.optimize
    signer = options.signer

    try:
        # buffer stuff, in case xpifile == outfile
        with open(xpifile, "rb") as tp:
            xp = BytesIO(tp.read())
        with xp:
            try:
                with BytesIO() as op:
                    try:
                        xpisign(xpifile=xp,
                                keyfile=keyfile,
								passfile=passfile,
                                outfile=op,
                                optimize_signatures=optimize,
                                optimize_compression=optimize,
                                signer=signer
                                )
                        with open(outfile, "wb") as outp:
                            outp.write(op.getvalue())
                    except ValueError, ex:
                        optparse.error(ex.message)
            except IOError:
                optparse.error("Failed to open outfile %s" % outfile)
    except IOError:
        optparse.error("Failed to open xpifile %s" % xpifile)

    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

# vim: ts=4:sw=4:et
