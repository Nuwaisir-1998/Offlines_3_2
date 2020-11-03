import subprocess
out = subprocess.run('sh main.sh', shell=True, capture_output=True)
val = out.stdout.decode()

