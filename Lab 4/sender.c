#include <Windows.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

HANDLE hComm;                          // Handle to the Serial port
char   ComPortName[] = "\\\\.\\COM11"; // Name of the Serial port(May Change) to be opened,
BOOL   Status;
DWORD dwEventMask;
DWORD NoBytesRead;

void setup_serial(void);
void send_string(char lpBuffer[], DWORD dNoOFBytestoWrite);
void send_character (char ch);
char get_character(void);

char  SerialBuffer[256];
int iSend = 0;

BOOL frameNo = 0;

struct Frame {
    unsigned char isData    : 1;
    unsigned char framNo    : 1;
    unsigned char isAck     : 1;
    unsigned char isEnd     : 1;
    unsigned char size      : 4;
    char data[15];
};

void main(void)
{
    //char dataToSend[] = "12345678901234abcdefghijabcdABCDEFGHIJABCD!@#$%^&*()!@#$,./;'[]";

    setup_serial();

    printf("\n ==========================================\n");

    struct Frame sendframe;
    struct Frame receiveframe;

    /// send file name

    char fileName[30];
    printf("Send file : ");
    scanf("%s", fileName);
    FILE * pFile;
    pFile = fopen ( fileName , "rb" );
    printf("\n\n");

    fseek(pFile, 0L, SEEK_END);
    int len = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
/*
    sendframe.isData = 0;
    sendframe.isAck = 0;
    sendframe.framNo = 0;
    sendframe.isEnd = 0;
    sendframe.size = 0;

    char out = 0;
    memcpy(&out, &sendframe, 1);

    send_character(out);*/
    send_string(fileName, strlen(fileName));
    send_character(0);

    /// end send file name

    while (iSend < len)
    {
        sendframe.isData = TRUE;
        sendframe.isAck = FALSE;
        sendframe.framNo = frameNo;
        sendframe.isEnd = FALSE;
        sendframe.size = len - iSend >= 15 ? 15 : len - iSend;

        memset(sendframe.data, 0, 14);
        fread(sendframe.data, 1, len - iSend >= 14 ? 14 : len - iSend, pFile);
        fseek(pFile, -(len - iSend >= 14 ? 14 : len - iSend), SEEK_CUR);
        // memcpy(sendframe.data, &dataToSend[iSend], len - iSend >= 14 ? 14 : len - iSend);

        unsigned char buf[16];
        memcpy(buf, &sendframe, 15);
        buf[15] = 0;

        send_string(buf, 15);

        printf("Send frame : %d\n", frameNo);
        printf("Data       : %s\n", &buf[1]);
        printf("Timeout    : ");
        char in;
        in = getch();
        putchar(in);
        putchar('\n');

        if (in != 't') {
            char receive;
            receive = get_character();
            memcpy(&receiveframe, &receive, 1);
        }

        if (receiveframe.framNo == sendframe.framNo || in == 't') {
            printf("Retransmit frame\n", frameNo);
        } else {
            printf("Receive ACK%d\n", receiveframe.framNo);
            frameNo = receiveframe.framNo;
            fseek(pFile, len - iSend >= 14 ? 14 : len - iSend, SEEK_CUR);
            iSend += len - iSend >= 14 ? 14 : len - iSend;
        }

        printf("\n\n");

        // Sleep(5000);
    }

    CloseHandle(hComm);//Closing the Serial Port
    fclose(pFile);
    printf("\n ==========================================\n");
    _getch();
}

void setup_serial(void)
{
    printf("\n\n +==========================================+");
    printf("\n |    Serial Port  Reception (Win32 API)    |");
    printf("\n +==========================================+\n");
    /*---------------------------------- Opening the Serial Port -------------------------------------------*/

    hComm = CreateFile( ComPortName,                  // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
                        0,                            // No Sharing, ports cant be shared
                        NULL,                         // No Security
                        OPEN_EXISTING,                // Open existing port only
                        0,                            // Non Overlapped I/O
                        NULL);                        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("\n    Error! - Port %s can't be opened\n", ComPortName);
    else
        printf("\n    Port %s Opened\n ", ComPortName);

    /*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

    DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

    if (Status == FALSE)
        printf("\n    Error! in GetCommState()");

    dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None

    Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

    if (Status == FALSE)
    {
        printf("\n    Error! in Setting DCB Structure");
    }
    else //If Successfull display the contents of the DCB Structure
    {
        printf("\n\n    Setting DCB Structure Successfull\n");
        printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }

    /*------------------------------------ Setting Timeouts --------------------------------------------------*/

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(hComm, &timeouts) == FALSE)
        printf("\n\n    Error! in Setting Time Outs");
    else
        printf("\n\n    Setting Serial Port Timeouts Successfull");

    /*------------------------------------ Setting Receive Mask ----------------------------------------------*/

    Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

    if (Status == FALSE)
        printf("\n\n    Error! in Setting CommMask");
    else
        printf("\n\n    Setting CommMask successfull");
}

void send_string(char lpBuffer[], DWORD dNoOFBytestoWrite)
{
    DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port

    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);
}

void send_character (char ch)
{
    DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port

    Status = WriteFile(hComm,               // Handle to the Serialport
                       &ch,            // Data to be written to the port
                       sizeof(ch),   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);
}

char get_character(void) {
    char lpBuf;

    if (NoBytesRead == 0)
        Status = WaitCommEvent(hComm, &dwEventMask, NULL);

    if (Status == FALSE) {
        printf("\n    Error! in Setting WaitCommEvent()");
        return 0;
    } else {
        ReadFile(hComm, &lpBuf, 1, &NoBytesRead, NULL);
        if (NoBytesRead == 0) {
            return get_character();
        } else {
            return lpBuf;
        }
    }
}
