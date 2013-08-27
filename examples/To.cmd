/* rexx */
 '@echo off'

/* modify this depending on your settings */
 ScreenW = 1024 /*800*/;
 ScreenH = 768  /*600*/;

 tmp = querystat('F');
 parse var tmp junk '-p' wX ',' wY ',' wW ',' wH ' ' junk;

/* parse command line */
 line = arg(1);
 if words(line) = 0
  then doHelp();

 do while length(line) > 0
  arg = translate(word(line, 1));
  tmp = wordindex(line, 2);
  if tmp > 0 then line = substr(line, tmp); else line = '';
  select
   when abbrev('TOP', arg, 1)
    then wY = ScreenH - wH;
   when abbrev('BOTTOM', arg, 1)
    then wY = 0;
   when abbrev('LEFT', arg, 1)
    then wX = 0;
   when abbrev('RIGHT', arg, 1)
    then wX = ScreenW - wW;
   when abbrev('HCENTER', arg, 1)
    then wX = (ScreenW - wW) / 2;
   when abbrev('VCENTER', arg, 1)
    then wY = (ScreenH - wH) / 2;
   when abbrev('DEFAULT', arg, 1)
    then do
          tmp = querystat('T');
          parse var tmp junk '-p' wX ',' wY ',' wW ',' wH ' ' junk;
         end;
   otherwise
    if pos(left(arg, 1), 'LRTBHVD') > 0
     then line = left(arg, 1)' 'substr(arg, 2)' 'line;
     else doHelp();
  end;
 end;

 'console -p'wX','wY;

exit;

querystat:
 procedure;
 arg default;
 if (default = 'T')
  then 'console -q | rxqueue /lifo';
  else 'console -p -q | rxqueue /lifo';
 do i = 1 to 2
  parse pull line;
 end;
 do while queued() > 0
  pull junk;
 end;
return line;

doHelp:
 SetColor('WHITE');
 say 'Ú[ TO ]ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ[ Version 0.0.1 ]¿'
 say 'Ã Copyright 1996,97 by FRIENDS software Ä All rights reserved Ù'
 SetColor('LCYAN');
 say '³ This script will put current console window to either top, left,'
 say '³ right, bottom side of screen or horizontally center, vertically'
 say '³ center or put it to default position'
 SetColor('GREEN');
 say 'Ã Command-line format:'
 SetColor('LGREEN');
 say 'Ã to {top|bottom|left|right|hcenter|vcenter|default}'
 SetColor('BLUE');
 say '³ Example:'
 say 'Ã to top'
 say 'Ã to top left'
 say 'Ã to vcenter right'
 say 'Ã to hv'
 say 'Ã to default'
 SetColor('LBLUE');
 say 'ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ'
 SetColor('DGRAY');
 say 'Ã¿Don`t forget to modify ScreenW and ScreenH variables inside'
 say 'Ã´the script according to your settings since there is no way'
 say 'À´to query them from REXX...'
exit 1;

SetColor:
 procedure;
 arg Col;
 Col = ColorNo(Col);

 if Col = -1 then return "";
 if Col > 7
  then Col = '1;3'Col-8;
  else Col = '0;3'Col;
 call charOut ,d2c(27)'['Col'm';
 return "";
 
ColorNo:
 procedure;
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
  otherwise
 end;
 return -1;
