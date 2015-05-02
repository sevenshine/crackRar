#include "rar.hpp"

CommandData::CommandData()
{
  FileArgs=ExclArgs=InclArgs=StoreArgs=ArcNames=NULL;
  Init();
}


CommandData::~CommandData()
{
  Close();
}


void CommandData::Init()
{
  Close();

  *Command=0;
  *ArcName=0;
  *ArcNameW=0;
  FileLists=false;
  NoMoreSwitches=false;
  TimeConverted=false;

  FileArgs=new StringList;
  ExclArgs=new StringList;
  InclArgs=new StringList;
  StoreArgs=new StringList;
  ArcNames=new StringList;
}


void CommandData::Close()
{
  delete FileArgs;
  delete ExclArgs;
  delete InclArgs;
  delete StoreArgs;
  delete ArcNames;
  FileArgs=ExclArgs=InclArgs=StoreArgs=ArcNames=NULL;
  NextVolSizes.Reset();
}


#if !defined(SFX_MODULE)
void CommandData::ParseArg(char *Arg,wchar *ArgW)
{
  if (IsSwitch(*Arg) && !NoMoreSwitches)
    if (Arg[1]=='-')
      NoMoreSwitches=true;
    else
      ProcessSwitch(&Arg[1],(ArgW!=NULL && *ArgW!=0 ? &ArgW[1]:NULL));
  else
    if (*Command==0)
    {
      strncpyz(Command,Arg,ASIZE(Command));
      if (ArgW!=NULL)
        strncpyw(CommandW,ArgW,sizeof(CommandW)/sizeof(CommandW[0]));
      if (etoupper(*Command)=='S')
      {
        const char *SFXName=Command[1] ? Command+1:DefSFXName;
        if (PointToName(SFXName)!=SFXName || FileExist(SFXName))
          strcpy(SFXModule,SFXName);
        else
          GetConfigName(SFXName,SFXModule,true);
      }
#ifndef GUI
      *Command=etoupper(*Command);
      if (*Command!='I' && *Command!='S')
        strupper(Command);
#endif
    }
    else
      if (*ArcName==0)
      {
        strncpyz(ArcName,Arg,ASIZE(ArcName));
        if (ArgW!=NULL)
          strncpyzw(ArcNameW,ArgW,ASIZE(ArcNameW));
      }
      else
      {
        size_t Length=strlen(Arg);
        char EndChar=Length==0 ? 0:Arg[Length-1];
        char CmdChar=etoupper(*Command);
        bool Add=strchr("AFUM",CmdChar)!=NULL;
        bool Extract=CmdChar=='X' || CmdChar=='E';
        if ((IsDriveDiv(EndChar) || IsPathDiv(EndChar)) && !Add)
        {
          strncpyz(ExtrPath,Arg,ASIZE(ExtrPath));
          if (ArgW!=NULL)
            strncpyzw(ExtrPathW,ArgW,ASIZE(ExtrPathW));
        }
        else
          if ((Add || CmdChar=='T') && *Arg!='@')
            FileArgs->AddString(Arg);
          else
          {
            struct FindData FileData;
            bool Found=FindFile::FastFind(Arg,NULL,&FileData);
            if (!Found && *Arg=='@' && !IsWildcard(Arg))
            {
              FileLists=true;

              RAR_CHARSET Charset=FilelistCharset;

#if defined(_WIN_32) && !defined(GUI)
              // for compatibility reasons we use OEM encoding
              // in Win32 console version by default

              if (Charset==RCH_DEFAULT)
                Charset=RCH_OEM;
#endif

              ReadTextFile(Arg+1,FileArgs,false,true,Charset,true,true,true);
            }
            else
              if (Found && FileData.IsDir && Extract && *ExtrPath==0)
              {
                strcpy(ExtrPath,Arg);
                AddEndSlash(ExtrPath);
              }
              else
                FileArgs->AddString(Arg);
          }
      }
}
#endif


void CommandData::ParseDone()
{
  if (FileArgs->ItemsCount()==0 && !FileLists)
    FileArgs->AddString(MASKALL);
  char CmdChar=etoupper(*Command);
  bool Extract=CmdChar=='X' || CmdChar=='E';
  if (Test && Extract)
    Test=false;
  BareOutput=(CmdChar=='L' || CmdChar=='V') && Command[1]=='B';
}


#if !defined(SFX_MODULE) && !defined(_WIN_CE)
void CommandData::ParseEnvVar()
{
  char *EnvStr=getenv("RAR");
  if (EnvStr!=NULL)
    ProcessSwitchesString(EnvStr);
}
#endif



