run:
	clang -O3 -march=native -flto main.c && ./a.out
