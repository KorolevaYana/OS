all:
	cd lib && make clean && make
	cd cat && make clean && make
	cd revwords && make clean && make	
<<<<<<< HEAD
	cd filter && make clean && make
=======
	cd lenwords && make clean && make

>>>>>>> origin/defence0102

clean:
	cd lib && make clean
	cd cat && make clean	
<<<<<<< HEAD
	cd revwords && make clean	
	cd filter && make clean
=======
	cd revwords && make clean		
	cd lenwords && make clean	
>>>>>>> origin/defence0102