// return 'false' if -cfg- is present and preprocess switches
// which must be processed before the rest of command line

#ifndef SFX_MODULE
bool CommandData::IsConfigEnabled(int argc,char *argv[])
{
  bool ConfigEnabled=true;
  for (int I=1;I<argc;I++)
    if (IsSwitch(*argv[I]))
    {
      if (stricomp(&argv[I][1],"cfg-")==0)
        ConfigEnabled=false;
#ifndef GUI
      if (strnicomp(&argv[I][1],"ilog",4)==0)
      {
        // ensure that correct log file name is already set
        // if we need to report an error when processing the command line
        ProcessSwitch(&argv[I][1]);
        InitLogOptions(LogName);
      }
#endif
      if (strnicomp(&argv[I][1],"sc",2)==0)
      {
        // process -cs before reading any file lists
        ProcessSwitch(&argv[I][1]);
      }
    }
  return(ConfigEnabled);
}
#endif


#if !defined(GUI) && !defined(SFX_MODULE)
void CommandData::ReadConfig(int argc,char *argv[])
{
  StringList List;
  if (ReadTextFile(DefConfigName,&List,true))
  {
    char *Str;
    while ((Str=List.GetString())!=NULL)
    {
      while (isspace(*Str))
        Str++;
      if (strnicomp(Str,"switches=",9)==0)
        ProcessSwitchesString(Str+9);
    }
  }
}
#endif


#if !defined(SFX_MODULE) && !defined(_WIN_CE)
void CommandData::ProcessSwitchesString(char *Str)
{
  while (*Str)
  {
    while (!IsSwitch(*Str) && *Str!=0)
      Str++;
    if (*Str==0)
      break;
    char *Next=Str;
    while (!(Next[0]==' ' && IsSwitch(Next[1])) && *Next!=0)
      Next++;
    char NextChar=*Next;
    *Next=0;
    ProcessSwitch(Str+1);
    *Next=NextChar;
    Str=Next;
  }
}
#endif


#if !defined(SFX_MODULE)
void CommandData::ProcessSwitch(char *Switch,wchar *SwitchW)
{
	if(etoupper(Switch[0])=='P')
		{

		  if (Switch[1]==0)
		  {
			GetPassword(PASSWORD_GLOBAL,NULL,Password,sizeof(Password));
			eprintf("\n");
			printf("test1\n");
		  }
		  else
		  {
			strncpyz(Password,Switch+1,ASIZE(Password));
			printf("test2\n");
		  }
		}
		else
			printf("p default\n");


}
#endif


#ifndef SFX_MODULE
void CommandData::BadSwitch(char *Switch)
{
  mprintf(St(MUnknownOption),Switch);
  ErrHandler.Exit(USER_ERROR);
}
#endif


#ifndef GUI
void CommandData::OutTitle()
{
  if (BareOutput || DisableCopyright)
    return;
#if defined(__GNUC__) && defined(SFX_MODULE)
  mprintf(St(MCopyrightS));
#else
#ifndef SILENT
  static bool TitleShown=false;
  if (TitleShown)
    return;
  TitleShown=true;
  char Version[50];
  int Beta=RARVER_BETA;
  if (Beta!=0)
    sprintf(Version,"%d.%02d %s %d",RARVER_MAJOR,RARVER_MINOR,St(MBeta),RARVER_BETA);
  else
    sprintf(Version,"%d.%02d",RARVER_MAJOR,RARVER_MINOR);
#ifdef UNRAR
  mprintf(St(MUCopyright),Version,RARVER_YEAR);
#else
#endif
#endif
#endif
}
#endif


inline bool CmpMSGID(MSGID i1,MSGID i2)
{
#ifdef MSGID_INT
  return(i1==i2);
#else
  // If MSGID is const char*, we cannot compare pointers only.
  // Pointers to different instances of same strings can differ,
  // so we need to compare complete strings.
  return(strcmp(i1,i2)==0);
#endif
}

