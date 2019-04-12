import hashlib
import socket
from Crypto.Cipher import AES
from binascii import b2a_hex
import pymysql

def calculate_hash(hash1: str, authen_code: str) -> str:
    md5 = hashlib.md5()
    md5.update(hash1.encode('utf-8'))
    md5.update(authen_code.encode('utf-8'))
    return md5.hexdigest()

def encrypt(hash1: str, authen_code: str) -> str:
    cryptor = AES.new(hash1[0:16], AES.MODE_CBC, hash1[16:32])
    count = len(authen_code)
    add = 16 - (count % 16)
    authen_code += ('\0' * add)
    text = cryptor.encrypt(authen_code)

    return str(b2a_hex(text), encoding='utf-8')

def run():
    db = pymysql.connect('localhost', 'root', 'wk...1997', 'password')

    cursor = db.cursor()
    sql = 'select * from K'
    try:
        cursor.execute(sql)
        data = cursor.fetchall()
        dicts = {d[0]:d[1] for d in data}
    except:
        db.rollback()

    db.close()

    s = socket.socket()
    s.bind(('127.0.0.1', 5300))
    s.listen()
    while True:
        content = ''
        sock, addr = s.accept()

        recv_content = str(sock.recv(1024), encoding='utf-8').split('\n')
        if recv_content[0] in dicts.keys():
            hash2 = calculate_hash(dicts[recv_content[0]], recv_content[2])
            if hash2 == recv_content[1]:
                # print('Login successfully!')
                en = encrypt(dicts[recv_content[0]], recv_content[2])
                content = 'yes' + '\n' + en
        else:
            content = 'no'
        sock.sendall(bytes(content, encoding='utf-8'))
        sock.close()
    s.close()

if __name__ == '__main__':
    # dicts = {'Bob':'20072d47594f9d4fd632c2f169af779c',
    # 'Alice':'fe533768e85cc7eeef098ea854075bf3',
    # 'Tony':'b1a3ef54950dbd24014c7689a258acf7'}

    run()
