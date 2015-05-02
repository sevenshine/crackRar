#include "rar.hpp"
#include "smallfn.cpp"


int main(int argc, char *argv[])
{
#ifdef _UNIX
  setlocale(LC_ALL,"");

#endif
printf("ccc\n");

  ErrHandler.SetSignalHandlers(true);

  RARInitData();


#ifdef ALLOW_EXCEPTIONS
  try
#endif
  {

    CommandData Cmd;

    if (Cmd.IsConfigEnabled(argc,argv))
    {
      Cmd.ReadConfig(argc,argv);
      Cmd.ParseEnvVar();
    }
    for (int I=1;I<argc;I++)
      Cmd.ParseArg(argv[I],NULL);	//argue -p1234	read to *Password

    Cmd.ParseDone();

    //无用？
    /*InitConsoleOptions(Cmd.MsgStream,Cmd.Sound);
    InitLogOptions(Cmd.LogName);
    ErrHandler.SetSilent(Cmd.AllYes || Cmd.MsgStream==MSG_NULL);
    ErrHandler.SetShutdown(Cmd.Shutdown);*/

    //output copyright info
    Cmd.OutTitle();
    Cmd.ProcessCommand();
  }
#ifdef ALLOW_EXCEPTIONS
  catch (int ErrCode)
  {
    ErrHandler.SetErrorCode(ErrCode);
  }
#ifdef ENABLE_BAD_ALLOC
  catch (bad_alloc)
  {
    ErrHandler.SetErrorCode(MEMORY_ERROR);
  }
#endif
  catch (...)
  {
    ErrHandler.SetErrorCode(FATAL_ERROR);
  }
#endif
  File::RemoveCreated();

  return(ErrHandler.GetErrorCode());
}



