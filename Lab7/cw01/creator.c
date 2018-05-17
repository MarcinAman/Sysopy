#include "hairdressers.h"

enum Client_status status;
int shared_memory_id;
int semaphore_id;

void claim_the_throne();
void run_client(int);

void claim_the_throne() {
    pid_t pid = getpid();

    if (status == INVITED) {
        pop_queue();
    } else if (status == NEWCOMER) {
        while (1) {
            release_semaphore(semaphore_id);
            get_semaphore(semaphore_id);
            if (barbershop->barber_status == READY) break;
        }
        status = INVITED;
    }
    barbershop->selected_client = pid;

    printf("%lo: PID :%i took place\n", get_time(), pid);
}

void run_client(int S) {
    pid_t pid = getpid();
    int cuts = 0;

    while (cuts < S) {
        // Handle client
        status = NEWCOMER;

        get_semaphore(semaphore_id);

        if (barbershop->barber_status == SLEEPING) {
            printf("%lo PID: %i woke up the barber\n", get_time(), pid);
            barbershop->barber_status = AWOKEN;
            claim_the_throne();
            barbershop->barber_status = BUSY;
        } else if (!is_queue_full()) {
            enter_queue(pid);
            printf("%lo PID: %i entering the queue\n", get_time(), pid);
        } else {
            printf("%lo PID:%i left the barber without success\n", get_time(), pid);
            release_semaphore(semaphore_id);
            return;
        }

        release_semaphore(semaphore_id);

        while(status < INVITED) {
            get_semaphore(semaphore_id);
            if (barbershop->selected_client == pid) {
                status = INVITED;
                claim_the_throne();
                barbershop->barber_status = BUSY;
            }
            release_semaphore(semaphore_id);
        }

        while(status < SHAVED) {
            get_semaphore(semaphore_id);
            if (barbershop->selected_client != pid) {
                status = SHAVED;
                printf("%lo PID: %i shaved\n", get_time(), pid);
                barbershop->barber_status = IDLE;
                cuts++;
            }
            release_semaphore(semaphore_id);
        }
    }
    printf("%lo PID: %i left barbershop after %i cuts\n", get_time(), pid, S);
}


// Takes two arguments:
// clients number - how many clients should be generated
// S              - number of cuts each client expects before dying or whatever
int main(int argc, char** argv) {
    CHECK_WITH_EXIT(argc,3,"!=","Wrong args, we want 2,(1->number of clients,2->number of cuts)\n")

    int clients_number = atoi(argv[1]);
    int S = atoi(argv[2]);

    key_t project_kij = generate_key();

    shared_memory_id = get_shared_memory(project_kij);

    barbershop = shmat(shared_memory_id, 0, 0);
    CHECK_WITH_EXIT(barbershop,(void*) -1,"==","Error at accesing shared memory\n");

    semaphore_id = semget(project_kij, 0, 0);

    CHECK_WITH_EXIT(semaphore_id,-1,"==","Error while creating sem\n");

    for (int i = 0; i < clients_number; ++i) {
        if (fork()==0) {
            run_client(S);
            exit(0);
        }
    }
    while (wait(0)) if (errno != ECHILD) break;
}
