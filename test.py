import re;
file = open("hfStat_f") 
sum = 0;
while 1:
    line = file.readline()
    if not line:
        break
    strTmp = "Chunks";
    strTmp2 = "File path:";
    if not cmp(line[:10], strTmp2[:10]):
        #print line;
        if line.find('.') == -1:
            print 'NaNType';
        else:
            fileType = line.split('.');
            print fileType[len(fileType)-1].replace("\n", "");
    if not cmp(line[:6], strTmp[:6]):
        #print line;
        line = re.sub("\D", "", line);
        #print line;
        num = int(line);
        print num;
        sum = sum + num; 

file.close()