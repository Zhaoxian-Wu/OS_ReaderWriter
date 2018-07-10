# !/usr/bin/shell
gcc fairCompetition.c -o fairCompetition.out -l pthread
gcc readFirst.c -o readFirst.out -l pthread
gcc writeFirst.c -o writeFirst.out -l pthread