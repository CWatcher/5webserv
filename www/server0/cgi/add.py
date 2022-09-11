import cgi
import cgitb

cgitb.enable()
data = cgi.FieldStorage()
a = data['a'].value
b = data['b'].value

result = f'result = {int(a) + int(b)}'
print(f'Content-Length: {len(result)}')
print('CGI-Header: hello from cgi')
print('Content-Type: text/plain')
print('Status: 200 the addition was successful')
print()
print(result)
print('INVISIBLE')
