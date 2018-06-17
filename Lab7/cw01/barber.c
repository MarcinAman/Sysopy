#include "hairdressers.h"

int shared_memory_id;
int semaphore_id;

void handle_signal(int amazingly_important_signal_value) {
    printf("Closing barbershop\n");
    exit(0);
}

void invite_client() {
    barbershop->selected_client = barbershop->queue[0];
    printf("%lo barber: invited client %i\n", get_time(), barbershop->queue[0]);
}

void cut_clients_head() {
    printf("%lo Current client on chair: %i\n",get_time(),barbershop->selected_client);
    barbershop->selected_client = 0;
}

void atexit_handler() {
    if(semaphore_id != 0) {
        semctl(semaphore_id, 0, IPC_RMID);
    }
    if(shared_memory_id != 0) {
        shmctl(shared_memory_id, IPC_RMID, NULL);
    }
}


int main(int argc, char** argv) {
    CHECK_WITH_EXIT(argc,2,"!=","Wrong number of args, we want 1 arg -> waiting room size\n");
    CHECK_WITH_EXIT(signal(SIGTERM, handle_signal),SIG_ERR,"==","Error at SIGTERM");
    CHECK_WITH_EXIT(signal(SIGINT, handle_signal),SIG_ERR,"==","Error at SIGINT");
    CHECK_WITH_EXIT(atexit(atexit_handler),0,"!=","Error at atexit");

    int roomS = atoi(argv[1]);
    CHECK_WITH_EXIT(roomS,MAX_QUEUE_SIZE,">","Provided room size is too big\n")

    key_t project_kij = generate_key();

    shared_memory_id = get_shared_memory(project_kij);

    barbershop = shmat(shared_memory_id, 0, 0);

    if(barbershop == (void*)-1){
        FAILURE_EXIT("Error at accesing shared memory\n");
    }
//    CHECK_WITH_EXIT(barbershop,(void*) -1,"==","Error at accesing shared memory\n");

    semaphore_id = semget(project_kij, 1, IPC_CREAT | S_IRWXU);

    CHECK_WITH_EXIT(semaphore_id,-1,"==","Error while creating sem\n");

    semctl(semaphore_id, 0, SETVAL, 0);

    barbershop->barber_status = SLEEPING; //because we initialize barber first, then his victims
    barbershop->waiting_room_size = roomS;
    barbershop->client_count = 0;
    barbershop->selected_client = 0;

    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) barbershop->queue[i] = 0;

    release_semaphore(semaphore_id);

    printf("Barber initialized and sleeping!\n");

    while(1) {
        get_semaphore(semaphore_id);

        if(barbershop->barber_status == IDLE && is_queue_empty()){
            BARBER_SLEEP
        }
        else if(barbershop->barber_status == IDLE && !is_queue_empty()){
            BARBER_SERVE
        }
        else if(barbershop->barber_status == AWOKEN){
            BARBER_READY
        }
        else if(barbershop->barber_status == BUSY){
            BARBER_BUSY
        }

        release_semaphore(semaphore_id);
    }
}