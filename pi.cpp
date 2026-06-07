#include <iostream>
#include <string>
#include <mpi.h> 

using namespace std;

int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 0;
    
    if (rank == 0) {
        if (argc < 2) {
            std::cerr << "Error: Debes ingresar el valor de n." << std::endl;
            n = -1; 
        } else {
            n = std::stoi(argv[1]);
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); //Se comparte el valor de n a todos los nodos

    if (n < 0) {
        MPI_Finalize();
        return 1;
    }
    
    double dx = 1.0 / n;
    double suma_local = 0.0;
    double suma_global = 0.0;
    double x, altura, area;
    
    // Distribución del trabajo
    // Cada proceso empieza en su 'rank' y salta de 'size' en 'size'
    for (int i = rank; i < n; i += size) {
        x = (i + 0.5) * dx;
        altura = 4.0 / (1.0 + (x * x));
        area = altura * dx;
        suma_local += area;
    }

    if(rank == 0){
        cout << "El valor obteniodo es " << suma_global << endl;
    }
    
    MPI_Finalize();
    return 0;
}
