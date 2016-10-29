/*
 * Copyright (c) 2004
 * Juanjo Alvarez Martinez <juanjux@yahoo.es>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Author makes no representations about
 * the suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 */

 /* Based (copied of) on the Python language getopt implementation. It implements
  * a POSIX-alike getopt, that is, it stops parsing the options as soon as it
  * find a non-option argument, and removes the options it parses from args[][] so the 
  * application is left with all the nonoption arguments in args. If you don't want to use
  * short options or long options just change the argument for "null".
  *
  * Version: 0.4
  *
  * Changes:
  *
  * 0.1 Initial release
  * 0.2 Lot's of bugfixing, "posixification" and improvements thanks to Jonathan Leffler
  * 0.3 More posix'ation (handling of ':' at start of shortargs meaning "don't handle 
  *     missing argument errors").
  * 0.4 A fix and using writef/writefln instead of printf for the debug and the tests
  */


/* Sample usage:
 *
 * --------------------------d code----------------------------
 * import getopt;
 *
 * int main(char[][] args)
 * {
 *  //Take out the program name (args[0]):
 *  args = args[1..args.length];
 *  
 *  getopt_t[] usedoptions;
 *
 *  char[] shortoptions = "ho:";
 *  char[][2] longoptions;
 *  longoptions[0] = "help";
 *  longoptions[1] = "output=";
 *
 *  try
 *  {
 *      usedoptions = getopt(args, shortoptions, longoptions);
 *  } catch(GetOptException e)
 *  {
 *      writefln("Error in passed options: ", e.msg);
 *      showhelp();
 *      return 1;
 *  }

 *  foreach(getopt_t item; usedoptions)
 *  {
 *      if (item.option == "-h" || item.option == "--help")
 *      {
 *          showhelp();
 *          return 1;
 *      }
 *      else if (item.option == "-o" || item.option == "--output")
 *      {
 *          char[] output = item.value;
 *      }
 *  }
 *
 *  ----------------------------------code---------------------------------------
 */

//TODO: GNU getopt, object oriented interface


module getopt;

private import std.string;
private import std.array;
private import std.stdio;

enum GetOptError
{
    NoOption = 1,
    ArgRequired,
    ArgNotRequired
}

class GetOptException : Exception
{
    public GetOptError errtype;
    char[] option;
    this(char[] msg, GetOptError errtype, char[] option)
    {
        this.errtype = errtype;
        this.option = option;
        super(msg);
    }
}

struct getopt_t
{
    char[] option;
    char[] value;
}

getopt_t[] getopt(inout char[][] args, char[] shortopts, char[][] longopts)
{
    char[] currentarg;
    getopt_t save;
    getopt_t[] retlist;
    int retlistsize = 0;
    bool handleerrors = 1;

    if (shortopts.length > 0)
        if (shortopts[0] == ':')
            // Let the app handle the missing argument errors
            handleerrors = 0;

    if (longopts.length > 0)
        if (longopts[0] == "=")
            handleerrors = 0;

    while (args.length > 0 && args[0][0] == '-' && args[0] != "-")
    {
        if (args[0] == null)
        {
            args = args[1..args.length];
            continue;
        }
        if (args[0] == "--")
        {
            args = args[1..args.length];
            break;
        }

        currentarg = args[0];
        args = args[1..args.length];
        if (currentarg[0..2] == "--")
        {
            currentarg = currentarg[2..currentarg.length];
            save = do_longs(longopts, currentarg, args, handleerrors);
            retlist.length = ++retlistsize;
            retlist[retlistsize - 1].option = save.option;
            retlist[retlistsize - 1].value  = save.value;
            debug(getopt)
            {
                writefln("LongOpt: ", save.option);
                writefln("Value: ", save.value == "" ? "No value" : save.value);
            }
        }
        else
        {
            currentarg = currentarg[1..currentarg.length];
            getopt_t[] savelist;
                
            savelist = do_shorts(shortopts, currentarg, args, handleerrors);
            foreach(getopt_t it; savelist) {
                retlist.length = ++retlistsize;
                retlist[retlistsize - 1].option = it.option;
                retlist[retlistsize - 1].value  = it.value;
                debug(getopt)
                {
                    writefln("ShortOpt: ", it.option);
                    writefln("Value: ", it.value);
                    writefln();
                }
            }
        }
    }
    return retlist;
}

