import re;
file = open("hfStat_f") 
sum = 0;
while 1:
    line = file.readline()
    if not line:
        break
    strTmp = "Chunks";
    if not cmp(line[:6], strTmp[:6]):
        #print line;
        line = re.sub("\D", "", line);
        #print line;
        num = int(line);
        sum = sum + num; 
file.close()

print sum;