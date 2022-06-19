#include <iostream>
#include <fstream>
#include <cstring>
#include "classes.h"
#include "formulas.h"

using namespace std;

int main() {

    //-----EXTRACCION DE DATOS DE ARCHIVOS:-----

    ifstream archivo("Instancias/AB101.dat");
    Instancia inst;
    Nodo *nodos;
    if(archivo.is_open()){
        inst = extraerInstancia(archivo);
        //Almacenar nodos en variable de heap:
        nodos= (Nodo*)malloc(sizeof(Nodo)*(inst.numClientes+inst.numClientes+1));
        extraerNodos(archivo,inst.numEstaciones,inst.numClientes,nodos);
    }
    else{
        cout << "ERROR EN LECTURA DE ARCHIVO\n";
        exit(-1);
    }
    //Almacenar depot, estaciones y clientes en variables de stack
    Nodo depot = nodos[0];
    Nodo estaciones[inst.numEstaciones];
    Nodo clientes[inst.numClientes];
    //depot.mostrar();
    for(int i=0;i<inst.numEstaciones;i++){
        estaciones[i] = nodos[i+1];
        //estaciones[i].mostrar();
    }
    for(int i=0;i<inst.numClientes;i++){
        clientes[i] = nodos[i+1+inst.numEstaciones];
        //clientes[i].mostrar();
    }
    free(nodos);
    
  return 0;
} 



