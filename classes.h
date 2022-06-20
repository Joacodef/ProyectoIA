#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
using namespace std;


//Clases que almacenan de datos de archivos:
class Nodo{
    public:
        int ID;
        char tipo;
        double longitud;
        double latitud;
        bool visitado;
        Nodo *next;

        Nodo();
        void mostrar();   
};

Nodo::Nodo(){
    ID = 0;
    tipo = '\0';
    longitud = 0.0;
    latitud = 0.0;
    visitado = false;
}

void Nodo::mostrar(){
    cout << "\nID: " << ID;
    cout << "\nTipo: " << tipo;
    cout << "\nLongitud: " << longitud;
    cout << "\nLatitud: " << latitud << "\n";
}


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



//Funciones que extraen datos de archivos:
Instancia extraerInstancia(ifstream& archivo){
    Instancia inst;
    archivo >> inst.nombre;
    archivo >> inst.numClientes;
    archivo >> inst.numEstaciones;
    archivo >> inst.maxTiempo;
    archivo >> inst.maxDistancia;
    archivo >> inst.velocidad;
    archivo >> inst.tiempoServicio;
    archivo >> inst.tiempoRecarga;
    return inst;
}

void extraerNodos(ifstream& archivo, int numEstaciones, int numClientes, Nodo* nodos){
    Nodo *nodosAux= (Nodo*)malloc(sizeof(Nodo)*(numClientes+numClientes+1));
    int i = 1;
    //Siguientes lineas del archivo:
    while(i <= numClientes+numEstaciones+1){
        //Inicializar nodo para que valgrind no alegue:
        nodosAux[i-1] = Nodo();
        archivo >> nodosAux[i-1].ID;
        archivo >> nodosAux[i-1].tipo;
        archivo >> nodosAux[i-1].longitud;
        archivo >> nodosAux[i-1].latitud;
        i++;
    }
    memcpy(nodos,nodosAux,sizeof(Nodo)*(numClientes+numClientes+1));
    free(nodosAux);
}



//Clases y structs para generar las soluciones, Recorrido es una lista enlazada:
typedef struct tPaso{
    Nodo data;
    struct tPaso *next;
} tPaso;

class Recorrido{
    public:
        tPaso *head;
        tPaso *tail;
        tPaso *curr; // curr apunta al nodo anterior al actual
        unsigned int listSize;
        unsigned int pos;
    
        Recorrido();
        int insert(Nodo item); //insertar en pos actual
        Nodo remove(); //remover nodo en pos actual
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        void print();
};

Recorrido::Recorrido(){
    head = tail = curr = (tPaso*)malloc(sizeof(tPaso)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
}

int Recorrido::insert(Nodo item){
    tPaso *aux = curr->next;
    curr->next = (tPaso*)malloc(sizeof(tPaso));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
    return pos;
}

Nodo Recorrido::remove(){
    Nodo eliminado;
    if(curr==tail) return eliminado;
    if(curr->next == tail) tail = curr;
    tPaso *aux = curr->next;
    eliminado = aux->data;
    curr->next = curr->next->next;
    free(aux);
    return eliminado;
}

void Recorrido::moveToStart(){curr=head;pos=0;}

void Recorrido::moveToEnd(){curr=tail;pos=listSize;}

void Recorrido::prev(){
    tPaso *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void Recorrido::next(){if (curr != tail) curr = curr->next; pos++;}

void Recorrido::clear(){
    moveToStart();
    for(unsigned int i=0;i<listSize+1;i++){
        if(!curr) break;
        tPaso *aux = curr;
        curr = curr->next;
        free(aux);
    }
}

void Recorrido::print(){
    moveToStart();
    next();
    cout <<"\n";
    while(curr != tail){
        cout << curr->data.ID << curr->data.tipo << " -> ";
        next();
    }
    cout << curr->data.ID << curr->data.tipo;
    cout << "\n";
}

class Vehiculo{
    public:
        Recorrido recorrido;
        int tiempoRecorrido;
        double distanciaRecorrida;

        Vehiculo();
};

Vehiculo::Vehiculo(){
    tiempoRecorrido = 0;
    distanciaRecorrida = 0.0;
}