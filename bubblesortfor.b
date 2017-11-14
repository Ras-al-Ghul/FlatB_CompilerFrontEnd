declblock{
	int arr[20], temp, i, j;
}

codeblock{
	for i=0,19{
		arr[i] = 19-i;
	}

	for i=0,19{
		for j=i+1,20{
			if(arr[i] > arr[j]){
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
	for i=0,20{
		print arr[i]," ";
	}
	println "";
}