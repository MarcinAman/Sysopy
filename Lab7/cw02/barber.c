#include "hairdressers.h"

int shared_memory_id;
sem_t* semaphore_id;

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
    if (semaphore_id != 0) sem_unlink(PROJECT_PATH);
    if (shared_memory_id != 0){
        shm_unlink(PROJECT_PATH);
        munmap(barbershop, sizeof(*barbershop));
    }
}

void init(int argc, char** argv) {
    CHECK_WITH_EXIT(signal(SIGTERM, handle_signal),SIG_ERR,"==","Error at SIGTERM");
    CHECK_WITH_EXIT(signal(SIGINT, handle_signal),SIG_ERR,"==","Error at SIGINT");
    CHECK_WITH_EXIT(atexit(atexit_handler),0,"!=","Error at atexit");

    int roomS = atoi(argv[1]);
    CHECK_WITH_EXIT(roomS,MAX_QUEUE_SIZE,">","Provided room size is too big\n")

    shared_memory_id = get_shared_memory();

    TRUC_FILE;

    GETBARBERSHOP();

//    CHECK_WITH_EXIT(barbershop,(void*) -1,"==","Error at accesing shared memory\n");

    if(barbershop == (void*)-1){
        FAILURE_EXIT("Error at accesing shared memory\n");
    }

    //We can have a dont-care approach and remove the O_EXCL flag but ...

    semaphore_id = sem_open(PROJECT_PATH, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG,0);

    if (semaphore_id == (void*) -1) FAILURE_EXIT("Couldn't create semaphore\n")

    barbershop->barber_status = SLEEPING; //because we initialize barber first, then his victims
    barbershop->waiting_room_size = roomS;
    barbershop->client_count = 0;
    barbershop->selected_client = 0;

    // Initialize empty clients queue
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) barbershop->queue[i] = 0;
}

int main(int argc, char** argv) {
    CHECK_WITH_EXIT(argc,2,"!=","Wrong number of args, we want 1 arg -> waiting room size\n");
    init(argc, argv);
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