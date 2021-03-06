#ifndef _RAR_DATAIO_
#define _RAR_DATAIO_

class CmdAdd;
class Unpack;


class ComprDataIO
{
  private:
    void ShowUnpRead(Int64 ArcPos,Int64 ArcSize);
    void ShowUnpWrite();


    bool UnpackFromMemory;
    uint UnpackFromMemorySize;
    byte *UnpackFromMemoryAddr;

    bool UnpackToMemory;
    uint UnpackToMemorySize;
    byte *UnpackToMemoryAddr;

    uint UnpWrSize;
    byte *UnpWrAddr;

    Int64 UnpPackedSize;

    bool ShowProgress;
    bool TestMode;
    bool SkipUnpCRC;

    File *SrcFile;
    File *DestFile;

    CmdAdd *Command;

    FileHeader *SubHead;
    Int64 *SubHeadPos;

#ifndef NOCRYPT
    CryptData Crypt;
    CryptData Decrypt;
#endif


    int LastPercent;

    char CurrentCommand;

  public:
    ComprDataIO();
    void Init();
    int UnpRead(byte *Addr,uint Count);
    void UnpWrite(byte *Addr,uint Count);
    void EnableShowProgress(bool Show) {ShowProgress=Show;}
    void GetUnpackedData(byte **Data,uint *Size);
    void SetPackedSizeToRead(Int64 Size) {UnpPackedSize=Size;}
    void SetTestMode(bool Mode) {TestMode=Mode;}
    void SetSkipUnpCRC(bool Skip) {SkipUnpCRC=Skip;}
    void SetFiles(File *SrcFile,File *DestFile);
    void SetCommand(CmdAdd *Cmd) {Command=Cmd;}
    void SetSubHeader(FileHeader *hd,Int64 *Pos) {SubHead=hd;SubHeadPos=Pos;}
    void SetEncryption(int Method,char *Password,byte *Salt,bool Encrypt,bool HandsOffHash);
    void SetAV15Encryption();
    void SetCmt13Encryption();
    void SetUnpackToMemory(byte *Addr,uint Size);
    void SetCurrentCommand(char Cmd) {CurrentCommand=Cmd;}

    bool PackVolume;
    bool UnpVolume;
    bool NextVolumeMissing;
    Int64 TotalPackRead;
    Int64 UnpArcSize;
    Int64 CurPackRead,CurPackWrite,CurUnpRead,CurUnpWrite;

    // Size of already processed archives.
    // Used to calculate the total operation progress.
    Int64 ProcessedArcSize;

    Int64 TotalArcSize;

    uint PackFileCRC,UnpFileCRC,PackedCRC;

    int Encryption;
    int Decryption;
};

#endif
