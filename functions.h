#include <math.h>
#include <iostream>
#include <fstream>
#include <cstring>

#define RADIO_TIERRA 4182.44949

using namespace std;

double aRadianes(double num){
    double rad = 0.0;
    rad = M_PI * (num/180.0);
    return rad;
}

double haversine(float num){
    return pow(sin(num/2.0),2);
}


//Funcion que calcula la distancia entre 2 puntos segun la formula de Haversine.

double calcularDistancia(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = haversine(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * haversine(deltaLon);
    
    double c = 2.0 * atan2(sqrt(a),sqrt(1-a));

    distancia = RADIO_TIERRA * c;

    return distancia;
}


//Funcion para calcular todas las distancias desde un nodo central a un conjunto de nodos.

double *calcularTodasDistancias(Nodo centro, Nodo *demasNodos, int size){
    double *distancias = (double*)malloc(sizeof(double)*size);
    for(int i=0; i<size;i++){
        distancias[i] = 0.0;
        distancias[i] = calcularDistancia(centro.longitud,centro.latitud,
                                    demasNodos[i].longitud,demasNodos[i].latitud);
    }
    return distancias;
}


//Funcion que chequea si el nodo "actual" está en el arreglo de "nodosRestringidos".

bool nodoRestringido(Nodo actual,ListaNodos nodosRestringidos){
    nodosRestringidos.moveToStart();
    nodosRestringidos.next();
    if(actual.ID == -1) return true;
    if(nodosRestringidos.listSize == 0) return false;
    //if(actual.restringidoTemp) return true;
    for(unsigned int i = 0;i<nodosRestringidos.listSize+1;i++){
        if(actual.ID == nodosRestringidos.curr->data.ID && actual.tipo == nodosRestringidos.curr->data.tipo){
            return true;
        }
        nodosRestringidos.next();
    }
    return false;
}


//Funcion que retorna el nodo que esté mas cerca del nodo "centro", chequeando si esta restringido.

Nodo* nodoMenorDistancia(Nodo centro, Nodo *demasNodos, int size, ListaNodos nodosRestringidos, double *distPtr){
    double menor = 999999999.9;
    Nodo *menorNodo;
    double distancia = 0.0;
    for(int i=0; i<size;i++){
        if(centro.ID == demasNodos[i].ID && centro.tipo == demasNodos[i].tipo) continue;
        if(nodoRestringido(demasNodos[i],nodosRestringidos)) continue;
        distancia = calcularDistancia(centro.longitud,centro.latitud,
                                    demasNodos[i].longitud,demasNodos[i].latitud);
        if(distancia<menor){
            menor = distancia;
            *distPtr = distancia;
            menorNodo = &demasNodos[i];
        }      
    }
    return menorNodo;
}

//Funciones que extraen datos de archivos:
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
        //Inicializar nodo para que valgrind no alegue:
        nodosAux[i-1] = Nodo();
        archivo >> nodosAux[i-1].ID;
        archivo >> nodosAux[i-1].tipo;
        archivo >> nodosAux[i-1].longitud;
        archivo >> nodosAux[i-1].latitud;
        i++;
    }
    memcpy(nodos,nodosAux,sizeof(Nodo)*(numClientes+numClientes+1));
    free(nodosAux);
}


