import hashlib
import random
import socket
import tkinter
from Crypto.Cipher import AES
from binascii import a2b_hex
import server
import threading

def random_code() -> str:
	x = random.randint(0, 2)
	s = ''
	if x == 0:
		s = chr(random.randint(65, 90))
	elif x == 1:
		s = chr(random.randint(97, 122))
	elif x == 2:
		s = str(random.randint(0, 9))
	return s

def calculate_hash(user_name: str, password: str) -> tuple:
	md5 = hashlib.md5()
	md5.update(user_name.encode('utf-8'))
	md5.update(password.encode('utf-8'))
	hash1 = md5.hexdigest()

	authen_code = ''.join([random_code() for i in range(6)])

	md5_ = hashlib.md5()
	md5_.update(hash1.encode('utf-8'))
	md5_.update(authen_code.encode('utf-8'))
	hash2 = md5_.hexdigest()

	return hash1, hash2, authen_code

def decrypt(hash1: str, text: str) -> str:
	cryptor = AES.new(hash1[0:16], AES.MODE_CBC, hash1[16:32])
	authen_code = cryptor.decrypt(a2b_hex(bytes(text, encoding='utf-8')))

	authen_code = str(authen_code, encoding='utf-8')

	return authen_code.rstrip('\0')

def on_click():
	text.delete(1.0, 'end')
	user_name = user_name_input.get()
	password = password_input.get()
	hash1, hash2, authen_code = calculate_hash(user_name, password)
	content = user_name + '\n' + hash2 + '\n' + authen_code
	s = socket.socket()
	try:
		s.connect(('127.0.0.1', 5300))
		s.sendall(bytes(content, encoding='utf-8'))

		recv_content = str(s.recv(1024), encoding='utf-8').split('\n')
		if recv_content[0] == 'yes':
			text.insert(1.0, 'Login successfully!')
			authen_code = decrypt(hash1, recv_content[1])
			print(authen_code)
			with open('Authentication_code.txt', 'a') as f:
				f.write(authen_code + '\n')
		else:
			text.insert(1.0, 'Login failed!')
		s.close()
	except socket.error:
		pass

if __name__ == '__main__':
	t = threading.Thread(target=server.run, args=())
	t.start()

	root = tkinter.Tk()
	root.title('Login')
	root.geometry('400x250')

	frame1 = tkinter.Frame(root)
	tkinter.Label(frame1, text='用户名', font=("Arial", 12), width=7, height=2).pack(side=tkinter.LEFT)
	user_name_input = tkinter.StringVar()
	tkinter.Entry(frame1, textvariable=user_name_input, bd=3).pack(side=tkinter.LEFT)
	frame1.pack()

	frame2 = tkinter.Frame()
	tkinter.Label(frame2, text='密码', font=('Arial', 12), width=7, height=2).pack(side=tkinter.LEFT)
	password_input = tkinter.StringVar()
	tkinter.Entry(frame2, textvariable=password_input, bd=3).pack(side=tkinter.LEFT)
	frame2.pack()

	tkinter.Button(root, text='Login', command=on_click, font=("Arial", 12), width=7, height=1).pack(side=tkinter.TOP)

	text = tkinter.Text(root, font=("Arial", 10))
	text.pack(side=tkinter.TOP)

	root.mainloop()
