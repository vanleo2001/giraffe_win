rm -f 9969.log
rm -f 22224.log

./monitor 2>&1 | tee print.txt
