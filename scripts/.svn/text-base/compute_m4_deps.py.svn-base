
import sys
import os
import os.path
import re

def get_inc_paths(vals):
	inc_paths = []
	for i in range(1, len(vals)):
		v = vals[i]
		if v == '-I':
			inc_paths.append(vals[i+1])
		elif v.startswith('-I'):
			inc_paths.append(vals[i][2:].strip())
	return map(os.path.abspath, inc_paths)

def process_m4_lines(input):
	files = set()
	syn = re.compile(r'^#line [0-9]+ "(?P<name>.*)"$')
	for line in input.readlines():
		m = syn.match(line.strip())
		if m:
			files.add(str(m.group('name')))
	return files

def get_file(inc_paths, file):
	if file == 'stdin':
		return None
	for i in inc_paths:
		f = os.path.join(i, file)
		if os.path.isfile(f):
			return f
	return None

def print_mf(out, dest, prereqs):
	print >> out,  dest, ':',
	for p in prereqs:
		print >> out,  '\\'
		print >> out,  '\t', p,
	print >> out

if __name__ == '__main__':
	inc_paths = get_inc_paths(sys.argv[2:])
	used_files = process_m4_lines(sys.stdin)
	deps = filter(None, map(lambda x:get_file(inc_paths, x), used_files))
	print_mf(sys.stdout, sys.argv[1], deps)
	
