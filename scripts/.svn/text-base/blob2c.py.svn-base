#!/usr/bin/env python

import sys

from optparse import OptionParser

parser = OptionParser(usage="%prog [ -n symbol_name ] [ -s symbol_section ] blob > output")
parser.add_option('-n',
				  dest = 'name', nargs = 1, type = 'string',
				  help = 'Change symbol name (default = "blob")',
				  default = 'blob')
parser.add_option('-a',
				  dest = 'align', nargs = 1, type = 'int',
				  help = 'Add an alignment constraint on blob variable',
				  default = '1')
parser.add_option('-s',
				  dest = 'section', nargs = 1, type = 'string',
				  help = 'Change section name (no default (.rodata))',
				  default = '')
parser.add_option('-S',
				  dest = 'size', action = 'store_true',
				  help = 'Also emit a size symbol "<name>_size"',
				  default = False)
parser.add_option('-o',
				  dest = 'output', nargs = 1, type = 'string',
				  help = 'Change section output file name (default = -)',
				  default = '-')
opts, args = parser.parse_args()

output = sys.stdout

if opts.output != '-':
	output = open(opts.output, 'w')

input = sys.stdin

if args and args[0] != '-':
	input = open(args[0], 'r')
blob = input.read()
input.close()

if opts.size:
	print >> output, '#include <stdint.h>'
if opts.section:
	print >> output, '__attribute__((section "%s"))'%opts.section
if opts.align != 1:
	print >> output, '__attribute__((aligned (%d)))'%opts.align
print >> output, 'const unsigned char %s[] = {'%opts.name
for i in range(0, len(blob), 8):
	print >> output, '   ',
	for c in blob[i:i+8]:
		print >> output, "0x%02x,"%ord(c),
	print >> output
print >> output, '};'
if opts.section:
	print >> output, '__attribute__((section "%s"))'%opts.section
if opts.size:
    print >> output, 'const size_t %s_size = %d;'%(
        opts.name, len(blob))