void CommandData::OutHelp()
{
#if !defined(GUI) && !defined(SILENT)
  OutTitle();
  static MSGID Help[]={
#ifdef SFX_MODULE
    MCHelpCmd,MSHelpCmdE,MSHelpCmdT,MSHelpCmdV
#elif defined(UNRAR)
    MUNRARTitle1,MRARTitle2,MCHelpCmd,MCHelpCmdE,MCHelpCmdL,
    MCHelpCmdP,MCHelpCmdT,MCHelpCmdV,MCHelpCmdX,MCHelpSw,
    MCHelpSwm,MCHelpSwAC,MCHelpSwAD,MCHelpSwAP,
    MCHelpSwCm,MCHelpSwCFGm,MCHelpSwCL,MCHelpSwCU,
    MCHelpSwDH,MCHelpSwEP,MCHelpSwEP3,MCHelpSwF,MCHelpSwIDP,MCHelpSwIERR,
    MCHelpSwINUL,MCHelpSwIOFF,MCHelpSwKB,MCHelpSwN,MCHelpSwNa,MCHelpSwNal,
    MCHelpSwO,MCHelpSwOC,MCHelpSwOR,MCHelpSwOW,MCHelpSwP,
    MCHelpSwPm,MCHelpSwR,MCHelpSwRI,MCHelpSwSL,MCHelpSwSM,MCHelpSwTA,
    MCHelpSwTB,MCHelpSwTN,MCHelpSwTO,MCHelpSwTS,MCHelpSwU,MCHelpSwVUnr,
    MCHelpSwVER,MCHelpSwVP,MCHelpSwX,MCHelpSwXa,MCHelpSwXal,MCHelpSwY
#else
    MRARTitle1,MRARTitle2,MCHelpCmd,MCHelpCmdA,MCHelpCmdC,MCHelpCmdCF,
    MCHelpCmdCH,MCHelpCmdCW,MCHelpCmdD,MCHelpCmdE,MCHelpCmdF,MCHelpCmdI,
    MCHelpCmdK,MCHelpCmdL,MCHelpCmdM,MCHelpCmdP,MCHelpCmdR,MCHelpCmdRC,
    MCHelpCmdRN,MCHelpCmdRR,MCHelpCmdRV,MCHelpCmdS,MCHelpCmdT,MCHelpCmdU,
    MCHelpCmdV,MCHelpCmdX,MCHelpSw,MCHelpSwm,MCHelpSwAC,MCHelpSwAD,MCHelpSwAG,
    MCHelpSwAO,MCHelpSwAP,MCHelpSwAS,MCHelpSwAV,MCHelpSwAVm,MCHelpSwCm,
    MCHelpSwCFGm,MCHelpSwCL,MCHelpSwCU,MCHelpSwDF,MCHelpSwDH,MCHelpSwDR,
    MCHelpSwDS,MCHelpSwDW,MCHelpSwEa,MCHelpSwED,MCHelpSwEE,MCHelpSwEN,
    MCHelpSwEP,MCHelpSwEP1,MCHelpSwEP2,MCHelpSwEP3,MCHelpSwF,MCHelpSwHP,
    MCHelpSwIDP,MCHelpSwIEML,MCHelpSwIERR,MCHelpSwILOG,MCHelpSwINUL,
    MCHelpSwIOFF,MCHelpSwISND,MCHelpSwK,MCHelpSwKB,MCHelpSwMn,MCHelpSwMC,
    MCHelpSwMD,MCHelpSwMS,MCHelpSwMT,MCHelpSwN,MCHelpSwNa,MCHelpSwNal,
    MCHelpSwO,MCHelpSwOC,MCHelpSwOL,MCHelpSwOR,MCHelpSwOS,MCHelpSwOW,
    MCHelpSwP,MCHelpSwPm,MCHelpSwR,MCHelpSwR0,MCHelpSwRI,MCHelpSwRR,
    MCHelpSwRV,MCHelpSwS,MCHelpSwSm,MCHelpSwSC,MCHelpSwSFX,MCHelpSwSI,
    MCHelpSwSL,MCHelpSwSM,MCHelpSwT,MCHelpSwTA,MCHelpSwTB,MCHelpSwTK,
    MCHelpSwTL,MCHelpSwTN,MCHelpSwTO,MCHelpSwTS,MCHelpSwU,MCHelpSwV,
    MCHelpSwVn,MCHelpSwVD,MCHelpSwVER,MCHelpSwVN,MCHelpSwVP,MCHelpSwW,
    MCHelpSwX,MCHelpSwXa,MCHelpSwXal,MCHelpSwY,MCHelpSwZ
#endif
  };

  for (int I=0;I<sizeof(Help)/sizeof(Help[0]);I++)
  {
#ifndef SFX_MODULE
#ifdef DISABLEAUTODETECT
    if (Help[I]==MCHelpSwV)
      continue;
#endif
#ifndef _WIN_32
    static MSGID Win32Only[]={
      MCHelpSwIEML,MCHelpSwVD,MCHelpSwAO,MCHelpSwOS,MCHelpSwIOFF,
      MCHelpSwEP2,MCHelpSwOC,MCHelpSwDR,MCHelpSwRI
    };
    bool Found=false;
    for (int J=0;J<sizeof(Win32Only)/sizeof(Win32Only[0]);J++)
      if (CmpMSGID(Help[I],Win32Only[J]))
      {
        Found=true;
        break;
      }
    if (Found)
      continue;
#endif
#if !defined(_UNIX) && !defined(_WIN_32)
    if (CmpMSGID(Help[I],MCHelpSwOW))
      continue;
#endif
#if !defined(_WIN_32) && !defined(_EMX)
    if (CmpMSGID(Help[I],MCHelpSwAC))
      continue;
#endif
#ifndef SAVE_LINKS
    if (CmpMSGID(Help[I],MCHelpSwOL))
      continue;
#endif
#ifndef PACK_SMP
    if (CmpMSGID(Help[I],MCHelpSwMT))
      continue;
#endif
#ifndef _BEOS
    if (CmpMSGID(Help[I],MCHelpSwEE))
    {
#if defined(_EMX) && !defined(_DJGPP)
      if (_osmode != OS2_MODE)
        continue;
#else
      continue;
#endif
    }
#endif
#endif
    mprintf(St(Help[I]));
  }
  mprintf("\n");
  ErrHandler.Exit(USER_ERROR);
#endif
}


