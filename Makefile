# 6Harmonics
# maintainer: Qige <qigezhao@gmail.com> since 2017.12.27

ALL=libgws2.so
EXTRA_FLAGS:=-l iwinfo -fPIC -shared

all: $(ALL)

$(ALL): gws2/*.c
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -I. -o $@ $^ -L.
    
clean:
	rm -f $(APP) gws2/*.o
