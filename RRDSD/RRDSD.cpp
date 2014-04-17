//
//  RRDSD.cpp
//  
//
//  Created by Justin Martinez on 2/28/14.
//
//

#include "Arduino.h"
#include "RRDSD.h"
#include "SdFat.h"
//#include "math.h"
//#include "MemoryFree.h"

//length and interval should be in seconds
//
RRDSD::RRDSD(char fileName[], unsigned long interval, unsigned long length, int dataLength, int compareLength, RRDSD *archive, int (*archiveFunction)(int*,int))
{
    _fileName = fileName;
    _interval = interval;
    _length = length;
    _archive = archive;
    _archiveFunction = archiveFunction;
    _dataLength = dataLength;
    _compareLength = compareLength;
    if (_archive != NULL){
        _archiveLength = _archive->getInterval();
        _archiveElements = _archiveLength/_interval;
    } else{
        _archiveLength = 0;
        _archiveElements = 0;
    }
    //_start = 0;
    //_end = _length/_interval - 1;
    _totalLength = _length+_archiveLength;
    _totalElements = _totalLength/_interval;
    //_archiveEnd = _totalElements-1;
    //_archiveStart = _archiveEnd-_archiveElements+1;
    //_archiveCounter = _archiveElements-_totalElements;
    //Serial.print(F("ArchiveCounter instantiated: ")); Serial.println(_archiveCounter);
    _headersByteEnding = 3*(_dataLength+2);
    _fileByteEnding = ((_dataLength+2)*(_totalElements))+_headersByteEnding;
    
    initializeFile();
}
RRDSD::RRDSD(){}

unsigned long RRDSD::getLength()
{
    return _length;
}
/*
unsigned long RRDSD::getArchiveLength()
{
    return _archiveLength;
}
*/
unsigned long RRDSD::getInterval()
{
    return _interval;
}

bool RRDSD::initializeFile()
{
    //read the headers to see if there is existing data in the file
    int headerVals[3];
    parseHeaders(&headerVals[0]);
    //Serial.print(F("HeaderVals: ")); Serial.print(headerVals[0]); Serial.print(" "); Serial.print(headerVals[1]); Serial.print(" "); Serial.println(headerVals[2]);
    if (headerVals[0] != -1){
        _start = headerVals[0];
        _end = headerVals[1];
        _archiveCounter = headerVals[2];
        _archiveEnd = (_start + _totalElements-1)%_totalElements;
        _archiveStart = (_archiveEnd-_archiveElements+1+_totalElements)%_totalElements;
    }
    else {
        resetFile();
    }
    return true;
}

bool RRDSD::resetFile()
{
    //Serial.println(F("Resetting File"));
    _start = 0;
    _end = _length/_interval - 1;
    _archiveEnd = _totalElements-1;
    _archiveStart = _archiveEnd-_archiveElements+1;
    _archiveCounter = _archiveElements-_totalElements;
    Serial.print(F("Clear: ")); Serial.println(clearFile());
    updateHeaders();
    fillEmptyData();
    return true;
}

bool RRDSD::clearFile()
{
    //Serial.print(F("Removing: ")); Serial.println(_fileName);
    SdFile file;
    file.open(_fileName,O_WRITE | O_CREAT);
    delay(1000);
    return file.remove();
    //file.open(_fileName, O_RDWR || O_CREAT);
    
    //return (*file).open(_fileName,O_CREAT);
}

void RRDSD::parseHeaders(int *headerVals)
{
    Serial.println(F("Parsing Headers: "));
    SdFile file;
    file.open(_fileName,O_RDWR | O_CREAT);
    char byteArray[_headersByteEnding];
    if (file.isOpen()){
        file.seekSet(0);
        for(int i=0; i<_headersByteEnding; i++){
            char nextByte = file.read();
            //Serial.print("Parsing: "); Serial.println(nextByte);
            if (nextByte == -1){
                Serial.println("Caught");
                headerVals[0] = -1;
                headerVals[1] = -1;
                headerVals[2] = -1;
                return;
            }
            byteArray[i] = nextByte;
        }
        file.close();
        int intArrayVal = 0;
        int j = 0;
        bool negative = false;
        for (int i=0; i<sizeof(byteArray)/sizeof(byteArray[0]); i++){
            if (byteArray[i] == -1){
                intArrayVal = -1;
            }
            else if (byteArray[i] == '-'){
                negative = true;
            }
            else if (byteArray[i] != '#' && byteArray[i] != '\n' && byteArray[i] != '\r' && byteArray[i] != '\t'){
                //Serial.println(byteArray[i]);
                //Serial.println(byteArray[i]-'0');
                intArrayVal = intArrayVal*10;
                intArrayVal += (byteArray[i]-'0');
            }
            if (i%(_dataLength+2) == _dataLength){
                if (negative) {
                    headerVals[j] = -intArrayVal;
                }
                else {
                    headerVals[j] = intArrayVal;
                }
                intArrayVal = 0;
                negative = false;
                j++;
            }
        }

    }
}

