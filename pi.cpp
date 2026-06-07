#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Error: Debes ingresar el valor de n." << std::endl;
        std::cerr << "Uso: " << argv[0] << " <numero>" << std::endl;
        return 1;
    }

    int n = std::stoi(argv[1]);
    double dx = 1.0/n;
    double suma = 0.0;
    double x, altura, area;
    
    for (int i = 0; i < n; i++)
    {
        x = (i+0.5) * dx;
        altura = 4.0 / (1.0 + (x*x));
        area = altura * dx;
        suma+=area;
    }

    cout << "El valor obtenido es " << suma << endl;
}
