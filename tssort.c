// Author: Nat Tuck
// CS3650 starter code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include "float_vec.h"
#include "barrier.h"
#include "utils.h"
#include "float.h"


// float comparator 
int floatcomp(const void* n1, const void* n2)
{
	float f1 = *(float*)n1;
	float f2 = *(float*)n2;
	if (f1 < f2) return -1;
	if (f1 > f2) return 1;
	return 0;
}

// structure to call the thread
typedef struct args {
	int pnum;
	floats* input;
	const char* output;
	int P;
	floats* samps;
	long* sizes;
	barrier* bb;
} args;


// qsort given floats
	void
qsort_floats(floats* xs)
{
	qsort(xs->data,xs->size,sizeof(float), floatcomp);
}

// make a sample 
	floats*
sample(floats* input, int P)
{
	int num = 3*(P - 1);
	// make an array of random items
	floats* array = make_floats(num);
	int j = 0;
	// randomly select 3*(p-1) items
	while(j < num) {
		int ind = rand() % input->size;
		floats_push(array, input->data[ind]);
		j++;
	}  
	// sort an array of random items
	qsort_floats(array);
	// make an array of medians
	floats* array2 = make_floats(P+1);
	floats_push(array2, 0);
	j = 1;
	while (j < num){
		floats_push(array2, array->data[j]);
		j= j+3;
	}
	floats_push(array2, FLT_MAX);
	//free the array of random items
	free_floats(array);
	return array2;
}

// function to sort subarray and insert it values to correct places of output file
	void*
sort_worker(void* data)
{
	// cast function argument to the correct structure
	args a = *((args*) data);
	free(data);
	// create subarray
	floats* xs = make_floats(a.input->size);
	// add elements to the subarray
	float p1 = a.samps->data[a.pnum];
	float p2 = a.samps->data[a.pnum + 1];
	for (int i = 0; i < a.input->size; i++) {
		float pr = a.input->data[i];
		if (pr >= p1 && pr < p2) {
			floats_push(xs, pr);
		}
	}
	// print the info about the subarray
	printf("%d: start %.04f, count %ld\n", a.pnum, a.samps->data[a.pnum], xs->size);
	a.sizes[a.pnum] = xs->size;
	// wait until all sizes are accuired 
	barrier_wait(a.bb);
	// sort the the subarray
	qsort_floats(xs);
	// open an output file descriptor
	int ofd = open(a.output, O_WRONLY, 0644);
	check_rv(ofd);
	int ss = 0;
	// get the starting position
	for (int i =0; i < a.pnum; i++){
		ss+=a.sizes[i];
	}
	int i = ss;
	int j=0;
	// insert items to the correct position of output file
 		lseek(ofd, i*sizeof(float)+8, SEEK_SET);
		write(ofd, &xs->data[j], xs->size*sizeof(float));
   
         // version 1 (v1) discussed in the report.txt
         //while (i < ss+ xs->size ){
		     //lseek(ofd, i*sizeof(float)+8, SEEK_SET);
		     //write(ofd, &xs->data[j], sizeof(float));
		     //i++;
		     //j++;
	       //}

	//close output file descriptor
	int rv = close(ofd);
	check_rv(rv);
	// fre subarray
	free_floats(xs);
	return 0;

}

// function to spawn P threads 
	void
run_sort_workers(floats* input, const char* output, int P, floats* samps, long* sizes, barrier* bb)
{
	pthread_t threads[P];
	for (int i = 0; i < P; i++) {
		// for each thread create an argument
		args* a = malloc (sizeof(args));
		a->pnum = i;
		a->input = input;
		a->output = output;
		a->P = P;
		a->samps = samps;
		a->sizes = sizes;
		a->bb = bb;
		// create a thread
		int rv = pthread_create(&threads[i], 0, sort_worker, a);
		check_rv(rv);
	}
	// wait for threads to finish.
	for (int i = 0; i < P; i++) {
		int rv = pthread_join(threads[i], 0);
		check_rv(rv);
	}

}

// sample sort 
	void
sample_sort(floats* input, const char* output, int P, long* sizes, barrier* bb)
{
	// create a sample (array of medians) 
	floats* samps = sample(input, P);
	// run sort function on a given sample
	run_sort_workers(input, output, P, samps, sizes, bb);
	// free samples
	free_floats(samps);
}

	int
main(int argc, char* argv[])
{
	alarm(120);
	// number of arguments should be 4
	if (argc != 4) {
		printf("Usage:\n");
		printf("\t%s P input.dat output.dat\n", argv[0]);
		return 1;
	}
	// number of parrallel threads
	const int P = atoi(argv[1]);
	// name of the input file
	const char* iname = argv[2];
	// name of the output file
	const char* oname = argv[3];
	seed_rng();
	// open input file descriptor
	int ifd = open(iname, O_RDONLY, 0644);
	check_rv(ifd);
	// get the size of the input
	long size;
	read(ifd, &size, sizeof(long));
 	// create new floats (input) of size size
 	floats* input = make_floats(size);
	// get values from the input file and push them to input
  read(ifd, input->data, size*sizeof(float));
  input->size = size;
          // version 1 (v1) discussed in the report.txt
   	      //float buf;
  	      //int i;
        	//for (i=0; i< size; i++){
        	//	lseek(ifd, i*sizeof(float)+8, SEEK_SET);
        	//	read(ifd, &buf, sizeof(float));
        	//	floats_push(input, buf);
        	//}
  // close input file descriptor
	close(ifd);
	// open output file desriptor
	int ofd = open(oname, O_CREAT|O_TRUNC|O_WRONLY, 0644);
	check_rv(ofd);
	// set the size of output file to the size of input file
	ftruncate(ofd, size*sizeof(float)+sizeof(long));
	// write the size o the output to the output file
	write(ofd, &size, sizeof(long));
	//close output file descriptor
	int rv = close(ofd);
	check_rv(rv);
	// make a barrier
	barrier* bb = make_barrier(P);
	// allocate an array of sizes
	long* sizes = malloc(P * sizeof(long));
	// call the smaple sort
	sample_sort(input, oname, P, sizes, bb);
	// free the array of sizes , the barrier and the input
	free(sizes);
	free_barrier(bb);
	free_floats(input);
	return 0;
}

