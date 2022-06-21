#include <cstring>
#include <bits/stdc++.h>

using namespace std;

ListaVehiculos loopPrincipal(Nodo depot, Nodo *estaciones, Nodo *clientes, Instancia inst){
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

                                //No hay estaciones que me permitan volver al depot, entonces se vuelve al depot aunque se rompan restrincciones
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

    //Liberar memoria
    free(distanciaAlCliente);
    free(distanciaAEstacion);
    clientesVisitados.clear();

    return vehiculos;
}