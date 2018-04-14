int main(){
	int distance;

	while(1){

		distance = getdigsensor(12, {4, 5});
		printf("distance: %i\n", distance);

		if(distance < 8){
			setvoltage(0, 1);
		} else {
			setvoltage(0, 0);
		}

		t_sleep(100);
	}

	return 0;
}
