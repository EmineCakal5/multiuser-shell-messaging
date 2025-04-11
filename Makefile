all:
	gcc view.c model.c controller.c -o proje `pkg-config --cflags --libs gtk+-3.0` -pthread
clean:
	rm -f proje
