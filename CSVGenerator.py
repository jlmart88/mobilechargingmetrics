import csv
import datetime
import random
import time as Time
f = open('data.csv', 'w').close()

while True:
    csvfile = open('data.csv', 'a', newline='')
    csvwriter = csv.writer(csvfile, dialect='excel')

    for portNum in range(1,9):       
        date = datetime.datetime.now().date().isoformat()
        time = datetime.datetime.now().time().isoformat()
        current = random.random()*2
        csvwriter.writerow([portNum,current,time,date])
    print("Wrote a sequence\n")
    csvfile.close()
    Time.sleep(5)        
    
