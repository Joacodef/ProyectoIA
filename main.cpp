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
    double *distanciaAlCliente = (double*)malloc(sizeof(double));
    ListaNodos nodosRestringidos[numVehiculos];
    for(int i = 0; i < numVehiculos; i++) nodosRestringidos[i] = ListaNodos();
    Nodo *nodoClienteCercano;
    Vehiculo vehi[numVehiculos];
    //Inicializar vehiculos con el deposito
    for(int i = 0; i < numVehiculos; i++) vehi[i].recorrido.insert(depot);
    //LOOP PRINCIPAL, que itera sobre cada vehiculo y le intenta asignar el cliente mas cercano
    int j = 0;
    while(!flag){
        for(int i = 0; i < numVehiculos; i++){
            vehi[i].recorrido.moveToEnd();
            Nodo nodoVehiActual = vehi[i].recorrido.curr->data;
            //Probar que las posibles asignaciones esten permitidas segun restricciones
            nodoClienteCercano = nodoMenorDistancia(nodoVehiActual, clientes, inst.numClientes, nodosRestringidos[i], distanciaAlCliente);
            
            vehi[i].agregarParada(*nodoClienteCercano, inst.velocidad, *distanciaAlCliente);
            for(int i = 0; i < numVehiculos; i++){
                nodosRestringidos[i].moveToEnd();
                nodosRestringidos[i].insert(*nodoClienteCercano);
            }
        }
        j++;
        if(j==5) flag = true;
    }
    for(int i = 0; i < numVehiculos; i++) vehi[i].recorrido.print();
    //Liberar memoria
    for(int i = 0; i < numVehiculos; i++) vehi[i].recorrido.clear();
    for(int i = 0; i < numVehiculos; i++) nodosRestringidos[i].clear();
    free(distanciaAlCliente);
  return 0;
} 
