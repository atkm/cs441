default:
	mpicc -c draw_forest.c -o draw_forest.o `pkg-config --cflags --libs gtk+-3.0 cairo`
	mpicc -c p_msort.c -o p_msort.o
	mpicc -c list_rank.c -o list_rank.o `pkg-config --cflags --libs gtk+-3.0 cairo`
	mpicc list_rank.o p_msort.o draw_forest.o -o list_rank `pkg-config --cflags --libs gtk+-3.0 cairo`

