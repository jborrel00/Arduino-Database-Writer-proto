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
	cur.execute("select hex from Name_hex_data where name = 'test'")
	rows = cur.fetchall()
	for row in rows:
		#print row[0]
		r=row[0].replace(" ","")
		print r
ser.write(r)
