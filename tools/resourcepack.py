#!/usr/bin/python
import sys, re, json, os, errno

def clean(s):
	# Remove invalid characters
	s = re.sub('[^0-9a-zA-Z_]', '', s)
	# Remove leading characters until we find a letter or underscore
	s = re.sub('^[^a-zA-Z_]+', '', s)
	return s

if __name__ == "__main__":
	cdata = dict()

	for res in sys.argv[2:]:
		binary_cdata = ""
		
		with open(res, 'rb') as f:
			data = f.read()
		
			for b in data:
				binary_cdata += hex(ord(b)) + ", "
		
		binary_cdata = binary_cdata[:-2];
		
		resource_c = "static const char %s[] = { %s };" % (clean(res), binary_cdata)
		resource_path = "rsc:/"+res
		cdata[resource_path] = resource_c;
	
	if os.path.dirname(sys.argv[1]) != "":
		if not os.path.exists(os.path.dirname(sys.argv[1])):
			try:
				os.makedirs(os.path.dirname(sys.argv[1]))
			except OSError as exc: # Guard against race condition
				if exc.errno != errno.EEXIST:
					raise

	with open(sys.argv[1], 'w') as f:
		f.write(json.dumps(cdata))
