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
        Nodo *next;

        Nodo();
        void mostrar();   
};

Nodo::Nodo(){
    ID = -1;
    tipo = '\0';
    longitud = 0.0;
    latitud = 0.0;
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
        Nodo remove(); //remover nodo en pos actual
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

Nodo ListaNodos::remove(){
    Nodo eliminado;
    if(curr==tail) return eliminado;
    if(curr->next == tail) tail = curr;
    tPaso *aux = curr->next;
    eliminado = aux->data;
    curr->next = curr->next->next;
    free(aux);
    return eliminado;
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
    moveToStart();
    for(unsigned int i=0;i<listSize+1;i++){
        if(!curr) break;
        tPaso *aux = curr;
        curr = curr->next;
        free(aux);
    }
}

string ListaNodos::to_string(){
    string output;
    moveToStart();
    next();
    output = "\n";
    while(curr != tail){
        output += std::to_string(curr->data.ID) + std::to_string(curr->data.tipo) + " -> ";
        next();
    }
    output += std::to_string(curr->data.ID) + std::to_string(curr->data.tipo);
    output += "\n";
    return output;
}


// Clases y métodos relativas a vehículos:

class Vehiculo{
    public:
        ListaNodos recorrido;
        int tiempoTranscurrido;
        double distanciaTotalRecorrida;
        double distanciaDesdeRecarga;

        Vehiculo();
        void agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga);
};

Vehiculo::Vehiculo(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
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
    }
}

typedef struct tVehi{
    Vehiculo data;
    struct tVehi *next;
} tVehi;

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
    Vehiculo remove(); //remover nodo en pos actual
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

Vehiculo ListaVehiculos::remove(){
    Vehiculo eliminado;
    if(curr==tail) return eliminado;
    if(curr->next == tail) tail = curr;
    tVehi *aux = curr->next;
    eliminado = aux->data;
    curr->next = curr->next->next;
    free(aux);
    return eliminado;
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
