rm -f file.bin
truncate --size=4096 first.bin
truncate --size=4096 second.bin
truncate --size=40960 u-boot-spl.bin
cat first.bin >> file.bin
cat second.bin >> file.bin
cat u-boot-spl.bin >> file.bin
cat u-boot.bin >> file.bin