declblock{
	int arr[2000], temp, i, j;
}

codeblock{
	i = 0;
	while(i<2000){
		arr[i] = 1999-i;
		i = i+1;
	}
	i = 0;
	while(i<1999){
		j = i+1;
		while(j<2000){
			if(arr[i] > arr[j]){
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
			j = j+1;
		}
		i = i + 1;
	}
	i = 0;
	while(i<2000){
		print arr[i]," ";
		i = i+1;
	}
	println "";
}

