#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "classes.h"
#include "functions.h"
#include "loopPrincipal.h"

using namespace std;

int main() {

    //-----EXTRACCIÓN DE DATOS DE ARCHIVOS-----

    string nombreArchivo = "AB101";
    string archivoInput = "Instancias/"+nombreArchivo+".dat";
    time_t start, end;
    ifstream input(archivoInput);
    Instancia inst;
    Nodo *nodos;
    time(&start);
    if(input.is_open()){
        inst = extraerInstancia(input);
        if(inst.nombre == ""){
            cout << "\nERROR EN EXTRACCION DE INSTANCIA\n";
            exit(-1);
        }
        //Almacenar nodos en variable de heap:
        nodos = (Nodo*)malloc(sizeof(Nodo)*(inst.numClientes+inst.numClientes+1));
        extraerNodos(input,inst.numEstaciones,inst.numClientes,nodos);
    }
    else{
        cout << "\nERROR EN LECTURA DE ARCHIVO\n";
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
    input.close();
    free(nodos);

    //-----EJECUCION DEL LOOP PRINCIPAL Y OBTENCIÓN DE RESULTADOS------
    
    ListaVehiculos vehiculos = loopPrincipal(depot, estaciones, clientes, inst);

    vehiculos.moveToStart();
    vehiculos.next();
    for(unsigned int i = 0;i<vehiculos.listSize;i++){
        vehiculos.curr->data.recorrido.print();
        cout << "Distancia recorrida: "<< vehiculos.curr->data.distanciaTotalRecorrida<<"\n";
        cout << "Tiempo transcurrido: "<< vehiculos.curr->data.tiempoTranscurrido<<"\n";
        vehiculos.next();
    }

    time(&end);
    
    //-----GENERAR ARCHIVO DE SOLUCIÓN-----
    
    string archivoOutput = "Soluciones/"+nombreArchivo+".out";
    ofstream output(archivoOutput);
    double tiempoEjecucion = double(end-start) / double(CLOCKS_PER_SEC);

    //Calcular distancia total recorrida
    float sumaDistancias = 0.0;
    for(int i = 0; i < vehiculos.listSize; i++) sumaDistancias += vehi[i].distanciaRecorrida;
    
    //Escribir en el archivo de output
    output << std::fixed << sumaDistancias << "    " << inst.numClientes << "     " << vehiculos.listSize << "     "<< tiempoEjecucion <<"\n";
    for(int i = 0; i < vehiculos.listSize; i++){ 
        //vehi[i].recorrido.print();
        output << std::fixed << "Ruta vehiculo "<< i << "     " << vehi[i].distanciaRecorrida << "    " << vehi[i].tiempoTranscurrido << "\n";
    }
    
    output.close();
    
    
  return 0;
} 


