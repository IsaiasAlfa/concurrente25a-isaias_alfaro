include ../../common/Makefile

FLAG += -fopenmp
INCLUDE += -I../omp_mergesort/src
LIBS += -lm

MIN=10000000
MAX=90000000
RANGE=$(MIN)-$(MAX)
COUNT=$(shell shuf -n 1 -i $(RANGE))
DATAF=tests/data.txt
TIMER=perf stat

.PHONY: data bench profile
data: $(DATAF) | $$(@D)/.

$(DATAF):
	echo ulong > $@
	shuf -r -n $(COUNT) -i 0-$(MAX) >> $@

bench: $(TARGETS) data
	$(TIMER) $(EXEARGS) < $(DATAF)

profile: debug data
	valgrind --tool=callgrind --separate-threads=yes $(EXEARGS) < $(DATAF)
