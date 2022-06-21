#include <math.h>
#include <iostream>

using namespace std;

//Clase que representa a la instancia:

class Instancia{
    public:
        string nombre;
        int numClientes;
        int numEstaciones;
        int maxTiempo;
        double maxDistancia;
        double velocidad;
        int tiempoServicio;
        int tiempoRecarga;

        Instancia();
        void mostrar();     
};

Instancia::Instancia(){
    nombre = "";
    numClientes = 0;
    numEstaciones = 0;
    maxTiempo = 0;
    maxDistancia = 0.0;
    velocidad = 0.0;
    tiempoServicio = 0;
    tiempoRecarga = 0;
}

void Instancia::mostrar(){
    cout << "\nNombre: " << nombre;
    cout << "\nNumClientes: " << numClientes;
    cout << "\nNumEstaciones: " << numEstaciones;
    cout << "\nMaxTiempo: " << maxTiempo;
    cout << "\nMaxDistancia: " << maxTiempo;
    cout << "\nVelocidad: " << velocidad;
    cout << "\nTiempoServicio: " << tiempoServicio;
    cout << "\nTiempoRecarga: " << tiempoRecarga << "\n";
}


//Clases y métodos relativas a los nodos de la instancia:

class Nodo{
    public:
        int ID;
        char tipo;
        double longitud;
        double latitud;
        //bool restringidoTemp;
        Nodo *next;

        Nodo();
        void mostrar();   
};

Nodo::Nodo(){
    ID = -1;
    tipo = '\0';
    longitud = 0.0;
    latitud = 0.0;
    //restringidoTemp = false;
}

void Nodo::mostrar(){
    cout << "\nID: " << ID;
    cout << "\nTipo: " << tipo;
    cout << "\nLongitud: " << longitud;
    cout << "\nLatitud: " << latitud << "\n";
}

typedef struct tPaso{
    Nodo data;
    struct tPaso *next;
} tPaso;

//ListaNodos es una lista enlazada

class ListaNodos{
    public:
        tPaso *head;
        tPaso *tail;
        tPaso *curr; // curr apunta al nodo anterior al actual
        unsigned int listSize;
        unsigned int pos;
    
        ListaNodos();
        int insert(Nodo item); //insertar en pos actual
        void remove(); //remover nodo en pos actual
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        string to_string();
};

ListaNodos::ListaNodos(){
    head = tail = curr = (tPaso*)malloc(sizeof(tPaso)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
}

int ListaNodos::insert(Nodo item){
    tPaso *aux = curr->next;
    curr->next = (tPaso*)malloc(sizeof(tPaso));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
    return pos;
}

void ListaNodos::remove(){
    if(curr==tail) return;
    if(curr->next == tail) tail = curr;
    tPaso *aux = curr->next;
    curr->next = curr->next->next;
    listSize--;
    free(aux);
}

void ListaNodos::moveToStart(){curr=head;pos=0;}

void ListaNodos::moveToEnd(){curr=tail;pos=listSize;}

void ListaNodos::prev(){
    tPaso *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void ListaNodos::next(){if (curr != tail) curr = curr->next; pos++;}

void ListaNodos::clear(){
    if(listSize!=0){
        moveToStart();
        while(listSize>0){
            remove();
        }
    }
}

string ListaNodos::to_string(){
    string output = "\n";
    moveToStart();
    next();
    if(listSize != 0){
        while(curr != tail){
            output += std::to_string(curr->data.ID) + curr->data.tipo + "-";
            next();
        }
        output += std::to_string(curr->data.ID) + curr->data.tipo;
        while(output.length()<50){
            output += " ";
        }
    }
    else{
        output = "No hay nodos en la lista";
    }
    return output;
}


// Clases y métodos relativas a vehículos:

class Vehiculo{
    public:
        ListaNodos recorrido;
        int tiempoTranscurrido;
        double distanciaTotalRecorrida;
        double distanciaDesdeRecarga;
        unsigned int cantClientesVisitados;

        Vehiculo();
        void agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga);
        void reiniciarRecorrido();
};

Vehiculo::Vehiculo(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
}

void Vehiculo::agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga){
    recorrido.insert(nodo);
    tiempoTranscurrido += distancia/velocidad;
    distanciaTotalRecorrida += distancia;
    distanciaDesdeRecarga += distancia;
    if(nodo.tipo=='f') {
        distanciaDesdeRecarga = 0.0;
        tiempoTranscurrido += tiempoRecarga;
    }
    else if(nodo.tipo=='c'){
        tiempoTranscurrido += tiempoServicio;
        cantClientesVisitados++;
    }
}

typedef struct tVehi{
    Vehiculo data;
    struct tVehi *next;
} tVehi;

void Vehiculo::reiniciarRecorrido(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
    recorrido.clear();
}

//ListaVehiculos tambien es una lista enlazada

class ListaVehiculos{
    public:
    tVehi *head;
    tVehi *tail;
    tVehi *curr; // curr apunta al nodo anterior al actual
    unsigned int listSize;
    unsigned int pos;

    ListaVehiculos();
    int insert(Vehiculo item); //insertar en pos actual
    void remove();
    void moveToStart();
    void moveToEnd();
    void prev();
    void next();
    void clear();
};

ListaVehiculos::ListaVehiculos(){
    head = tail = curr = (tVehi*)malloc(sizeof(tVehi)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
}

int ListaVehiculos::insert(Vehiculo item){
    tVehi *aux = curr->next;
    curr->next = (tVehi*)malloc(sizeof(tVehi));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
    return pos;
}

void ListaVehiculos::remove(){
    if(curr==tail) return;
    if(curr->next == tail) tail = curr;
    tVehi *aux = curr->next;
    curr->next = curr->next->next;
    listSize--;
    free(aux);
}

void ListaVehiculos::moveToStart(){curr=head;pos=0;}

void ListaVehiculos::moveToEnd(){curr=tail;pos=listSize;}

void ListaVehiculos::prev(){
    tVehi *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void ListaVehiculos::next(){if (curr != tail) curr = curr->next; pos++;}

void ListaVehiculos::clear(){
    moveToStart();
    for(unsigned int i=0;i<listSize+1;i++){
        if(!curr) break;
        tVehi *aux = curr;
        curr = curr->next;
        free(aux);
    }
}


ListaNodos concatenar(ListaNodos *lista1, ListaNodos *lista2){
    ListaNodos concatenacion = ListaNodos();
    lista1->moveToStart();
    lista1->next();
    lista2->moveToStart();
    lista2->next();
    if(lista1->listSize>0 && lista2->listSize>0){
        for(unsigned int i=0;i<lista1->listSize;i++){
            concatenacion.insert(lista1->curr->data);
            lista1->next();
            concatenacion.next();
        }
        for(unsigned int i=0;i<lista2->listSize;i++){
            concatenacion.insert(lista2->curr->data);
            lista2->next();
            concatenacion.next();
        }
    }
    else if(lista1->listSize>0){
        concatenacion = *lista1;
    }
    else if(lista2->listSize>0){
        concatenacion = *lista2;
    }
    return concatenacion;
}