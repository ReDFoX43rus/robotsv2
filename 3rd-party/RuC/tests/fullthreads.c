int ruc_sem = -1;

void* ruc_test_worker(void *arg){
	t_sem_wait(ruc_sem);
	printf("Hello from worker\n");
	t_sem_post(ruc_sem);

	/* 2 - descriptor of msg receiver */
	t_msg_send({2, 66});
	printf("Message sent");

	return NULL;
}

void* ruc_test_msg_reveice(void *arg){
	struct msg_info msg = t_msg_receive();
	printf("Received data: %i\n", msg.data);
	return NULL;
}

void test_ruc_threadsv2(void){
	int worker, receiver;
	t_init();

	ruc_sem = t_sem_create(0);

	printf("Creating worker\n");
	worker = t_create(ruc_test_worker);
	printf("Creating receiver\n");
	receiver = t_create(ruc_test_msg_reveice);

	printf("Worker ID: %i, Receiver ID: %i\n", worker, receiver);

	vTaskDelay(pdMS_TO_TICKS(10000));

	printf("Destroying everything\n");
	t_destroy();
	printf("OK\n");
}
