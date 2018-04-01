void *thread(void* arg){
	printf("Hello from thread\n");
	t_sleep(1);
	return 0;
}

void main(){
	int numth = t_create(thread);
	t_sleep(5);
	printf("Main thread is gonna end\n");
}
