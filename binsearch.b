declblock{
	int a[1000000], size, i, j, left, right, mid, key, flag;
}
codeblock{
	i = 0;
	key = 4;
	for i = 0,1000000{
		a[i] = i+1;
	}
	for i=0,1000000{
		print a[i]," ";
	}
	println "";
	
	left = 0;
	right = 1999999;
	while(left<=right){
		mid = left + (right-left)/2;
		i = a[mid];
		if(i==key){
			println key;
			right = left + 1;
		}
		else{
		}
		if(i < key){
			left = mid + 1;
		}
		else{
			right = mid - 1;
		}
	}
}