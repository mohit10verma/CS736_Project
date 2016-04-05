gcc -o test_check test_check.c -m64 -DARCH_x86_64 libfiber.so
./test_check $1
