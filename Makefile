all:
	cd lib && make clean && make
	cd cat && make clean && make
	cd revwords && make clean && make	
	cd lenwords && make clean && make


clean:
	cd lib && make clean
	cd cat && make clean	
	cd revwords && make clean		
	cd lenwords && make clean	
