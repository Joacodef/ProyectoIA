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
        Nodo *next;

        void mostrar(){
            cout << "\nID: " << ID;
            cout << "\nTipo: " << tipo;
            cout << "\nLongitud: " << longitud;
            cout << "\nLatitud: " << latitud << "\n";
        }
};

class Instancia{
    public:
        string nombre;
        int numClientes;
        int numEstaciones;
        int maxTiempo;
        int maxDistancia;
        double velocidad;
        int tiempoServicio;
        int tiempoRecarga;

        void mostrar(){
            cout << "\nNombre: " << nombre;
            cout << "\nNumClientes: " << numClientes;
            cout << "\nNumEstaciones: " << numEstaciones;
            cout << "\nMaxTiempo: " << maxTiempo;
            cout << "\nMaxDistancia: " << maxTiempo;
            cout << "\nVelocidad: " << velocidad;
            cout << "\nTiempoServicio: " << tiempoServicio;
            cout << "\nTiempoRecarga: " << tiempoRecarga << "\n";
        }
};



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
        archivo >> nodosAux[i-1].ID;
        archivo >> nodosAux[i-1].tipo;
        archivo >> nodosAux[i-1].longitud;
        archivo >> nodosAux[i-1].latitud;
        i++;
    }
    memcpy(nodos,nodosAux,sizeof(Nodo)*(numClientes+numClientes+1));
    free(nodosAux);
}



//Clases y structs para generar las soluciones:
typedef struct tPaso{
    Nodo data;
    struct tPaso *next;
} tPaso;

class Recorrido{
    tPaso *head;
    tPaso *tail;
    tPaso *curr; // curr apunta al nodo anterior al actual
    unsigned int listSize;
    unsigned int pos;
    public:
        Recorrido();
        int insert(Nodo item); //insertar en pos actual
        Nodo remove(); //remover nodo en pos actual
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
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
    tail = head;
    listSize = 0;
    while(curr != NULL){
        tPaso *aux = curr;
        curr = curr->next;
        free(aux);
    }
    curr = head;
}

class Vehiculo{
    private:
        Recorrido recorrido;
        int tiempoRecorrido;
        double distanciaRecorrida;
};