bool CommandData::ExclCheckArgs(StringList *Args,char *CheckName,bool CheckFullPath,int MatchMode)
{
  char *Name=ConvertPath(CheckName,NULL);
  char FullName[NM],*CurName;
  *FullName=0;
  Args->Rewind();
  while ((CurName=Args->GetString())!=NULL)
#ifndef SFX_MODULE
    if (CheckFullPath && IsFullPath(CurName))
    {
      if (*FullName==0)
        ConvertNameToFull(CheckName,FullName);
      if (CmpName(CurName,FullName,MatchMode))
        return(true);
    }
    else
#endif
      if (CmpName(ConvertPath(CurName,NULL),Name,MatchMode))
        return(true);
  return(false);
}


bool CommandData::ExclCheck(char *CheckName,bool CheckFullPath)
{
  if (ExclCheckArgs(ExclArgs,CheckName,CheckFullPath,MATCH_WILDSUBPATH))
    return(true);
  if (InclArgs->ItemsCount()==0)
    return(false);
  if (ExclCheckArgs(InclArgs,CheckName,false,MATCH_WILDSUBPATH))
    return(false);
  return(true);
}




#ifndef SFX_MODULE
bool CommandData::TimeCheck(RarTime &ft)
{
  if (FileTimeBefore.IsSet() && ft>=FileTimeBefore)
    return(true);
  if (FileTimeAfter.IsSet() && ft<=FileTimeAfter)
    return(true);
/*
  if (FileTimeOlder!=0 || FileTimeNewer!=0)
  {
    if (!TimeConverted)
    {
      if (FileTimeOlder!=0)
        FileTimeOlder=SecondsToDosTime(FileTimeOlder);
      if (FileTimeNewer!=0)
        FileTimeNewer=SecondsToDosTime(FileTimeNewer);
      TimeConverted=true;
    }
    if (FileTimeOlder!=0 && ft>=FileTimeOlder)
      return(true);
    if (FileTimeNewer!=0 && ft<=FileTimeNewer)
      return(true);

  }
*/
  return(false);
}
#endif


#ifndef SFX_MODULE
bool CommandData::SizeCheck(Int64 Size)
{
  if (FileSizeLess!=INT64ERR && Size>=FileSizeLess)
    return(true);
  if (FileSizeMore!=INT64ERR && Size<=FileSizeMore)
    return(true);
  return(false);
}
#endif


