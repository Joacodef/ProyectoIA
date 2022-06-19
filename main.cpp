#include <iostream>
#include <fstream>
#include <math.h>
#define RADIO_TIERRA 4182.44949
using namespace std;

class Nodo{
    public:
    int ID;
    char tipo;
    double longitud;
    double latitud;

    void mostrar(){
        cout << "\nID: " << ID;
        cout << "\nTipo: " << tipo;
        cout << "\nLongitud: " << longitud;
        cout << "\nLatitud: " << latitud << "\n";
    }
};

class Instancia{
    public:
    string nombre;
    int numClientes;
    int numEstaciones;
    int maxTiempo;
    int maxDistancia;
    double velocidad;
    int tiempoServicio;
    int tiempoRecarga;

    void mostrar(){
        cout << "\nNombre: " << nombre;
        cout << "\nNumClientes: " << numClientes;
        cout << "\nNumEstaciones: " << numEstaciones;
        cout << "\nMaxTiempo: " << maxTiempo;
        cout << "\nMaxDistancia: " << maxTiempo;
        cout << "\nVelocidad: " << velocidad;
        cout << "\nTiempoServicio: " << tiempoServicio;
        cout << "\nTiempoRecarga: " << tiempoRecarga << "\n";
    }
};

double aRadianes(double num){
    double rad = 0.0;
    rad = M_PI * (num/180.0);
    return rad;
}

double haversine(float num){
    return pow(sin(num/2.0),2);
}

double calcularDistancia(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = haversine(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * haversine(deltaLon);
    
    double c = 2.0 * atan2(sqrt(a),sqrt(1-a));

    distancia = RADIO_TIERRA * c;

    return distancia;
}

Instancia extraerInstancia(ifstream& archivo){
    Instancia inst;
    archivo >> inst.nombre;
    archivo >> inst.numClientes;
    archivo >> inst.numEstaciones;
    archivo >> inst.maxTiempo;
    archivo >> inst.maxDistancia;
    archivo >> inst.velocidad;
    archivo >> inst.tiempoServicio;
    archivo >> inst.tiempoRecarga;
    return inst;
}

void extraerNodos(ifstream& archivo, int numEstaciones, int numClientes, Nodo* nodos){
    Nodo *nodosAux= (Nodo*)malloc(sizeof(Nodo)*(numClientes+numClientes+1));
    int i = 1;
    //Siguientes lineas del archivo:
    while(i <= numClientes+numEstaciones+1){
        archivo >> nodosAux[i-1].ID;
        archivo >> nodosAux[i-1].tipo;
        archivo >> nodosAux[i-1].longitud;
        archivo >> nodosAux[i-1].latitud;
        i++;
    }
    nodosAux[72].mostrar();
    *nodos = *nodosAux;
    free(nodosAux);
}

int main() {
    //EXTRACCION DE DATOS DE ARCHIVOS:
    ifstream archivo("Instancias/AB101.dat");
    if(archivo.is_open()){
        Instancia inst;
        inst = extraerInstancia(archivo);
        Nodo *nodos= (Nodo*)malloc(sizeof(Nodo)*(inst.numClientes+inst.numClientes+1));
        extraerNodos(archivo,inst.numEstaciones,inst.numClientes,nodos);
        nodos[22].mostrar();
    }
    else{
        cout << "ERROR EN LECTURA DE ARCHIVO\n";
    }
    //FIN DE EXTRACCION DE DATOS


  return 0;
} 



