#include <iostream>
#include <vector>
#include <cstdlib> // Para rand() y srand()
#include <ctime>   // Para time()
#include <mpi.h>

using namespace std;

int main(int argc, char* argv[]) {

    // Inicializar el entorno MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Identificador del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Número total de procesos (nodos)
    
    if (argc < 2) {
        if (rank == 0) {
            cerr << "Error: Debes ingresar el valor de n." << endl;
            cerr << "Uso: " << argv[0] << endl;
        }
        MPI_Finalize();
        return 1;
    }

    int n = std::stoi(argv[1]);
  
    //De momento usamanos dimensiones n que sean divisibles por la cantidad de procesos, luego se puede modificar para que funcione como el ejericicio anterior
    //pero en este caso es un poco mas complicado al ser una operacion de vector x matriz
    if (n <= 0 || n % size != 0) {
        if (rank == 0) {
            cout << "Error: La dimension ('n') debe ser mayor a 0 y divisible por el número de procesos (" << size << ")." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    //semilla para los números aleatorios
    srand(time(0));
  
    vector<int> A; // La matriz sera de 1 dimension para poder compartirla entre los procesos
    vector<int> B(n);
    vector<int> C;

    //Generar matriz, solo lo va a hacer el nodo maestro
    if (rank == 0) {
        srand(time(0));
        A.resize(n * n); // Equivalente a n x n
        C.resize(n, 0);

        for (int i = 0; i < n; ++i) {
            B[i] = rand() % 1001; 
            for (int j = 0; j < n; ++j) {
                //[i][j] se convierte en [i * n + j]
                A[i * n + j] = rand() % 1001; 
            }
        }
    }

    //Compartimos el vector B completo a todos los nodos
    MPI_Bcast(B.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    // Vector donde cada nodo almacenará sus filas asignadas de la matriz A
    vector<int> A_local(filas_locales * n);

    // Distribuir las filas de A entre todos los procesos
    MPI_Scatter(A.data(), filas_locales * n, MPI_INT, 
                A_local.data(), filas_locales * n, MPI_INT, 
                0, MPI_COMM_WORLD);

    // Vector para guardar el resultado de las multiplicaciones locales
    vector<int> C_local(filas_locales, 0);

    //Operamos
    for (int i = 0; i < filas_locales; ++i) {
        for (int j = 0; j < n; ++j) {
            C_local[i] += A_local[i * n + j] * B[j];
        }
    }

    //recolectamos los datos en el nodo maestro
    MPI_Gather(C_local.data(), filas_locales, MPI_INT, 
               C.data(), filas_locales, MPI_INT, 
               0, MPI_COMM_WORLD);
  
    // Mostrar resultados (Opcional, solo en el rank 0)
    /* if (rank == 0) {
        cout << "VECTOR RESULTADO: " << endl;
        for (int i = 0; i < n; ++i) {
            cout << C[i] << endl;
        }
    }
    */
    if (rank == 0) cout << "PROCESO TERMINADO CON EXITO" << endl;  
    MPI_Finalize();
    return 0;
}
