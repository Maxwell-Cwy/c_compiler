int max(int t1,int t2){
	int t3,t4;
	t3=t1;
	t4=t2;
	if(t3>t4){
		return 1;
	}else{
		return 0;
	}
}

int main(){
	int a,b,c;
	a=1;
	b=2;
	c=max(a,b);
}