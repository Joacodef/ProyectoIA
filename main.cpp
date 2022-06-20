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
        nodos = (Nodo*)malloc(sizeof(Nodo)*(inst.numClientes+inst.numClientes+1));
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
    }
    for(int i=0;i<inst.numClientes;i++){
        clientes[i] = nodos[i+1+inst.numEstaciones];
        //clientes[i].mostrar();
    }
    free(nodos);
    
    
    bool flag = false;
    int numVehiculos = 10;
    Nodo *nodoClienteCercano;
    Vehiculo vehi[numVehiculos];
    //Inicializar vehiculos con el deposito
    for(int i = 0; i < numVehiculos; i++) vehi[i].recorrido.insert(depot);
    //Loop que itera sobre cada vehiculo y le intenta asignar el cliente mas cercano
    int j = 0;
    while(!flag){
        for(int i = 0; i < numVehiculos; i++){
            //Se debe verificar si se cumplen las restricciones de bencina y tiempo
            //Se debe calcular por adelantado si el tiempo alcanza para volver
            //y si la bencina alcanza para llegar a una estacion
            vehi[i].recorrido.moveToEnd();
            Nodo nodoVehiActual = vehi[i].recorrido.curr->data;
            nodoClienteCercano = calcularMenorDistancia(nodoVehiActual, clientes, inst.numClientes);
            nodoClienteCercano->visitado = true;
            vehi[i].recorrido.insert(*nodoClienteCercano);
        }
        j++;
        if(j==5) flag = true;
    }
    //Liberar memoria
    for(int i = 0; i < numVehiculos; i++) vehi[i].recorrido.clear();
  return 0;
} 
