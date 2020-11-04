import subprocess
out = subprocess.run('sh main.sh', shell=True, capture_output=True)
val = out.stdout.decode()
print(val)
print(ord(val[0]), ord(val[1]))
print(ord(val[2]), ord(val[3]))
print(ord(val[4]), ord(val[5]))
print(ord(val[6]), ord(val[7]))

