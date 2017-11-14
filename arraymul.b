declblock{
	int arr1[10000], arr2[10000], arr3[10000], temp, i, j;
}

codeblock{
	for i=0,10000{
		arr1[i] = 9999-i;
	}
	for i=0,10000{
		arr2[i] = i;
	}

	for j=0,10000,2{
		if(arr1[j]>arr2[j]){
			arr3[j] = arr1[j] * arr1[j];
		}
		else{
			arr3[j] = arr2[j] * arr2[j];
		}
	}

	for i=1,10000,2{
		if(arr1[i]<arr2[j]){
			arr3[i] = arr1[i] * arr1[i];
		}
		else{
			arr3[i] = arr2[i] * arr2[i];
		}
	}

	for i=0,10000{
		temp = arr3[i] * arr3[i];
		arr3[i] = temp;
	}

	for i=0,10000{
		print arr3[i]," ";
	}
	println "";
}

