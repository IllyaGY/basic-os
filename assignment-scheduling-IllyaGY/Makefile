# This file is used to compile your code and run your code. Do not modify this file. 
SRC := src
INCLUDE := include
BUILD := build
RESULT := result

all:
	mkdir -p build 
	gcc $(SRC)/runall.c $(SRC)/pslibrary.c -I$(INCLUDE) -o $(BUILD)/runall
	gcc $(SRC)/Assign_sched.c $(SRC)/pslibrary.c -I$(INCLUDE) -o $(BUILD)/Assign_sched

test:
	./$(BUILD)/Assign_sched 2 10 3 5 5 2 3

run: all
	@mkdir -p $(RESULT) 
	@./$(BUILD)/runall 
	@wc *.all > $(RESULT)/wc.txt
	md5sum *.all 
	@md5sum fcfs.all > $(RESULT)/fcfs.txt
	@md5sum psjf.all > $(RESULT)/psjf.txt
	@md5sum rr.all > $(RESULT)/rr.txt
	@md5sum sjf.all > $(RESULT)/sjf.txt

test1:
	@cat $(RESULT)/fcfs.txt

test2:
	@cat $(RESULT)/sjf.txt

test3:
	@cat $(RESULT)/psjf.txt

test4:
	@cat $(RESULT)/rr.txt

clean:
	rm -rf $(BUILD) 
	rm -rf $(RESULT)/*.all


