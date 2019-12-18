default:
	dd if=/dev/zero of=sharedfile count=10 bs=10
	gcc -o receiver receiver.c
	gcc -o sender sender.c
	
clean:
	rm -f sender
	rm -f receiver
	rm -f sharedfile

	


