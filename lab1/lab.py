import socket
import threading
import tkinter

def scan(ip, port):
	global output
	s = socket.socket()
	s.settimeout(0.1)   # 设置超时时间
	out = s.connect_ex((ip, port))
	if out == 0:
		output += str(port) + ' : OPEN \n'
	else:
		output += str(port) + ' : CLOSE\n'
	s.close()


def on_click():
	global output
	text.delete(1.0, 'end')  # 清空上次扫描结果
	nums_thread = int(nums_text.get())
	ip = ip_text.get()
	output = ip + ' :\n'
	port_min = int(port_left_text.get())
	port_max = int(port_right_text.get())
	ports = [i for i in range(port_min, port_max+1)]

	for port in ports:
		while True:
			# 等号是因为主线程的存在
			if threading.active_count() <= nums_thread:
				thread = threading.Thread(target=scan, args=(ip,port))
				thread.start()
				break

	# 等待所有子线程执行结束
	while threading.active_count() > 1:
		pass

	label.set('扫描结果')
	text.insert(1.0, output)

if __name__== "__main__":
	tk = tkinter.Tk()
	tk.title('端口扫描')
	tk.geometry('480x640')

	frm_1 = tkinter.Frame(tk)
	tkinter.Label(frm_1, text='线程数', font=("Arial", 12), width=7, height=2).pack(side=tkinter.LEFT)
	nums_text = tkinter.StringVar()
	nums = tkinter.Entry(frm_1, textvariable=nums_text, bd=3)
	nums_text.set('')
	nums.pack(side=tkinter.LEFT)
	frm_1.pack(side=tkinter.TOP)

	frm_2 = tkinter.Frame(tk)
	tkinter.Label(frm_2, text="ip", font=("Arial", 12), width=7, height=2).pack(side=tkinter.LEFT)
	ip_text = tkinter.StringVar()
	ip = tkinter.Entry(frm_2, textvariable=ip_text, bd=3)
	ip_text.set('')
	ip.pack(side=tkinter.LEFT)
	frm_2.pack(side=tkinter.TOP)

	frm_3 = tkinter.Frame(tk)
	tkinter.Label(frm_3, text='端口范围', font=("Arial", 12), width=7, height=2).pack(side=tkinter.LEFT)
	port_left_text = tkinter.StringVar()
	port_l = tkinter.Entry(frm_3, textvariable=port_left_text, bd=3, width=6)
	port_left_text.set('')
	port_l.pack(side=tkinter.LEFT)
	tkinter.Label(frm_3, text='   -   ', font=("Arial", 16)).pack(side=tkinter.LEFT)
	port_right_text = tkinter.StringVar()
	port_r = tkinter.Entry(frm_3, textvariable=port_right_text, bd=3, width=6)
	port_right_text.set('')
	port_r.pack(side=tkinter.LEFT)
	frm_3.pack(side=tkinter.TOP)

	tkinter.Button(tk, text='扫描', command=on_click, font=("Arial", 12), width=7, height=1).pack(side=tkinter.TOP)

	label = tkinter.StringVar()
	label.set('')
	tkinter.Label(tk, textvariable=label, font=("Arial", 10)).pack(side=tkinter.LEFT)
	# text = tkinter.StringVar()
	# text.set('test')
	
	scroll = tkinter.Scrollbar()
	scroll.pack(side=tkinter.RIGHT, fill=tkinter.Y)
	text = tkinter.Text(tk, font=("Arial", 10))
	scroll.config(command=text.yview)
	text.config(yscrollcommand=scroll.set)
	text.pack(side=tkinter.LEFT)

	tk.mainloop()