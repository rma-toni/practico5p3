#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

bool isPrime(int n){
	if (n <= 1) return false;
	for(int i = 2; i*i <= n; i++){
		if(n % i == 0){
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[]){

	if(argc < 2){
		cerr << "Error:  You need to send at least one interger parameter" << endl;
		return 1;
	}

	int rankid, size, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rankid);
	MPI_Get_processor_name(hostname, &length);

	const int n = stoi(argv[1]);
	
	int base_chunk = n / size;
	int res = n % size;

	int local_n = base_chunk + (rankid < res ? 1 : 0);

	int start_val = rankid * base_chunk + (rankid < res ? rankid : res);
	int end_val = start_val + local_n - 1;

	MPI_Barrier(MPI_COMM_WORLD);
	double start_time = MPI_Wtime();

	//local
	vector<int> primos_locales;
	for(int i = start_val; i <= end_val; i++){
		if(isPrime(i)){
			primos_locales.push_back(i);
		}
	}

	int local_count = primos_locales.size();

	//---RECOLECTAR CANTIDADES
	vector<int> recvcounts;
	if (rankid == 0) {
		recvcounts.resize(size);
	}

	MPI_Gather(&local_count, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

	//---PREPARAR MAESTRO
	vector<int> displs;
	vector<int> primos_globales;
	int total_primos_encontrados = 0;

	if (rankid == 0) {
		displs.resize(size);
		int suma_desplazamientos = 0;

		for(int i = 0; i < size; i++){
			displs[i] = suma_desplazamientos;
			suma_desplazamientos += recvcounts[i];
		}
		total_primos_encontrados = suma_desplazamientos;

		primos_globales.resize(total_primos_encontrados);
	}

	//RECOLECTAR DATOS
	MPI_Gatherv(primos_locales.data(), local_count, MPI_INT, primos_globales.data(), recvcounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);

	double end_time = MPI_Wtime();

	//RESULTADOS
	if(rankid == 0){
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "Analisis completo de 0 a " << n << endl;
		cout << "Primos encontrados: " << total_primos_encontrados << endl;
		cout << "Tiempo empleado desde el inicio de la busqueda: " << (end_time - start_time) << " segundos." << endl;

		cout << "Ultimos primos encontrados: ";
		for(int i = max(0, total_primos_encontrados -5); i < total_primos_encontrados; i++){
			cout << primos_globales[i] << " ";
		}
		cout << endl << "------------------------------------------------------------------------------" << endl;
	}
	MPI_Finalize();
	return 0;
}
