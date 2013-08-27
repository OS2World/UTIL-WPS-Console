/* rexx */
 '@echo on'

 call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
 call SysLoadFuncs

 say 'this example script will hide console window for five seconds'
 call charout , 'press Enter to continue...'
 pull junk;

 'console -ov-'
 call SysSleep 5;
 'console -ov+'
