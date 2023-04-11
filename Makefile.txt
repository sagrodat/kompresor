run : file.c getopt.c globals.c main.c misc.c print.c tree.c
        cc file.c getopt.c globals.c main.c misc.c print.c tree.c -lm -o run
test8 :
        ./run test.txt testkomp8.txt ; ./run testkomp8.txt testdekomp8.txt

test12 :
        ./run test.txt testkomp12.txt -t ; ./run testkomp12.txt testdekomp12.txt

test16 :
        ./run test.txt testkomp16.txt -s ; ./run testkomp16.txt testdekomp16.txt


clean :
        rm run

removetests:
        rm testkomp8.txt testdekomp8.txt testkomp12.txt testdekomp12.txt testkomp16.txt testdekomp16.txt