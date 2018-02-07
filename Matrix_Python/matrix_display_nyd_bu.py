
#_*_  coding:UTF-8 _*_
import time
import serial
import datetime
import glob
import os
import thread

os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')
base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')
device_file = device_folder[0] + '/w1_slave'

temp_c = ""
message_dir =  '/home/pi/Documents/'
#message_folder = glob.glob(message_dir + 'read_file_line_n.txt')
#message_folder = glob.glob(message_dir + 'onetoten.txt')
#message_folder = glob.glob(message_dir + 'empty_file.txt')
message_folder = glob.glob(message_dir + 'matrix_2018.txt')
message_file = message_folder[0]

last_time = ""
last_date = ""
last_temp = ""
mesg = "[~"
oldmesg = "xxx"
update_date = 10
update_temp = 10
update_mesg = 10

def readTemp(delay):
    global temp_c
    f = open(device_file, 'r')
    while True:
        f.seek(0)
        lines = f.readlines()
        if lines[0].strip()[-3: ] == 'YES':
            equals_pos = lines[1].find('t=')
            if equals_pos != -1:
                temp_string = lines[1][equals_pos+2: ]
                temp_c1 = float(temp_string) / 1000.0
                temp_c =  "%.1f" % temp_c1
        time.sleep (delay)
    return

thread.start_new_thread (readTemp, ( 5, ))

def selectMessage():
    global  mesg
    global oldmesg #ensure mesg is only sent once.
    mesg_ts = -1
    lines = ( "Starting up" )
    index = 0
    while True:
        # Check time file stamp
        temp_ts = os.path.getmtime (message_file)
        if mesg_ts != temp_ts:
            # Re-read the file
            f = open (message_file)

            lines = f.readlines ()
            f.close ()
            mesg_ts = temp_ts
            index = 0

        # Check time of day
        l = time.localtime(time.time ())
        t = l.tm_hour * 100 + l.tm_min
        if (t >0600) & (t < 2200):
            # Pick message
            if index < len (lines):
                #print index
                #print len(lines)
                mesg = "[" + lines [index].strip () + "~"
                #print mesg
                index += 1
                #print mesg
                #print index
            else:
                if len (lines) > 0:
                    index = 0
                else:
                    mesg = "[Empty message file~"
        else:
            #mesg = "[Zzz~" 
            if oldmesg == "[Zzz~":
                mesg = "[Zzzz~" #with an extra 'z'
            else:
                mesg = "[Zzz~" #with no space
        time.sleep (40)
        
thread.start_new_thread (selectMessage, ())

ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()
time.sleep (2)
while ser.inWaiting() > 0:ser.read(1)
while 1:
    temp = temp_c
    tim = time.strftime('%H:%M:%S')
    hms = ''.join(('[',tim,']'))
    dat = time.strftime('%d-%m-%Y')
    dmy = ''.join(('[',dat,'!'))
    tmp = ''.join(('[',temp,'^'))

    if last_time != hms:
        ser.write(hms)
        last_time = hms
        #print last_time
        
    if (update_date ==  0) or (last_date != dmy):
        ser.write(dmy)
        last_date = dmy
        update_date = 500
        #print last_date
        
    if (update_temp == 0) or last_temp != tmp:
        ser.write(tmp)
        last_temp = tmp
        update_temp = 150
        #print last_temp

    if update_mesg == 0:
        #l = time.localtime(time.time ())
        #t = l.tm_hour * 100 + l.tm_min
        if mesg != oldmesg: # or(t < 1932) & (t > 1927): # only send mesg once.
            ser.write(mesg)
            oldmesg = mesg
            #print mesg
        update_mesg = 100

    update_date -= 1
    update_temp -= 1
    update_mesg -= 1
    #print update_mesg
    
    time.sleep(0.2)

  

        
 
