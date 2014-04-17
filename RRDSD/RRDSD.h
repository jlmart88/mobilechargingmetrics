//
//  RRDSD.h
//  
//
//  Created by Justin Martinez on 2/28/14.
//
//

#ifndef _RRDSD_h
#define _RRDSD_h

#include "Arduino.h"
#include "SdFat.h"

class RRDSD
{
    public:
        RRDSD(char fileName[], unsigned long interval, unsigned long length, int dataLength, int compareLength, RRDSD *archive = NULL, int (*archiveFunction)(int*,int) = NULL);
        RRDSD();
        //bool canLog();
        bool log(char data[], char timestamp[], char *dataBuffer, char *timestampBuffer);
        //bool log(char data[], char timestamp[]);
        unsigned long getLength();
        //unsigned long getArchiveLength();
        unsigned long getInterval();
        bool resetFile();
        //static int average(int intArray[],int length);
    private:
        char* _fileName;
        unsigned long _interval;
        unsigned long _length;
        int _dataLength;
        int _compareLength;
        RRDSD *_archive;
        int (*_archiveFunction)(int*, int);
        // try changing these longs to ints, see how much memory that saves
        unsigned int _start;
        unsigned int _end;
        unsigned int _archiveStart;
        unsigned int _archiveEnd;
        int _archiveCounter;
        unsigned long _archiveLength;
        int _archiveElements;
        unsigned long _totalLength;
        unsigned int _totalElements;
        unsigned int _headersByteEnding;
        unsigned int _fileByteEnding;
    
        SdFile *getFile();
        bool initializeFile();
        bool clearFile();
        void parseHeaders(int *headerVals);
        //bool cleanDatabase();
        bool updateHeaders();
        //char* toArray(int number);
        bool fillEmptyData();
        void makeFullRow(char inputString[], int length, char *outputString);
        unsigned int indexToByte(unsigned int index);
        unsigned int incrementIndex(unsigned int index);
        bool attemptToArchive(char *dataBuffer, char *timestampBuffer);
        void printLine(SdFile file, char *line, int size);
    
        
};
#endif
