@echo off
cd ..\..\doc
call convert Console-rus.html from koi8 to alt
call html2ipf index.html
call convert Console-rus.html from alt to koi8
ipfc -i -s -D:7 -C:866 index.ipf Console.inf
del index.ipf
del *.bmp
view Console.inf