bool RRDSD::updateHeaders()
{
    //Serial.print(F("freeMemory()="));
    //Serial.println(freeMemory());
    Serial.println(F("Updating Headers: "));
    SdFile file;
    file.open(_fileName,O_RDWR | O_CREAT);
    if (file.isOpen()){
        file.seekSet(0);
        int headers[3];
        headers[0] = _start;
        headers[1] = _end;
        headers[2] = _archiveCounter;
        for (int i=0; i<3; i++){
            char line[_dataLength];
            
            int number = abs(headers[i]);
            int n = log10(number) + 1;
            int size = n*sizeof(char);
            if (number != headers[i]){
                size += 1;
            }
            if (headers[i] == 0){
                size = 1;
            }
            //char buffer[size];
            
            sprintf(line, "%d", headers[i]);
            //char* data;
            //data = toArray(headers[i]);
            makeFullRow(line, size, &line[0]);
            //printLine(file, &line[0], size);
            for (int i=0; i<_dataLength; i++){
                //Serial.print("Printing char: ");Serial.println(*(line+i));
                file.print(*(line+i));
            }
            file.println(F(""));
        }
        file.close();
        // file=SD.open(_fileName);
        // if(file){
        //     Serial.println("test.txt:");
        // if(file.available()){
        //     Serial.write(file.read());
        // }
        // file.close();
        // }
        // else {Serial.println("error opening test.txt");}
        return true;
    }
    //Serial.println(F("Failed to open file to update headers"));
    return false;
}
/*
char* RRDSD::toArray(int number)
{
    int n = log10(number) + 1;
    int i;
    char numberArray[n*sizeof(char)];
    for ( i = 0; i < n; ++i, number /= 10 ){
        numberArray[i] = number % 10;
    }
    return numberArray;
}
*/
bool RRDSD::fillEmptyData()
{
    Serial.println(F("Filling w/ empty: "));
    SdFile file;
    file.open(_fileName,O_RDWR | O_CREAT);
    if (file.isOpen()){
        file.seekSet(_headersByteEnding);
        char line[_dataLength];
        
        makeFullRow(line,0,&line[0]);
        while (file.curPosition()<_fileByteEnding){
            Serial.print(F("CurrPos, fileByte: ")); Serial.print(file.curPosition()); Serial.print(" "); Serial.println(_fileByteEnding);
            //printLine(file, &line[0], 0);
            for (int i=0; i<_dataLength-1; i++){
                file.print(*(line+i));
            }
            file.println(*(line+_dataLength-1));
        }
        file.close();
        return true;
    }
    return false;
}

SdFile *RRDSD::getFile()
{
    // File file = SD.open(_fileName);
    // if (file){
    //     while (file.available()){
    //         file.read();
    //     }
    //     file.close();
    //     delay(10);
    // }
    SdFile file;
    Serial.println(file.open(_fileName,O_RDWR || O_CREAT));

    return &file;
}

void RRDSD::makeFullRow(char inputString[], int length, char *outputString)
{
    int i=0;
    while (i < length && i < _dataLength && *(inputString+i)!= '\0'){
        //Serial.print(F("Outputting char for "));Serial.print(i);Serial.print(": ");Serial.println(*(inputString+i));
        outputString[i] = *(inputString+i);
        i++;
    }
    while (i >= length && i < _dataLength){
        //Serial.print(F("Outputting pound for "));Serial.print(i);Serial.print(": ");Serial.println('#');
        outputString[i] = '#';
        i++;
    }
    //outputString[_dataLength+1]='\0';
    //Serial.print(F("Outputting String: ")); Serial.println(outputString);
}

unsigned int RRDSD::indexToByte(unsigned int index)
{
    return index*(_dataLength+2)+_headersByteEnding;
}

