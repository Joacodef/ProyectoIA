#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "classes.h"
#include "functions.h"

using namespace std;

int main() {

    //-----EXTRACCIÓN DE DATOS DE ARCHIVOS:-----
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
    double *distanciaAEstacion = (double*)malloc(sizeof(double));
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

            
            //--------Comprobación de restricciones--------


            //Si no hay clientes disponible (todos asignados), se debe terminar todo
            if(clientesVisitados.listSize == abs(inst.numClientes)){
                nodoSiguiente = &depot;
                recorridoTerminado = true;
                finEjecucion = true;
            }  
            else{
                //Buscar el cliente a menor distancia no asignado a otros vehículos
                nodoClienteCercano = nodoMenorDistancia(nodoVehiculoActual, clientes, inst.numClientes, clientesVisitados, distanciaAlCliente);
                //Ver si se tiene tiempo para llegar directamente
                double tiempoEnLlegar = *distanciaAlCliente/inst.velocidad;
                if(vehiculoActual->tiempoTranscurrido+tiempoEnLlegar+inst.tiempoServicio < inst.maxTiempo){
                    //Chequear combustible suficiente
                    if(vehiculoActual->distanciaDesdeRecarga+*distanciaAlCliente > inst.maxDistancia){
                        // Si no hay combustible, buscar estación más cercana desde la que haya tiempo suficiente para volver al depot
                        bool EstacionEncontrada = false;
                        ListaNodos estacionesNoPermitidas = ListaNodos();
                        while(!EstacionEncontrada){
                            if(estacionesNoPermitidas.listSize == abs(inst.numEstaciones)){
                                //No hay estaciones que me permitan volver al depot (no puedo volver al depot)
                                //HACER BACKTRACKING?
                                nodoSiguiente = &depot;
                                recorridoTerminado = true;
                                break;
                            }
                            //Comprobar todas las estaciones, si ninguna cumple, se vuelve al depot                            
                            nodoSiguiente = nodoMenorDistancia(nodoVehiculoActual, estaciones, inst.numEstaciones, estacionesNoPermitidas, distanciaAEstacion);
                            double distDepotEst = calcularDistancia(nodoSiguiente->longitud,nodoSiguiente->latitud,depot.longitud,depot.latitud);
                            double tiempoActualEst = *distanciaAEstacion/inst.velocidad;
                            double tiempoDepotEst = distDepotEst/inst.velocidad;
                            if(vehiculoActual->tiempoTranscurrido + tiempoDepotEst + inst.tiempoRecarga + tiempoActualEst < inst.maxTiempo){
                                EstacionEncontrada = true;
                            }
                            else{
                                estacionesNoPermitidas.insert(*nodoSiguiente);
                            }
                        }
                    }
                    else{
                        //Si se tiene combustible, chequear si se tiene tiempo para volver directamente al depot desde el cliente
                        double distanciaClienteDepot = calcularDistancia(nodoClienteCercano->longitud,nodoClienteCercano->latitud,depot.longitud,depot.latitud);
                        double tiempoClienteYDepot = (distanciaClienteDepot+*distanciaAlCliente)/inst.velocidad;
                        if(vehiculoActual->tiempoTranscurrido + tiempoClienteYDepot + inst.tiempoServicio > inst.maxTiempo){
                            //No se tiene tiempo para ir al cliente mas cercano y volver, se debe vuelve en seguida al depot
                            nodoSiguiente = &depot;
                            recorridoTerminado = true;
                        }
                        else{
                            //Si se tiene tiempo, ver si desde ese cliente se tendría combustible para llegar a estación más cercana (sino se quedaría "atascado" en el cliente)
                            ListaNodos listaAux = ListaNodos();
                            Nodo *estacionCercana = nodoMenorDistancia(*nodoClienteCercano,estaciones,inst.numEstaciones,listaAux,distanciaAEstacion);
                            if(vehiculoActual->distanciaDesdeRecarga + *distanciaAlCliente + *distanciaAEstacion > inst.maxDistancia){
                                //Ya que no alcanza, se debe recargar combustible o volver al depot, para decidir, se chequea que el tiempo restante sea suficiente
                                double tiempoEstacion = *distanciaAEstacion / inst.velocidad;
                                if(vehiculoActual->tiempoTranscurrido + inst.tiempoRecarga + tiempoEstacion > inst.maxTiempo){
                                    //No se puede ir a la estacion de recarga por falta de tiempo, entonces se debe volver al depot, aun si se excede distancia
                                    nodoSiguiente = &depot;
                                    recorridoTerminado = true;
                                }
                                else{
                                    //Se elige recargar combustible en vez de ir al cliente
                                    nodoSiguiente = estacionCercana;
                                }
                            }
                            else{
                                //Si se cumplen todas las restricciones, se va al cliente más cercano
                                nodoSiguiente = nodoClienteCercano;
                            }
                        }                        
                    }                   
                }
                else{
                    //Si no se tiene tiempo para llegar directamente, se vuelve al depot
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
        if(j==100) finEjecucion = true;
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
    free(distanciaAEstacion);
  return 0;
} 


