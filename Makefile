all:
	cd lib && make clean && make
	cd cat && make clean && make
	cd revwords && make clean && make	
	cd filter && make clean && make
	cd lenwords && make clean && make
	cd bufcat && make clean && make
	cd simplesh && make clean && make

clean:
	cd lib && make clean
	cd cat && make clean	
	cd revwords && make clean	
	cd filter && make clean
	cd lenwords && make clean	
	cd bufcat && make clean	
	cd simplesh && make clean