unsigned int RRDSD::incrementIndex(unsigned int index)
{
    if (index+1 >= _totalElements){
        return 0;
    }
    return index+1;
}

bool RRDSD::log(char data[], char timestamp[], char *dataBuffer, char *timestampBuffer)
{
    SdFile file;
    file.open(_fileName,O_RDWR | O_CREAT);
    if (file.isOpen()) {
        //Serial.print(F("Logging: "));Serial.print(data);Serial.print(",");Serial.println(timestamp);
        _start = incrementIndex(_start);
        _end = incrementIndex(_end);
        _archiveStart = incrementIndex(_archiveStart);
        _archiveEnd = incrementIndex(_archiveEnd);
        
        unsigned int startByte = indexToByte(_end);
        file.seekSet(startByte);

        //char line[_dataLength];
        char dataAndTimestamp[_dataLength];
        int i=0;
        for (; *(data+i)!='\0'; i++){
            dataAndTimestamp[i] = *(data+i);
        }
        dataAndTimestamp[i]=',';
        i++;
        int dataEnd = i;
        for (int j=0; *(timestamp+j)!='\0'; j++){
            dataAndTimestamp[i] = *(timestamp+j);
            i++;
        }
        //makeFullRow(dataAndTimestamp,i,&dataAndTimestamp[0]);
        printLine(file, &dataAndTimestamp[0], i);
        // for (int j=0; j<_dataLength-1; j++){
        //     file.print(*(dataAndTimestamp+j));
        // }
        // file.println(*(dataAndTimestamp+_dataLength-1));
        //Serial.println("eherre0");
        _archiveCounter++;
        //Serial.println(updateHeaders());
        file.close();
        //Serial.print(F("freeMemory()="));
        //Serial.println(freeMemory());
        //char dataBuffer2[_compareLength];
        //char timestampBuffer2[_dataLength-_compareLength];

        bool archive = attemptToArchive(&dataBuffer[0],&timestampBuffer[0]);
        //strcpy(dataBuffer,dataBuffer2);
        //strcpy(timestampBuffer,timestampBuffer2);
        
        //Serial.print(F("databuffer in Log: ")); Serial.println(dataBuffer);
        //Serial.print(F("timestampbuffer in Log: ")); Serial.println(timestampBuffer);
        //Serial.println(archive);
        updateHeaders();
        return archive;
    }
    else {
        Serial.println(F("Failed to open to log."));
        return false;
    }
}
/*
bool RRDSD::log(char data[], char timestamp[])
{
    File file = SD.open(_fileName, FILE_WRITE);
    if (file) {
        //Serial.print(F("Logging w/ no archive params: "));Serial.print(data);Serial.print(",");Serial.println(timestamp);
        _start = incrementIndex(_start);
        _end = incrementIndex(_end);
        _archiveStart = incrementIndex(_archiveStart);
        _archiveEnd = incrementIndex(_archiveEnd);
        
        unsigned int startByte = indexToByte(_end);
        file.seek(startByte);

        char line[_dataLength];
        char dataAndTimestamp[_dataLength];
        int i=0;
        for (; *(data+i)!='\0'; i++){
            dataAndTimestamp[i] = *(data+i);
        }
        dataAndTimestamp[i]=',';
        i++;
        int dataEnd = i;
        for (int j=0; *(timestamp+j)!='\0'; j++){
            dataAndTimestamp[i] = *(timestamp+j);
            i++;
        }
        makeFullRow(dataAndTimestamp,i,&line[0]);
        for (int j=0; j<_dataLength-1; j++){
            file.print(*(line+j));
        }
        file.println(*(line+_dataLength-1));
        //Serial.println(F("eherre0"));
        //_archiveCounter++;
        //Serial.println(updateHeaders());
        file.close();
        //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
        //char dataBuffer[_compareLength];
        //char timestampBuffer[_dataLength-_compareLength];
        //bool archive = attemptToArchive(dataBuffer,timestampBuffer);
        //Serial.println(archive);
        updateHeaders();
        return true;
    }
    else {
        //Serial.println(F("File failed to open to log."));
        return false;
    }
}
*/
bool RRDSD::attemptToArchive(char *dataBuffer, char *timestampBuffer)
{
    if (_archiveLength > 0){
        if (_archiveCounter >= _archiveElements){
            //Serial.print(F("freeMemory()="));
            //Serial.println(freeMemory());
            SdFile file;
            file.open(_fileName,O_RDWR | O_CREAT);
            if (file.isOpen()) {
                unsigned int archiveStart = indexToByte(_archiveStart);
                file.seekSet(archiveStart);
                char byteArray[_archiveElements*(_dataLength+2)];
                char nextByte;
                int intArray[_archiveElements];
                int intArrayVal = 0;
                int j = 0;
                for(int i=0; i<_archiveElements*(_dataLength+2); i++){
                    nextByte = file.peek();
                    if (nextByte == -1){
                        //file.print('');
                        file.seekSet(_headersByteEnding);
                        nextByte = file.peek();
                        i++;
                    }
                    byteArray[i] = nextByte;

                    if (i%(_dataLength+2) < _compareLength){
                        Serial.println(byteArray[i]);
                        Serial.println((byteArray[i]-'0')*pow(10,_compareLength-(i%(_dataLength+2))-1));
                        intArrayVal+= (byteArray[i]-'0')*pow(10,_compareLength-(i%(_dataLength+2))-1);
                    }
                    else if (i%(_dataLength+2) == _compareLength){
                        intArray[j] = intArrayVal;
                        intArrayVal = 0;
                        j++;
                    }
                    if (nextByte == '\r' || nextByte == '\n'){
                        file.read();
                    }
                    else {
                        file.print('#');
                    }

                }
                
                file.close();
                /*
                char line[_dataLength];
                char data[0];
                makeFullRow(data,0,&line[0]);
                for(int i=0; i<_archiveElements; i++){
                    if (file.position() >= _fileByteEnding){
                        file.seek(_headersByteEnding);
                    }
                    printLine(file, line);
                    // for (int k=0; k<_dataLength-1; k++){
                    //     //Serial.print("Printing char: "); Serial.println(*(line+k));
                    //     file.print(line[k]);
                    // }
                    // file.println(line[_dataLength-1]);
                }
                file.close();
                int intArray[_archiveElements];
                int intArrayVal = 0;
                int j = 0;
                //Serial.println(sizeof(byteArray)/sizeof(byteArray[0]));
                for (int i=0; i<sizeof(byteArray)/sizeof(byteArray[0]); i++){
                    if (i%(_dataLength+2) < _compareLength){
                        //Serial.println(byteArray[i]);
                        //Serial.println((byteArray[i]-'0')*pow(10,_compareLength-(i%(_dataLength+2))-1));
                        intArrayVal+= (byteArray[i]-'0')*pow(10,_compareLength-(i%(_dataLength+2))-1);
                    }
                    else if (i%(_dataLength+2) == _compareLength){
                        intArray[j] = intArrayVal;
                        intArrayVal = 0;
                        j++;
                    }
                }
                */
                int logValue = (*_archiveFunction)(intArray, sizeof(intArray)/sizeof(intArray[0]));
                //Serial.println(intArray[0]);
                //Serial.println(intArray[1]);
                //Serial.println(logValue);
                char buffer[_compareLength];
                sprintf(buffer, "%d", logValue);
                for (int i=0; i<_compareLength; i++){
                    dataBuffer[i] = buffer[i];
                }
                dataBuffer[_compareLength]='\0';
                //Serial.print(F("databuffer in Archive: ")); Serial.println(dataBuffer);

                
                _archiveCounter = 0;
                
                //char timestamp[_dataLength-_compareLength-1];
                for (int i=0; i<_dataLength-_compareLength-1; i++){
                    timestampBuffer[i] = byteArray[i+_compareLength+1];
                }
                timestampBuffer[_dataLength-_compareLength-1]='\0';
                //Serial.print(F("timestampbuffer in Archive: ")); Serial.println(timestampBuffer);
                
                return true;
            }
            //Serial.println(F("Failed to open file to archive"));
            
        }
    }
    
    return false;
}

void RRDSD::printLine(SdFile file, char *line, int size)
{
    makeFullRow(line, size, &line[0]);
    for (int k=0; k<_dataLength-1; k++){
        //Serial.print(F("Printing char: ")); Serial.println(*(line+k));
        file.print(line[k]);
        //file.seekEnd();
        //Serial.print(F("CurrPos, fileByte: ")); Serial.print(file->curPosition()); Serial.print(" "); Serial.println(_fileByteEnding);     
    }
    file.println(line[_dataLength-1]);

}

/*
static int average(int intArray[], int length)
{
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += intArray[i];
    }
    return total/length;
}
  */  
    
