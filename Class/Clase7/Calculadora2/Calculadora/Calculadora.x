struct Dato{
	int a;
	int b;
};

program CALCULADORA{
	version VER_CALCU{
		int suma(Dato)=1;
		int rest(Dato)=2;
		int multiplicar(Dato)=3;
		int dividir(Dato)=4;
	}=1;
}=0x30000001;
