#include <windows.h>
#include <stdio.h>
#include <string.h>

HANDLE hComm;                          // Handle to the Serial port
char   ComPortName[] = "\\\\.\\COM12"; // Name of the Serial port(May Change) to be opened,
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
    setup_serial();

    printf("\n ==========================================\n");

    struct Frame receiveframe;
    struct Frame sendframe;

    char fileReceiveName[30], temp;
    int i = 0;
    while ((temp = get_character()) != 0) {
        fileReceiveName[i++] = temp;
    }

    printf("Sender Send : %s\n", fileReceiveName);

    char fileName[30];
    printf("Save as : ");
    scanf("%s", fileName);
    FILE * pFile;
    pFile = fopen ( fileName , "wb" );
    printf("\n\n");


    while (1)
    {
        printf("wait frame : %d\n", frameNo);

        char temp = get_character();
        memcpy(&receiveframe, &temp, 1);

        for (int i = 0; i < 14; i++) {
            receiveframe.data[i] = get_character();
        }
        receiveframe.data[14] = 0;
        /*
        printf("isData   : %d\n", receiveframe.isData);
        printf("framNo   : %d\n", receiveframe.framNo);
        printf("isAck    : %d\n", receiveframe.isAck);
        printf("size     : %d\n", receiveframe.size);
        printf("\n\n");
        */

        printf("Receive frame : %d\n", receiveframe.framNo);
        printf("Data          : %s\n", receiveframe.data);
        printf("Action frame  : ");

        char in;
        in = getch();
        putchar(in);
        putchar('\n');

        BOOL isStore = FALSE;

        if (in == 'n'&& frameNo == receiveframe.framNo) {
            isStore = TRUE;
        } else if (in == 'a' && frameNo == receiveframe.framNo) {
            isStore = TRUE;
        }

        if (isStore) {
            frameNo = !frameNo;
            fwrite (receiveframe.data , 1, receiveframe.size == 15 ? 14 : receiveframe.size, pFile);
            // memcpy(&SerialBuffer[iSend], receiveframe.data, receiveframe.size == 15 ? 14 : receiveframe.size);
            iSend += receiveframe.size == 15 ? 14 : receiveframe.size;

            SerialBuffer[iSend + 1] = 0;

            if (receiveframe.size != 15) {
                // printf("\n --- %s --- \n", SerialBuffer);
                sendframe.isData = FALSE;
                sendframe.framNo = frameNo;
                sendframe.isAck = TRUE;
                char send = 0;
                memcpy(&send, &sendframe, 1);

                send_character(send);
                break;
            }
        }

        if (in == 'a') {
            if (!frameNo == receiveframe.framNo) {
                printf("Received & Send ACK%d\n", frameNo);
            } else {
                printf("Reject & Send ACK%d\n", frameNo);
            }

            sendframe.isData = FALSE;
            sendframe.framNo = frameNo;
            sendframe.isAck = TRUE;
            char send = 0;
            memcpy(&send, &sendframe, 1);

            send_character(send);
        } else if (in == 'r') {
            printf("reject & Sleep\n");
        } else if (in == 'n') {
            printf("Received & Sleep\n");
        }

        printf("\n\n");


        //Sleep(1000);
    }

    CloseHandle(hComm);//Closing the Serial Port
    fclose(pFile);
    printf("\n ==========================================\n");
    //_getch();
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
