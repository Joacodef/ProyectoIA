#include <cstring>
#include <bits/stdc++.h>
#define MAX_INTENTOS 200
#define MAX_ITER_TOTALES 5

using namespace std;

// Funcion para generar recorridos para un vehiculo, de acuerdo con restricciones del GVRP

void loopGeneracionRecorrido(Nodo depot, Nodo *estaciones, Nodo *clientes, Instancia inst, Vehiculo *vehiculoActual, ListaNodos *clientesVisitados, ListaNodos nodosRestringidos){
    bool recorridoTerminado = false;
    double *distanciaAlCliente = (double*)malloc(sizeof(double));
    double *distanciaAEstacion = (double*)malloc(sizeof(double));
    Nodo *nodoClienteCercano, *nodoSiguiente;
    while(!recorridoTerminado){
        //Obtener último nodo del recorrido del vehículo (el nodo en el que está actualmente)
        vehiculoActual->recorrido.moveToEnd();
        Nodo nodoVehiculoActual = vehiculoActual->recorrido.curr->data;

        //--------Comprobación de restricciones--------

        //Si no hay clientes disponible (todos asignados), se debe terminar todo
        if(clientesVisitados->listSize >= abs(inst.numClientes)){
            nodoSiguiente = &depot;
            recorridoTerminado = true;
        }  
        else{
            
            //Buscar el cliente a menor distancia no asignado a otros vehículos
            nodoClienteCercano = nodoMenorDistancia(nodoVehiculoActual, clientes, inst.numClientes, nodosRestringidos, distanciaAlCliente);
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

                        //Comprobar todas las estaciones por alguna que de tiempo suficiente como para volver al depot                        
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

                    //Si se tiene combustible, chequear si se tiene tiempo para volver directamente al depot desde el cliente nuevo
                    double distanciaClienteDepot = calcularDistancia(nodoClienteCercano->longitud,nodoClienteCercano->latitud,depot.longitud,depot.latitud);
                    double tiempoClienteYDepot = (distanciaClienteDepot+*distanciaAlCliente)/inst.velocidad;
                    if(vehiculoActual->tiempoTranscurrido + tiempoClienteYDepot + inst.tiempoServicio > inst.maxTiempo){
                        //No se tiene tiempo para ir al cliente mas cercano y volver al depot, se debe vuelve en seguida al depot                            
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
        if(nodoSiguiente->tipo == 'c'){
            clientesVisitados->moveToStart();
            clientesVisitados->insert(*nodoSiguiente);
            nodosRestringidos.insert(*nodoSiguiente);
        }
    }
    free(distanciaAlCliente);
    free(distanciaAEstacion);
}

//Función que asigna a los vehículos sus recorridos, buscando que se cumplan las restricciones y aplicando backtracking para buscar mejores soluciones

ListaVehiculos loopGeneracionSolucion(Nodo depot, Nodo *estaciones, Nodo *clientes, Instancia inst, double eficienciaEsperada){
    bool finEjecucion = false;
    bool backTracking = false;
    double eficienciaReco = 0.0;
    int final = 0;
    ListaVehiculos vehiculos = ListaVehiculos();
    ListaNodos clientesVisitados = ListaNodos();
    ListaNodos clientesRestringidosTemp = ListaNodos();
    ListaNodos clientesRestringidosBT = ListaNodos();
    int numIntentos = 0;
    int posicionDeRestriccion = 0; 
    while(!finEjecucion){
        //Comprobar si ya todos los clientes han sido asignados, en cuyo caso termina la ejecución
        Vehiculo *vehiculoActual;
        if(clientesVisitados.listSize >= abs(inst.numClientes)) finEjecucion = true;
        else{
            numIntentos = 0;
            int iter2 = 0;
            if(backTracking){

                //Si en la iteración anterior se decidió volver atrás, debemos eliminar el recorrido del vehiculo que se había creado
                vehiculoActual = &vehiculos.curr->data;
                clientesVisitados.moveToStart();
                for(unsigned int i=0;i<vehiculoActual->cantClientesVisitados;i++){
                    clientesVisitados.remove();
                }
                vehiculoActual->recorrido.moveToEnd();
                vehiculoActual->recorrido.prev();
                //Segunda "capa" de restricciones: se restringe el último nodo visitado en el recorrido que ahora se va a eliminar
                /*
                while(vehiculoActual->recorrido.curr->data.tipo == 'f' || vehiculoActual->recorrido.curr->data.tipo == 'd'){
                    if(vehiculoActual->recorrido.curr != vehiculoActual->recorrido.head->next){
                        vehiculoActual->recorrido.prev();
                    }
                    else{
                        vehiculoActual->recorrido.moveToEnd();
                    }
                    iter2++;
                    if(iter2>4) break;
                }
                if(iter2<=4){*/
                    clientesRestringidosBT.insert(vehiculoActual->recorrido.curr->data);
                    cout << "Recorrido de vehiculo anterior:\n" << vehiculoActual->recorrido.to_string()<<"\n";
                    cout << "Nuevo restringido: " << clientesRestringidosBT.to_string() << "\n";
                /*}
                else{

                    //Si el recorrido que se había hecho antes, por alguna razón, sólo contenía el depot 
                    //y una parada en estación, se hace backtracking de nuevo
                    numIntentos = MAX_INTENTOS;
                }*/
                vehiculoActual->reiniciarRecorrido();
                backTracking = false;
            }
            else{

                //Si simplemente estamos creando un nuevo vehículom, se debe crear vehículo en lista de vehículos
                Vehiculo vehi = Vehiculo();
                vehiculos.moveToEnd();
                vehiculos.insert(vehi);
                vehiculos.moveToEnd();
                //Inicializar vehiculo en depot
                vehiculoActual = &vehiculos.curr->data;
            }
            vehiculoActual->agregarParada(depot,inst.velocidad,0.0,0,0);
            
            //-----Generación de Recorridos, comprobación de eficiencia, backtracking-----

            /*Generar recorrido para el vehículo, se debe cumplir la condicion de eficiencia tras 100 iteraciones, de lo contrario, se vuelve
              a la ruta anterior y la genera de nuevo restringiendo uno de los clientes de su ruta al azar: este es el BACKTRACKING*/
            eficienciaReco = 0.0;
            //La siguiente variable sirve para especificar la posicion del valor que sera restringido. Si es cero, será el último elemento del recorrido,
            //si es uno, será el penúltimo, y así sucesivamente.
            posicionDeRestriccion = 0;
            clientesRestringidosTemp.clear();
            while(eficienciaReco < eficienciaEsperada && numIntentos < MAX_INTENTOS){
                cout << "\nIteracion nueva\n";
                cout << "clientes visitados antes de generacion de recorrido: " << clientesVisitados.to_string() <<"\n";
                cout << "clientes restringidos: "<< clientesRestringidosTemp.to_string()<<"\n";
                ListaNodos visitadosYRestringidos = concatenar(&clientesVisitados, &clientesRestringidosTemp);
                visitadosYRestringidos = concatenar(&visitadosYRestringidos, &clientesRestringidosBT);
                cout << "concatenacion de las 3 cosas: "<< visitadosYRestringidos.to_string()<<"\n";
                
                loopGeneracionRecorrido(depot, estaciones, clientes, inst, vehiculoActual, &clientesVisitados, visitadosYRestringidos);
                
                eficienciaReco = vehiculoActual->recorrido.listSize/vehiculoActual->distanciaTotalRecorrida;
                cout<<"\nNuevo recorrido del vehiculo "<< vehiculoActual->recorrido.to_string() <<"\n";
                if(eficienciaReco < eficienciaEsperada){

                    //Si no se alcanza la eficiencia, se prueba restringir los valores de los últimos clientes del recorrido, que tienden a ser los más ineficientes.
                    //Luego se vuelve a generar de nuevo un recorrido para el vehculo actual
                    
                    if(clientesVisitados.listSize + clientesRestringidosTemp.listSize + clientesRestringidosBT.listSize < abs(inst.numClientes)){
                        
                        //Se deben quitar los nodos que haya visitado 
                        clientesVisitados.moveToStart();
                        for(unsigned int i=0;i<vehiculoActual->cantClientesVisitados;i++){
                            clientesVisitados.remove();
                        }
                        //Agregar nodo a restringir a la lista de nodos restringidos, que es temporal, se resetea cuando ya hayamos restringido todos los clientes o terminado de iterar.
                        int iter = 0;
                        if(abs(posicionDeRestriccion)<vehiculoActual->recorrido.listSize-1){
                            vehiculoActual->recorrido.moveToEnd();
                            for(int i=0; i<=posicionDeRestriccion; i++){
                                vehiculoActual->recorrido.prev();
                            }   /*                         
                            while(vehiculoActual->recorrido.curr->data.tipo == 'f' || vehiculoActual->recorrido.curr->data.tipo == 'd'){

                                //Si el que queremos elminar resulta que es una estacion de recarga o depot, elegimos otro adyacente
                                if(vehiculoActual->recorrido.curr != vehiculoActual->recorrido.head->next){
                                    vehiculoActual->recorrido.prev();
                                }
                                else{
                                    vehiculoActual->recorrido.moveToEnd();
                                }
                                iter++;
                                if(iter>4) break;
                            }*/
                        }
                        else{
                            break;
                        }
                        //if(iter<=4){
                            clientesRestringidosTemp.insert(vehiculoActual->recorrido.curr->data);
                            numIntentos++; 
                        /*}
                        else{
                           
                            //Si no hay nodos que se puedan agregar, se deja de intentar con este recorrido (se hace backtracking)
                            numIntentos = MAX_INTENTOS;
                            
                        }*/
                        vehiculoActual->reiniciarRecorrido();
                        vehiculoActual->agregarParada(depot,inst.velocidad,0.0,0,0);                              
                    }
                    else{

                        //Si ya se restringieron todos los nodos clientes, se vuelve a empezar, pero restringiendo el cliente que este una posicion mas atras que antes.
                        
                        clientesRestringidosTemp.clear();
                        posicionDeRestriccion++;
                        clientesVisitados.moveToStart();
                        for(unsigned int i=0;i<vehiculoActual->cantClientesVisitados;i++){
                            clientesVisitados.remove();
                        }
                        vehiculoActual->reiniciarRecorrido();
                        vehiculoActual->agregarParada(depot,inst.velocidad,0.0,0,0);
                        numIntentos++; 
                    }
                }
                else{
                    cout << "\n\n\n\n\n===========EFICIENCIA ALCANZADA, PASANDO A SIGUIENTE VEHICULO============\n\n\n\n\n\n";
                    clientesRestringidosTemp.clear();
                }
            }
        }
        if(eficienciaReco < eficienciaEsperada){

            //Si no se pudo alcanzar la eficiencia, se vuelve al recorrido anterior, y se restringe un nodo al azar de ese recorrido
            //Antes se asegura de eliminar las listas hechas durante el intento de recorrido actual
            clientesRestringidosTemp.clear();
            clientesVisitados.moveToStart();
            for(unsigned int i=0;i<vehiculoActual->cantClientesVisitados;i++){
                clientesVisitados.remove();
            }
            vehiculoActual->reiniciarRecorrido();
            vehiculos.moveToEnd();
            vehiculos.prev();
            vehiculos.remove();
            cout << "\n\n\n\n\n===========REALIZANDO BACKTRACKING===========\n\n\n\n\n";
            backTracking = true;
        }
        clientesRestringidosBT.clear();
        final++;
        if(final >= MAX_ITER_TOTALES){
            finEjecucion = true; 
        }
    }

    clientesVisitados.clear();
    clientesRestringidosTemp.clear();
    clientesRestringidosBT.clear();

    return vehiculos;
}