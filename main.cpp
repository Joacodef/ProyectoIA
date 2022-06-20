#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "classes.h"
#include "functions.h"

using namespace std;

int main() {

    //-----EXTRACCIÓN DE DATOS DE ARCHIVOS:-----
    string nombreArchivo = "AB102";
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
    input.close();
    free(nodos);
    

    //-----LOOP PRINCIPAL, ENVÍO DE VEHÍCULOS-----
    bool finEjecucion = false;
    bool recorridoTerminado = false;
    ListaVehiculos vehiculos = ListaVehiculos();
    ListaNodos clientesVisitados = ListaNodos();
    double *distanciaAlCliente = (double*)malloc(sizeof(double));
    Nodo *nodoClienteCercano, *nodoSiguiente;
    int j = 0;
    while(!finEjecucion){
        //Crear vehículo en lista de vehículos
        Vehiculo vehi = Vehiculo();
        vehiculos.moveToEnd();
        vehiculos.insert(vehi);
        vehiculos.moveToEnd();

        //Inicializar vehiculo en depot
        Vehiculo *vehiculoActual = &vehiculos.curr->data;
        vehiculoActual->agregarParada(depot,inst.velocidad,0.0,0,0);

        recorridoTerminado = false;
        while(!recorridoTerminado){
            //Obtener último nodo del recorrido del vehículo (el nodo en el que está actualmente)
            vehiculoActual->recorrido.moveToEnd();
            Nodo nodoVehiculoActual = vehiculoActual->recorrido.curr->data;

            //Buscar el cliente a menor distancia no asignado a otros vehículos
            nodoClienteCercano = nodoMenorDistancia(nodoVehiculoActual, clientes, inst.numClientes, clientesVisitados, distanciaAlCliente);
            
            //--------Comprobación de restricciones--------

            //Si no hay cliente disponible (todos asignados), se debe terminar todo
            if(!nodoClienteCercano){
                nodoSiguiente = &depot;
                recorridoTerminado = true;
                finEjecucion = true;
            }  
            else{
                //Ver si se tiene tiempo para llegar directamente
                double tiempoEnLlegar = *distanciaAlCliente/inst.velocidad;
                if(vehiculoActual->tiempoTranscurrido+tiempoEnLlegar+inst.tiempoServicio < inst.maxTiempo){
                    //Chequear combustible suficiente
                        //Si no se cumple, buscar estación más cercana desde la que haya tiempo suficiente para volver al depot

                    //Si se tiene, ver si desde ese cliente se tendría combustible para llegar a estación más cercana
                    //Además, chequear si se tiene tiempo y combustible para volver directamente al depot
                    //O, si se tiene tiempo para pasar a una estación antes de pasar al depot
                        
                        /*BACKTRACKING???????? CUANDO LO HAGO?????*/
                    nodoSiguiente = nodoClienteCercano;//Sólo hacer esto si se cumple todo
                }
                else{
                    nodoSiguiente = &depot;
                    recorridoTerminado = true;
                }
            }

            //Asignar al final del recorrido de vehículo el nodo correspondiente
            vehiculoActual->recorrido.moveToEnd();
            vehiculoActual->agregarParada(*nodoSiguiente, inst.velocidad, *distanciaAlCliente, inst.tiempoServicio, inst.tiempoRecarga);

            //Además, agregar este nodo a lista de clientes visitados
            clientesVisitados.insert(*nodoSiguiente);
        }
        j++;
        if(j==6) finEjecucion = true;
    }

    time(&end);
    vehiculos.moveToStart();
    vehiculos.next();
    for(unsigned int i = 0;i<vehiculos.listSize;i++){
        vehiculos.curr->data.recorrido.print();
        cout << "Distancia recorrida: "<< vehiculos.curr->data.distanciaTotalRecorrida<<"\n";
        cout << "Tiempo transcurrido: "<< vehiculos.curr->data.tiempoTranscurrido<<"\n";
        vehiculos.next();
    }

    /*
    //Generar archivo solución:
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
    */
    
    //Liberar memoria
    free(distanciaAlCliente);
  return 0;
} 


