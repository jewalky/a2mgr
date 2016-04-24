import os
import hashlib
import sys

sha1_o = ''
try:
	with open('./version', 'r') as f:
		sha1_oc = hashlib.sha1()
		sha1_oc.update(f.read())
		sha1_o = sha1_oc.hexdigest()
except:
	sha1_o = ''

#odata = open('./version', 'w')
odata = ''
for subdir, dirs, files in os.walk('.'):
	for file in files:
		if subdir == '.' and (file == 'version' or file == 'genversion.py' or file == 'version_id' or file == 'genversion.cmd'):
			continue
		fn = os.path.join(subdir, file).replace('\\', '/')[2:]
		sha1 = hashlib.sha1()
		f = open(fn, 'rb')
		try:
			sha1.update(f.read())
		finally:
			f.close()
		d = sha1.hexdigest()
		odata += '%s %s\n' % (fn, d)
		sys.stdout.write('\rProcessing %s...                          ' % (fn))
		sys.stdout.flush()
sys.stdout.write('\n')
sys.stdout.flush()

with open('./version', 'w') as of:
	of.write(odata)

with open('./version', 'r') as f:
	sha1_oc = hashlib.sha1()
	sha1_oc.update(f.read())
	sha1_o2 = sha1_oc.hexdigest()

if sha1_o2 != sha1_o:
	print('Updates detected, version_id bumped')
	with open('./version_id', 'w') as of:
		of.write(sha1_o2)
else:
	print('No updates detected, version_id unchanged')

print('Done!')
