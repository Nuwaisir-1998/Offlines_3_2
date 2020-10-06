#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>

using namespace std;

#define S 5
#define C 2
#define number_of_cycles 10

int idx_services;
int idx_payment;

pthread_mutex_t mutex_services[S];
sem_t sem_payment_full;
sem_t sem_payment_empty;
sem_t sem_depart;
sem_t sem_entrance;
sem_t sem_test;
sem_t sem_waiting_to_depart;

pthread_mutex_t testMutex;
pthread_mutex_t mutex_payment;
pthread_mutex_t mutex_after_payment;
pthread_mutex_t mutex_payment_finished;
pthread_mutex_t mutex_waiting_to_depart;
pthread_mutex_t mutex_waiting_at_entrance;
pthread_mutex_t mutex_loop;
pthread_mutex_t mutex_make_entrace_atomic;
pthread_mutex_t mutex_blocked;


/*****************************************************************************************************/
//	First, determine which parts of your program need to be completely independant of each other,
//	meaning, you don't want to change that part, by some other part of the code simultaneously.	
//	To achieve this, you have to wrap them (the whole code part) with the same mutex (otherwise they 
//  will effect each other). So, now do what you need to do to achieve this... Maybe make a wait call 
//  to semaphore inside a while(true) loop... Maybe use some global variables to keep track of some 
//  counts... and wrap them with a mutex while read, writing or processing it... And always validate
// 	your solution by stress testing.
/*****************************************************************************************************/


int gen_rand(){
	return rand() % 200 + 500;
}

int payment_finished = 0;
int inside_service_center = 0;
int waiting_at_entrance = S;
int waiting_to_depart = 0;
int blocked = 0;

void * run(void * arg){
	pthread_mutex_lock(&mutex_services[0]);

	bool flag;
	while(true){							// attempting to enter service center
		flag = false;
		pthread_mutex_lock(&mutex_payment_finished);
		if(payment_finished != 0){
			blocked++;
			pthread_mutex_unlock(&mutex_payment_finished);
			sem_wait(&sem_entrance);
			continue;
		}else {								// will enter service center
			flag = true;
			break;
		}
	}

	pthread_mutex_lock(&mutex_loop);
	inside_service_center++;
	pthread_mutex_unlock(&mutex_loop);

	pthread_mutex_lock(&mutex_waiting_at_entrance);
	waiting_at_entrance--;
	pthread_mutex_unlock(&mutex_waiting_at_entrance);

	// SERVICE CENTER STARTS

	for(int i=0;i<S;i++){
		printf("%s started taking service from serviceman %d\n", (char*)arg, i + 1);
		fflush(stdout);
		if(flag and i == 0){
			pthread_mutex_unlock(&mutex_payment_finished);
		}
		usleep(gen_rand());
		printf("%s finished taking service from serviceman %d\n", (char*)arg, i + 1);
		fflush(stdout);
		if(i+1 < S){
			pthread_mutex_lock(&mutex_services[i+1]);
		}
		else{
			pthread_mutex_lock(&mutex_loop);
			inside_service_center--;
			if(inside_service_center == 0){
				pthread_mutex_lock(&mutex_waiting_to_depart);
				int temp = waiting_to_depart;
				while(waiting_to_depart --){
					sem_post(&sem_waiting_to_depart);
				}
				waiting_to_depart = 0;
				pthread_mutex_unlock(&mutex_waiting_to_depart);
			}
			pthread_mutex_unlock(&mutex_loop);
		}
		pthread_mutex_unlock(&mutex_services[i]);
	}

	// SERVICE CENTER ENDS

	// PAYMENT ROOMS STARTS

	sem_wait(&sem_payment_empty);
	printf("%s started paying the service bill\n", (char*) arg);
	fflush(stdout);
	usleep(gen_rand());			
	pthread_mutex_lock(&mutex_payment_finished);
	payment_finished++;
	printf("%s finished paying the service bill\n", (char*) arg);						
	fflush(stdout);
	pthread_mutex_unlock(&mutex_payment_finished);
	sem_post(&sem_payment_empty);

	// Deciding whether to wait or not

	pthread_mutex_lock(&mutex_loop);
	if(inside_service_center != 0){
		pthread_mutex_unlock(&mutex_loop);
		pthread_mutex_lock(&mutex_waiting_to_depart);
		waiting_to_depart++;
		pthread_mutex_unlock(&mutex_waiting_to_depart);
		sem_wait(&sem_waiting_to_depart);
	}else{
		pthread_mutex_unlock(&mutex_loop);
	}

	// PAYMENT ROOM ENDS

	usleep(gen_rand());					// Departing
	
	pthread_mutex_lock(&mutex_payment_finished);
	printf("%s has departed\n", (char*) arg);
	fflush(stdout);
	payment_finished--;
	pthread_mutex_lock(&mutex_blocked);
	if(blocked != 0){
		blocked--;
		sem_post(&sem_entrance);
	}
	pthread_mutex_unlock(&mutex_blocked);
	pthread_mutex_unlock(&mutex_payment_finished);
	
	pthread_exit((void*)"Finished\n");
}


void init(){
	int res;
	for(int i=0;i<S;i++){
		res = pthread_mutex_init(&mutex_services[i],NULL);
		if(res != 0){
			printf("Failed to create mutex %d\n", (i+1));
		}
	}

	res = sem_init(&sem_test,0,0);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_init(&sem_waiting_to_depart,0,0);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_init(&sem_depart,0,0);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_init(&sem_entrance,0,0);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_init(&sem_payment_full,0,0);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_init(&sem_payment_empty,0,C);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_init\n");
    }

	res = pthread_mutex_init(&mutex_payment,NULL);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_init(&mutex_after_payment,NULL);
    if(res != 0){
        printf("Failed\n");
    }
	
	res = pthread_mutex_init(&testMutex,NULL);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_init(&mutex_waiting_to_depart,NULL);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_init(&mutex_loop,NULL);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_init(&mutex_make_entrace_atomic,NULL);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_init(&mutex_blocked,NULL);
    if(res != 0){
        printf("Failed\n");
    }
}

void destroy(){
	int res;
	for(int i=0;i<S;i++){
		res = pthread_mutex_destroy(&mutex_services[i]);
		if(res != 0){
			printf("Failed to create mutex %d\n", (i+1));
		}
	}

	res = sem_destroy(&sem_test);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_destroy(&sem_waiting_to_depart);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_destroy(&sem_depart);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_destroy(&sem_entrance);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_destroy(&sem_payment_full);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_full\n");
    }

	res = sem_destroy(&sem_payment_empty);
    if(res != 0){
        printf("Failed to create semaphore sem_payment_init\n");
    }

	res = pthread_mutex_destroy(&mutex_payment);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_destroy(&mutex_after_payment);
    if(res != 0){
        printf("Failed\n");
    }
	
	res = pthread_mutex_destroy(&testMutex);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_destroy(&mutex_waiting_to_depart);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_destroy(&mutex_loop);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_destroy(&mutex_make_entrace_atomic);
    if(res != 0){
        printf("Failed\n");
    }

	res = pthread_mutex_destroy(&mutex_blocked);
    if(res != 0){
        printf("Failed\n");
    }
}


int main(){
	// freopen("out", "w", stdout);
	int res;

	srand(time(NULL));

	init();

	pthread_t cycles[number_of_cycles];
	for(int i=0;i<number_of_cycles;i++){
		char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());
		pthread_create(&cycles[i], NULL, run, (void*) id);
	}

	for(int i=0;i<number_of_cycles;i++){
		void * result;
		pthread_join(cycles[i], &result);
	}

	destroy();

	return 0;
}