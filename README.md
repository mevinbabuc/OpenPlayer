OpenPlayer
==========

Linux Console mp3 Player

Instaltion
==========

Download bass audio library(linux) from http://www.un4seen.com/bass.html and place it in the same directory as OpenPlayer

Compiling the source
====================

 gcc main.c ./libbass.so -o play

Running the application
=======================
  ./play $song_dir 3    to play all the songs from the directory 
  ./play song.mp3       to play a particular song

Features
========

Has an integrated shell with play/pause , stop , next and prev controls.
Help can be accessed by typing h in the shell. 