getopt_t do_longs(char[][] longopts, char[]str, inout char[][]args, bool handleerrors)
{
    getopt_t ret;
    char[] optarg, errormsg;

    int i = find(str, '=');
    if(i != -1)
    {
        // Take the argument and strip the argument part from the option
        optarg = str[i+1..str.length];
        str = str[0..i];
    }
    if(long_has_args(str, longopts))
    {   
        if(optarg.length == 0) //No "=", so it's an option after an space
        {
            if(args.length == 0)
            {
                if(handleerrors)
                {
                    errormsg = std.string.format("option --", str, " requires argument");
                    throw new GetOptException(errormsg, GetOptError.ArgRequired, str);
                }
                else optarg = "";
            }
            else
            {
                if(find(args[0], '-') == 0)
                {
                    if(handleerrors)
                    {
                        errormsg = std.string.format("option --",str," requires argument");
                        throw new GetOptException(errormsg, GetOptError.ArgRequired, str);
                    }
                    else
                        optarg = "";
                }
                else
                {
                    optarg = args[0];
                    args = args[1..args.length];
                }
            }
        }
    }
    else if(optarg.length > 0 && handleerrors)
    {
            errormsg = std.string.format("option --",str," must not have an argument (\"",optarg,"\" supplied)");
            throw new GetOptException(errormsg, GetOptError.ArgNotRequired, str);
    }
    ret.option = "--"~str;
    ret.value = optarg.length > 0 ? optarg : "";
    return ret;
}

bool long_has_args(char[]option, char[][]longoptions)
{
    int pos;
    bool hasopt;
    char[] origoption;

    foreach(char[] currentarg; longoptions) 
    {
        if(currentarg == null)continue;

        hasopt = 0;
        if( (pos = find(currentarg, '=')) != -1 )
        {
            origoption = currentarg[0..currentarg.length-1];
            hasopt = 1;
        }
        else
        {
            origoption = currentarg;
            hasopt = 0;
        }

        if(option != origoption)
            continue;
        else
            return hasopt;
    }
    
    char[] errormsg = std.string.format("option --",option," not recognized");
    throw new GetOptException(errormsg, GetOptError.NoOption, option);
}

getopt_t[] do_shorts(char[] shortopts, char[]str, inout char[][]args, bool handleerrors)
{
    getopt_t ret;
    getopt_t[] retlist;
    int retlistsize = 0;
    char[] optarg, errormsg;
    char opt;

    while(str.length > 0)
    {
        opt = str[0];
        str = str[1..str.length];
        if(short_has_args(opt,shortopts))
        {
            if(str.length == 0)
            {
                if(args.length == 0)
                {
                    if(handleerrors)
                    {
                        errormsg = std.string.format("option -",opt," requires argument");
                        throw new GetOptException(errormsg, GetOptError.ArgRequired, str);
                    }
                    else optarg = "";
                }
                else
                {
                    if(find(args[0], '-') == 0)
                    {
                        if(handleerrors)
                        {
                            errormsg = std.string.format("option -",opt," requires argument");
                            throw new GetOptException(errormsg, GetOptError.ArgRequired, str);
                        }
                        else
                        {
                            optarg = "";
                        }
                    }
                    else
                    {
                        optarg = args[0];
                        args = args[1..args.length];
                    }
                }
            }
            else
            {
                optarg = str;
                str = "";
            }
        }
        else
        {
            optarg = "";
        }
        retlist.length = ++retlistsize;
        ret.option = std.string.format("-", opt);
        ret.value = optarg.length > 0 ? optarg : "";
        retlist[retlistsize - 1] = ret;
    }
    return retlist;
}

