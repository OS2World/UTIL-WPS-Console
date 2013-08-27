/* rexx */
 '@echo off'
 null = '1>/dev/nul 2>&1';

 call rxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
 call SysLoadFuncs

 call SysCls;
 call SetColor White;
 say center("컴컴컴캑 C O N S O L E 쳐컴컴컴", 78);
 call SetColor LCyan;
 say center("     installation program      ", 78);
 call SetColor LBlue;
 say center("컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�", 78);

 say "";
 call SetColor Yellow;
 say center("WARNING", 78);
 call SetColor LGreen;
 say "";
 say center("This program is free software; you can redistribute it and/or modify", 78);
 say center("it under the terms of the GNU General Public License as published by", 78);
 say center("the Free Software Foundation; either version 2 of the License, or", 78);
 say center("(at your option) any later version.", 78);
 say "";
 say center("This program is distributed in the hope that it will be useful,", 78);
 say center("but WITHOUT ANY WARRANTY; without even the implied warranty of", 78);
 say center("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the", 78);
 say center("GNU General Public License for more details.", 78);
 say "";
 say center("You should have received a copy of the GNU General Public License", 78);
 say center("along with this program; if not, write to the Free Software", 78);
 say center("Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA", 78);
 say "";
 call SetColor Red;
 say center("Press ENTER if you agree, any other key if you don't", 78);
 if SysGetKey('NOECHO') <> d2c(13)
  then do
	call SysCls;
	call SysCurPos 12,1;
	call SetColor LRed;
	say center("-=Too bad for you=-", 78);
        exit;
       end;

 call SetColor Yellow
 if stream('eConsole.exe', 'C', 'QUERY EXISTS') <> ''
  then do
        call charout ,'please wait a bit ...'
        'eConsole.exe 'null' <install.cmd';
        if (rc = 0)
         then do
               'del sConsole.exe sCWRP.exe 'null;
               'ren eConsole.exe Console.exe 'null;
               'ren eCWRP.exe CWRP.exe 'null;
              end;
         else do
               'del eConsole.exe eCWRP.exe 'null;
               'ren sConsole.exe Console.exe 'null;
               'ren sCWRP.exe CWRP.exe 'null;
              end;
        say ' ok';
       end;

 call SetColor Green
 say "Please enter the destination directory for program, ex: C:\OS2\APPS:"
 call SetColor LGreen
 do until (destDir <> '')
  pull destDir
 end;
 if (length(destDir) > 3) & (pos(right(destDir, 1), "\/") > 0)
  then destDir = left(destDir, length(DestDir) - 1);
 do while stream(destDir,"c","query datetime") = ""
  QueryCreate = Ask("The directory you specified does not exist. Create it? (Y/N)", "YN")
  select
   when QueryCreate = "Y"
    then 'mkDir 'destDir' 1>nul 2>nul'
   otherwise
    say "Aborting..."
    exit
  end
 end
 curDir = directory();
 destDir = directory(destDir);
 call directory curDir;

 if Pos(translate(destDir), translate(value('PATH',,'OS2ENVIRONMENT'))) = 0
  then do
        call SetColor LRed;
        say 'You should put "'destDir'" in the PATH variable of your CONFIG.SYS';
        say 'and then reboot';
       end;

 call CopyFile 'Console.exe', '';
 call CopyFile 'CWRP.exe', '';

 QueryFolder = Ask("Do you want a folder with CONSOLE usage examples? (Y/N)", "YN");
 if (QueryFolder = "Y")
  then do
        rc = SysCreateObject('WPFolder', 'Console', '<WP_DESKTOP>', ,
             'OBJECTID=<CONSOLE_FOLDER>;CCVIEW=NO;'||,
             'ICONVIEWPOS=30,50,50,40;ICONVIEW=FLOWED;', 'U');
        rc = SysSetObjectData('<CONSOLE_FOLDER>', 'OPEN=DEFAULT');
        rc = SysCreateObject('WPProgram', 'Weak title', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on 'destDir'\CONSOLE.EXE -t"Top-level title" * /k 'destDir'\CONSOLE.EXE -t"Weak title -- type {exit}" *', 'U');
        rc = SysCreateObject('WPProgram', 'Sticky title', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -t"Sticky title"', 'U');
        rc = SysCreateObject('WPProgram', 'Scrolling window', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;PARAMETERS=-t"Scrolling window" -p,,200,100 *', 'U');
        rc = SysCreateObject('WPProgram', 'Background', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -ob -p0,0', 'U');
        rc = SysCreateObject('WPProgram', 'Flashing', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;PARAMETERS=-of *', 'U');
        rc = SysCreateObject('WPProgram', 'Thick border', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;PARAMETERS=-b10,10 *', 'U');
        rc = SysCreateObject('WPProgram', '40x40 with 8x8 font', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -s40,40 -f8,8', 'U');
        rc = SysCreateObject('WPProgram', '80x33 with 10x6 font', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;PARAMETERS=-s80,33 -f10,6 *', 'U');
        rc = SysCreateObject('WPProgram', 'Alternate OS/2 window[640x480]', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -p80,30 -s80,40 -f10,6 *', 'U');
        rc = SysCreateObject('WPProgram', 'Alternate OS/2 window[800x600]', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -p80,40 -s80,33 -f14,8 *', 'U');
        rc = SysCreateObject('WPProgram', 'Alternate OS/2 window[1024x768]', '<CONSOLE_FOLDER>', ,
             'EXENAME='destDir'\CONSOLE.EXE;MINIMIZED=YES;PARAMETERS=-on -p360,104 -s80,40 -f14,8 *', 'U');
       end;

 say ""
 if Ask("Do you wish to read the documentation now? (Y/N)", "YN") = "Y"
  then 'view doc\Console.inf';
 say ""
 call SetColor Yellow;
 say "The directory doc\ contains documentation files in both"
 say "INF and HTML formats. Install program did not copied them,"
 say "you should do it itself if you want them."
 call SetColor LGreen;
 say "You also can go to examples\ directory to see some"
 say "sample Console usage REXX scripts"
exit 0;

CopyFile:
 parse arg fName dPath
 call SetColor Cyan
 say "Copying "fName" -> "destDir||dPath"\"fName
 'copy 'fName destDir||dPath' 1>nul 2>nul'
return;

Ask:
 parse arg Question,Reply;
 call SetColor Green
 rc = charOut(, Question)
 call SetColor LGreen
 do until Pos(Answer, Reply) \= 0
  KeyIn = SysGetKey("noecho")
  parse upper var KeyIn Answer
 end
 say Answer;
return Answer;

SetColor:
 procedure expose break;
 arg Col;
 Col = ColorNo(Col);

 if Col = -1 then return -1;
 if Col > 7
  then Col = '1;3'Col-8;
  else Col = '0;3'Col;
 call charOut ,d2c(27)'['Col'm';
return 0;

ColorNo:
 procedure expose break;
 arg ColName;
 if Substr(ColName, 1, 1) = 'L'
  then do
        ColName = right(ColName, length(ColName) - 1);
        Light = 8;
       end
  else Light = 0;
 select
  when Abbrev('BLACK', ColName, 3)
   then return Light + 0;
  when Abbrev('BLUE', ColName, 3)
   then return Light + 4;
  when Abbrev('GREEN', ColName, 3)
   then return Light + 2;
  when Abbrev('CYAN', ColName, 3)
   then return Light + 6;
  when Abbrev('RED', ColName, 3)
   then return Light + 1;
  when Abbrev('MAGENTA', ColName, 3)
   then return Light + 5;
  when Abbrev('BROWN', ColName, 3)
   then return Light + 3;
  when Abbrev('GRAY', ColName, 3)
   then return Light + 7;
  when Abbrev('DGRAY', ColName, 3)
   then return 8;
  when Abbrev('YELLOW', ColName, 3)
   then return 11;
  when Abbrev('WHITE', ColName, 3)
   then return 15;
 end;
return -1;
