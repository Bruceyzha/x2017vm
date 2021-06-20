CC=gcc 
CFLAGS= -s -Os
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections
# fill in all your make rules
vm_x2017: x2017.c vm.c
	$(CC) $(CFLAGS) -c $^
	$(CC) $(LDFLAGS) -o $@ x2017.o vm.o
	strip -R .note.ABI-tag $@
	strip -R .note.gnu.build-id $@
	strip -R .gnu.version $@
	strip -R .gnu.hash $@
	strip -R .note.gnu.property $@
	strip -R .shstrtab $@
	strip -R .comment $@
	strip -R .eh_frame_hdr $@
objdump_x2017:disassembler.c vm.c
	$(CC) $^ -o $@

tests:
	bash test.sh

run_tests:
	bash test.sh

clean:
	rm vm_x2017