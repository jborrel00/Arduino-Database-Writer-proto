import MySQLdb as mdb
import serial
import glob

if '/dev/ttyACM1' in glob.glob('/dev/tty*'):
  ser = serial.Serial('/dev/ttyACM1',115200)
else:
	ser = serial.Serial('/dev/ttyACM0',115200)

r=[]
con = mdb.connect('localhost','root','foosball','foosball')
with con:
	cur = con.cursor();
	cur.execute("select hex from name_hex_data where name = 'Test'")
	rows = cur.fetchall()
	for row in rows:
		print row[0]
		r=row[0].replace(" ","")
		#print r + 1

                r = hex(r)[2:]
                r = r[:-1]
                while len(r) < 32:
                        r = "0" + r
		print r
print r
ser.write(r)