bool short_has_args(char option, char[]shortoptions)
{
    //for(int i=0;i<shortoptions.length;i++)
    foreach(int i, char c; shortoptions)
    {
        if(option == c)
        {
            try 
            {
                if(shortoptions[i+1] == ':')
                    return 1;
                else
                    return 0;
            } catch(ArrayBoundsError) {
                return 0;
            }
        }
    }
    char[] stroption = std.string.format(option);
    char[] errormsg = std.string.format("option -",stroption," not recognized");
    throw new GetOptException(errormsg, GetOptError.NoOption, stroption);
}

unittest
{
    int idx = -1;
    int idx2 = -1;
    debug(getopt) writefln("getopt.getopt.unittest");
    
    char[][] longoptions;
    longoptions.length = 5;
    longoptions[++idx2] = "test1";
    longoptions[++idx2] = "test2=";
    longoptions[++idx2] = "test10";
    longoptions[++idx2] = "test20=";
    longoptions[++idx2] = "test30";
    
    char[] shortoptions = "ab:c:d";
    char[][] args;
    args.length = 10;
    args[++idx] = "--test1";
    args[++idx] = "-a";
    args[++idx] = "--test2";
    args[++idx] = "param2";
    args[++idx] = "-bparamb";
    args[++idx] = "--test10";
    args[++idx] = "--test20=param20";
    args[++idx] = "-adc";
    args[++idx] = "paramc";
    args[++idx] = "nonoptionargument";
    debug(getopt) 
        writefln("Args: --test1 -a --test2 param2 -bparamb --test10 --test20=param20 -adc paramc nonoptionargument");

    getopt_t[] usedoptions;
    usedoptions = getopt(args, shortoptions, longoptions);
    bool hastest1, hasa, hastest2, hastest2param, hasb, hasbparam, hastest10;
    bool hastest20, hastest20param, hasc, hascparam, hasnooption;
    foreach(getopt_t item; usedoptions)
    {
        if (item.option == "--test1") 
            hastest1 = 1;
        else if (item.option == "-a")
            hasa = 1;
        else if (item.option == "--test2") {
            hastest2 = 1;
            if (item.value == "param2")
                hastest2param = 1;
        }
        else if (item.option == "-b") {
            hasb = 1;
            if (item.value == "paramb") 
                hasbparam = 1;
        } 
        else if (item.option == "--test10")
            hastest10 = 1;
        else if (item.option == "--test20") {
            hastest20 = 1;
            if (item.value == "param20")
                hastest20param = 1;
        }
        else if (item.option == "-c") {
            hasc = 1;
            if (item.value == "paramc")
                hascparam = 1;
        }
    }

    assert(hastest1);
    assert(hasa);
    assert(hastest2);
    assert(hastest2param);
    assert(hasb);
    assert(hasbparam);
    assert(hastest10);
    assert(hastest20);
    assert(hastest20param);
    assert(hasc);
    assert(hascparam);
    assert(args[0] == "nonoptionargument");

    debug(getopt) writefln("getopt.unittest passed ok");
}

/*
// Only for testing:
void main(char[][] args) {
    args = args[1..args.length];
    getopt_t[] usedoptions;

    foreach(char[] argv;args) 
    {
        writefln("File: ", argv);
    }

    char[] shortoptions = "ho:";
    //char[] shortoptions = ":ho:";

    char[][2]longoptions;
    //char[][3]longoptions;
    int idxop = -1;
    //longoptions[++idxop] = "=";
    longoptions[++idxop] = "help";
    longoptions[++idxop] = "output=";


    try
    {
        usedoptions = getopt(args, shortoptions, longoptions);
    } catch(GetOptException e)
    {
        writefln("Error in passed options: ", e.msg);
        return 1;
    }

    foreach(getopt_t item; usedoptions)
    {
        if (item.option == "-h" || item.option == "--help")
        {
            writefln("-h or --help passed");
        }
        if (item.option == "-o" || item.option == "--output")
        {
            char[] output = item.value;
            writefln("-o or --output passed");
            writefln("Arg to -o or --output: ", output);
        }
    }
}


*/
