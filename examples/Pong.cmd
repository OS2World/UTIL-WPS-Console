/* rexx */

 '@echo off'

/* modify this depending on your settings */
 ScreenW = 640 /*1024*/;
 ScreenH = 480 /*768*/;

 say 'This script will make current console window fly through the screen'
 say 'however, you`ll get the best results on a reasonably fast computer'
 say '(P5/133 and faster). On slower computers you`ll get only the main'
 say 'idea behind this :-)'
 say '--------------------------------------------------------------------'
 say 'Don`t forget to modify ScreenW and ScreenH variables inside the'
 say 'script since there is no way to query them from REXX...'
 call charout , 'press Enter to continue...'
 pull junk;

 line = querystat();
 restcon = substr(line, 4);

 'console -s14,5 -b10,10 -f18,10 -of'
 say 'Press [ESC]apekey to end    this mess...'

 cx = ScreenW/2; cy = ScreenH/2;
 wx = cx/2; wy = cy/2;
 dx = 0; dy = 0;
 Visible = '+';
 do while chars() = 0
  if (wx < cx)
   then dx = dx + 1
   else dx = dx - 1;
  if (wy < cy)
   then dy = dy + 1
   else dy = dy - 1;
  wx = wx + dx; wy = wy + dy;
  'console -p'wx','wy;

  if (Visible = '+') then n = random(0,100); else n = random(0,10);
  if n = 1
  then do
        if Visible = '-' then Visible = '+'; else Visible = '-';
        'Console -ov'Visible;
       end;
 end;

 /* Hide console temporarily */
 'Console -ov-'
 /* restore console at best we can... */
 restcon' -of- -ov -ox'
exit;

querystat:
 procedure;
 'console -p -f -q | rxqueue /lifo';
 do i = 1 to 2
  parse pull line;
 end;
 do while queued() > 0
  pull junk;
 end;
return line;
