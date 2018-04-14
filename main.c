#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <gmp.h>
#include <time.h>
#include <x86intrin.h>
#include <unistd.h>

#define DEBUG_RUNTIMEPAD 

int runtimepad_count;

void runtimepad_sigHandler(int signo){
	if(signo == SIGRTMAX){
		runtimepad_count = 1;
	}
}

void crypto_random_make_mpz(mpz_t ret, unsigned int l){
	unsigned char r[l / 8]; unsigned short tmp;
	int i, j;
	for(i = 0; i < l / 8 - 4; i+=4){
		_rdrand32_step((unsigned int *)&r[i]);
	}
	for(j = i; j < l / 8; j++){
		_rdrand16_step(&tmp);
		r[j] = tmp % 256;
	}
	mpz_import(ret, l / 8, 1, 1, 0, 0, r);
}

void randomized_time_program(void *carryover){
	mpz_t a, b;
	mpz_inits(a, b, NULL);

	for(int i = 0; i < 10; i++){
		crypto_random_make_mpz(a, 2048);
		crypto_random_make_mpz(b, 224);
		
		mpz_powm(a, a, b, *((mpz_t*)carryover));
	}

	mpz_clears(a, b, NULL);
}

void runtimepad(const long expected_usec, const long cooldown_times, void (*func)(void*), void *carryover){
	long spent_usec = 0;
	struct timespec start;
	struct timespec end;
	long max_usec = 0;

	long cur_usec;

	#ifdef DEBUG_RUNTIMEPAD
	long runtime = 0;
	#endif

	runtimepad_count = 0;

	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	
	sigact.sa_flags = SA_SIGINFO;
	sigact.sa_handler = runtimepad_sigHandler;

	sigaction(SIGRTMAX, &sigact, 0);

	struct sigevent sigev;
	memset(&sigev, 0, sizeof(sigev));

	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGRTMAX;
	sigev.sigev_value.sival_int = 1;

	timer_t timer;
	int rtn = timer_create(CLOCK_MONOTONIC, &sigev, &timer);
	if(rtn != 0){
		perror("Timer cannot be created");
		return ;
	}

	struct itimerspec itimer;
	itimer.it_value.tv_sec = expected_usec / 1000 / 1000;
	itimer.it_value.tv_nsec = (expected_usec % 1000000) * 1000;
	itimer.it_interval.tv_sec = 0L;
	itimer.it_interval.tv_nsec = 0L;
	rtn = timer_settime(timer, 0, &itimer, 0);
	if(rtn != 0){
		perror("Timer cannot be created");
                return ;
	}

	while(1){
		#ifdef DEBUG_RUNTIMEPAD
		runtime = runtime + 1;
		#endif
		
		clock_gettime(CLOCK_MONOTONIC, &start);
		func(carryover);		
		clock_gettime(CLOCK_MONOTONIC, &end);

		cur_usec = (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_nsec - start.tv_nsec) / 1000;
		if(cur_usec > max_usec){
			max_usec = cur_usec;
		} 
		spent_usec += cur_usec;
		if(expected_usec - spent_usec < cooldown_times * max_usec){
			break;
		}
	}

	while(runtimepad_count == 0){
		usleep(10);
	}

	#ifdef DEBUG_RUNTIMEPAD
	printf("The function ran %ld times. We spent %ld usecond for cooling down.\n", runtime, expected_usec - spent_usec); 
	#endif

	timer_delete(timer);
}

int main(){
	mpz_t p;
	mpz_init(p);
	crypto_random_make_mpz(p, 2048);

	struct timespec start;
	struct timespec end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	runtimepad(1000 * 1000, 5, &randomized_time_program, &p);
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("time spent in the part managed by runtimepad: %ld usec\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_nsec - start.tv_nsec) / 1000);

	mpz_clear(p);

	return 0;
}
