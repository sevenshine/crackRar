#include "rar.hpp"
#include "smallfn.cpp"

int crackRar(char *str,char *password)
{
  setlocale(LC_ALL,"");

  ErrHandler.SetSignalHandlers(true);

  RARInitData();

#ifdef ALLOW_EXCEPTIONS
  try
#endif
  {
    CommandData Cmd;

    int a=5;
    char ss[10]="-p";
    strcat(ss,password);
    char *b[]={" ","t","-y",ss,str};

    if (Cmd.IsConfigEnabled(a,b))
    {
       Cmd.ReadConfig(a,b);
       Cmd.ParseEnvVar();
    }
    for (int I=1;I<a;I++)
       Cmd.ParseArg(b[I],NULL);	//argue -p1234	read to *Password

    Cmd.ParseDone();

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

int main(int argc, char *argv[])
{
	char *password="100";
	char *str="test.rar";
	crackRar(str,password);
	return 0;



#ifdef _UNIX
  setlocale(LC_ALL,"");

#endif
//printf("ccc\n");

  ErrHandler.SetSignalHandlers(true);

  RARInitData();


#ifdef ALLOW_EXCEPTIONS
  try
#endif
  {

    CommandData Cmd;

    int a=5;
    char *b[]={" ","t","-y","-p100","test1.rar"};

    if (Cmd.IsConfigEnabled(a,b))
        {
          Cmd.ReadConfig(a,b);
          Cmd.ParseEnvVar();
        }
        for (int I=1;I<a;I++)
          Cmd.ParseArg(b[I],NULL);	//argue -p1234	read to *Password

    /*if (Cmd.IsConfigEnabled(argc,argv))
    {
      Cmd.ReadConfig(argc,argv);
      Cmd.ParseEnvVar();
    }
    for (int I=1;I<argc;I++)
      Cmd.ParseArg(argv[I],NULL);	//argue -p1234	read to *Password*/

    Cmd.ParseDone();

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



