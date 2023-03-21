import os

return_code = os.system('webbench -c 10000 -t 20 http://127.0.0.1:8080/')
print("return code:", return_code)
