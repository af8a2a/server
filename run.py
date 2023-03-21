
import sys, getopt, os,socket,_thread


if __name__ == "__main__":
    #os.system('webbench -c 1 -t 100 http://127.0.0.1:1234/')
     sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
     sock.connect(('localhost',1234))
     sock.send(bytes('Get'.encode(encoding='UTF-8')))
     print(sock.recv(1024))
     sock.close()