int CommandData::IsProcessFile(FileHeader &NewLhd,bool *ExactMatch,int MatchType)
{
  if (strlen(NewLhd.FileName)>=NM || strlenw(NewLhd.FileNameW)>=NM)
    return(0);
  if (ExclCheck(NewLhd.FileName,false))
    return(0);
#ifndef SFX_MODULE
  if (TimeCheck(NewLhd.mtime))
    return(0);
  if ((NewLhd.FileAttr & ExclFileAttr)!=0 || InclAttrSet && (NewLhd.FileAttr & InclFileAttr)==0)
    return(0);
  if ((NewLhd.Flags & LHD_WINDOWMASK)!=LHD_DIRECTORY && SizeCheck(NewLhd.FullUnpSize))
    return(0);
#endif
  char *ArgName;
  wchar *ArgNameW;
  FileArgs->Rewind();
  for (int StringCount=1;FileArgs->GetString(&ArgName,&ArgNameW);StringCount++)
  {
#ifndef SFX_MODULE
    bool Unicode=(NewLhd.Flags & LHD_UNICODE) || ArgNameW!=NULL;
    if (Unicode)
    {
      wchar NameW[NM],ArgW[NM],*NamePtr=NewLhd.FileNameW;
      bool CorrectUnicode=true;
      if (ArgNameW==NULL)
      {
        if (!CharToWide(ArgName,ArgW) || *ArgW==0)
          CorrectUnicode=false;
        ArgNameW=ArgW;
      }
      if ((NewLhd.Flags & LHD_UNICODE)==0)
      {
        if (!CharToWide(NewLhd.FileName,NameW) || *NameW==0)
          CorrectUnicode=false;
        NamePtr=NameW;
      }
      if (CmpName(ArgNameW,NamePtr,MatchType))
      {
        if (ExactMatch!=NULL)
          *ExactMatch=stricompcw(ArgNameW,NamePtr)==0;
        return(StringCount);
      }
      if (CorrectUnicode)
        continue;
    }
#endif
    if (CmpName(ArgName,NewLhd.FileName,MatchType))
    {
      if (ExactMatch!=NULL)
        *ExactMatch=stricompc(ArgName,NewLhd.FileName)==0;
      return(StringCount);
    }
  }
  return(0);
}


#ifndef GUI
void CommandData::ProcessCommand()
{
#ifndef SFX_MODULE
  const char *SingleCharCommands="FUADPXETK";
  if (Command[1] && strchr(SingleCharCommands,*Command)!=NULL || *ArcName==0)
    OutHelp();

  	  //get filename *ArcName
#ifdef _UNIX
  if (GetExt(ArcName)==NULL && (!FileExist(ArcName) || IsDir(GetFileAttr(ArcName))))
    strcat(ArcName,".rar");
#endif

  	  //printf("test4  %c  %s\n",*Command,ArcName);
  	  //找文件，将ArcName传到FindData.Name
  if (strchr("AFUMD",*Command)==NULL)
  {
    StringList ArcMasks;
    ArcMasks.AddString(ArcName);
    ScanTree Scan(&ArcMasks,Recurse,SaveLinks,SCAN_SKIPDIRS);
    FindData FindData;
    while (Scan.GetNext(&FindData)==SCAN_SUCCESS)
      AddArcName(FindData.Name,FindData.NameW);		//FindData.Name 	"ere.rar"
  }
#endif

  switch(Command[0])
  {
    case 'P':
    case 'X':
    case 'E':
    case 'T':
    case 'I':
      {
    	  printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        CmdExtract Extract;
        Extract.DoExtract(this);
        printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      }
      break;
#ifndef SILENT
    case 'V':
    case 'L':
      ListArchive(this);
      break;
    default:
      OutHelp();
#endif
  }
  if (!BareOutput)
    mprintf("\n");
}
#endif


void CommandData::AddArcName(char *Name,wchar *NameW)
{
  ArcNames->AddString(Name,NameW);
}


bool CommandData::GetArcName(char *Name,wchar *NameW,int MaxSize)
{
  if (!ArcNames->GetString(Name,NameW,NM))
    return(false);
  return(true);
}


bool CommandData::IsSwitch(int Ch)
{
#if defined(_WIN_32) || defined(_EMX)
  return(Ch=='-' || Ch=='/');
#else
  return(Ch=='-');
#endif
}


#ifndef SFX_MODULE
uint CommandData::GetExclAttr(char *Str)
{
  if (isdigit(*Str))
    return(strtol(Str,NULL,0));
  else
  {
    uint Attr;
    for (Attr=0;*Str;Str++)
      switch(etoupper(*Str))
      {
#ifdef _UNIX
        case 'D':
          Attr|=S_IFDIR;
          break;
        case 'V':
          Attr|=S_IFCHR;
          break;
#elif defined(_WIN_32) || defined(_EMX)
        case 'R':
          Attr|=0x1;
          break;
        case 'H':
          Attr|=0x2;
          break;
        case 'S':
          Attr|=0x4;
          break;
        case 'D':
          Attr|=0x10;
          break;
        case 'A':
          Attr|=0x20;
          break;
#endif
      }
    return(Attr);
  }
}
#endif




#ifndef SFX_MODULE
bool CommandData::CheckWinSize()
{
  static int ValidSize[]={
    0x10000,0x20000,0x40000,0x80000,0x100000,0x200000,0x400000
  };
  for (int I=0;I<sizeof(ValidSize)/sizeof(ValidSize[0]);I++)
    if (WinSize==ValidSize[I])
      return(true);
  WinSize=0x400000;
  return(false);
}
#endif
