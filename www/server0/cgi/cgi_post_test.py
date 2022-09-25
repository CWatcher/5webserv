from sys import stdin
import cgi
import cgitb

cgitb.enable()

print('Content-Type: text/plain')
print()
for i in range(1):
	line = stdin.read(10000000)
	print(line)
print(f"10000000 Hello!")
